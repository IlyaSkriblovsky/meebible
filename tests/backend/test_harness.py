"""Check that the test tools reject real regressions without a running backend."""

import sqlite3
import tempfile
import unittest
from types import SimpleNamespace
from unittest.mock import patch
from pathlib import Path

from cases import cases, form
from run import check_case, clean_error, compare
from support import Page, Response, delta, local_url, user_data


class HarnessChecks(unittest.TestCase):
    def test_translation_nonclient_cases_are_not_required_for_python(self):
        required = {case["id"] for case in cases("python")}
        extended = {case["id"] for case in cases("python", include_optional=True)}
        observations = {"translation-zero", "translation-bare"}
        optional = {"translation-array", "translation-duplicate"}
        self.assertTrue(required.isdisjoint(observations | optional))
        self.assertTrue(extended.isdisjoint(observations))
        self.assertEqual(extended - required, optional)
        self.assertTrue({"chapter-array", "chapter-duplicate", "translation-unknown",
                         "translation-sql-text"}.issubset(required))
        php = {case["id"] for case in cases("php")}
        self.assertTrue((observations | optional).issubset(php))
        self.assertEqual(php, {case["id"] for case in cases("php", include_optional=True)})

    def test_optional_translation_checks_keep_their_strict_validation(self):
        args = SimpleNamespace(profile="python", user_data=None, php_reference=False,
                               base_url="http://127.0.0.1:8000")
        for case in cases("python", include_optional=True):
            if not case.get("python_optional"):
                continue
            with self.subTest(case=case["id"]):
                response = Response(400, [("Content-Type", "text/plain")], b"", b"Bad input")
                with patch("run.request", return_value=response):
                    check_case(args, {}, {}, case)
                response.status = 200
                with patch("run.request", return_value=response), self.assertRaises(AssertionError):
                    check_case(args, {}, {}, case)

    def test_never_accept_a_public_target(self):
        for url in ("http://meebible.org", "https://127.0.0.1", "http://127.0.0.1@meebible.org",
                    "http://127.0.0.1/api", "http://127.0.0.1?target=production"):
            with self.subTest(url=url), self.assertRaises(ValueError):
                local_url(url)
        self.assertEqual(local_url("http://[::1]:8000").hostname, "::1")

    def test_xml_allows_attribute_order_but_checks_child_order(self):
        expected = {"status": 200, "headers": [("Content-Type", "application/xml")]}
        body = b'<translation><book code="ge" name="Genesis"><chapter no="1"/><chapter no="2"/></book></translation>'
        response = Response(200, expected["headers"], b"", body.replace(b'code="ge" name="Genesis"', b'name="Genesis" code="ge"'))
        compare(response, expected, body, "xml")
        response.body = body.replace(b'no="1"', b'no="3"')
        with self.assertRaises(AssertionError):
            compare(response, expected, body, "xml")

    def test_chapter_comparison_is_byte_exact(self):
        expected = {"status": 200, "headers": [("Content-Type", "text/html;charset=UTF-8")]}
        body = b'<span verse="1">A verse.</span>\n'
        response = Response(200, expected["headers"], b"", body.rstrip())
        with self.assertRaises(AssertionError):
            compare(response, expected, body, "bytes")

    def test_automatic_json_errors_are_rejected(self):
        response = Response(400, [("Content-Type", "application/json")], b"", b'{"detail":"bad input"}')
        with self.assertRaises(AssertionError):
            clean_error(response)

    def test_passage_comparison_tracks_split_verses_and_superscripts(self):
        body = b'<title>Psalm 3</title><div class="superscript">A song</div><span verse="1">First</span><span verse="1">second</span><span class="hole">[...]</span><span verse="3">Third</span>'
        page = Page(body).value(True)
        self.assertEqual(page["verses"], [("1", "First"), ("1", "second"), ("3", "Third")])
        self.assertEqual(page["superscripts"], ["A song"])
        self.assertEqual(page["holes"], ["[...]"])
        self.assertNotEqual(page, Page(body.replace(b'verse="3"', b'verse="2"')).value(True))

    def test_historical_qt_fixture_keeps_literal_plus(self):
        self.assertEqual(form({"message": "C++ &=% é\r\n"}), "message=C++%20%26%3D%25%20%C3%A9%0D%0A")

    def test_python_home_requires_sorted_names_and_preserves_content(self):
        expected = {"status": 200, "headers": [("Content-Type", "text/html;charset=UTF-8")]}
        body = '<h2>About</h2><div class="language"><h3>English</h3><div class="translation">Zulu</div><div class="translation">Alpha</div></div><div class="language"><h3>Russian</h3><div class="translation">Beta</div></div><p>© 2011–2013 Ilya Skriblovsky</p>'.encode()
        sorted_body = body.replace(b'>Zulu</div><div class="translation">Alpha<', b'>Alpha</div><div class="translation">Zulu<')
        response = Response(200, expected["headers"], b"", sorted_body)
        compare(response, expected, body, "home")
        for broken in (
            body,
            sorted_body.replace(b'<div class="translation">Zulu</div>', b''),
            sorted_body.replace(b'<div class="translation">Alpha</div>', b'<div class="translation">Alpha</div>' * 2),
            sorted_body.replace(b'>Alpha<', b'>Beta<').replace(b'>Beta</div></div><p>', b'>Alpha</div></div><p>'),
            sorted_body.replace(b'>About<', b'>Changed<'),
        ):
            with self.subTest(body=broken), self.assertRaises(AssertionError):
                compare(Response(200, expected["headers"], b"", broken), expected, body, "home")
        response.body = sorted_body.decode().replace('2011–2013', '2011–2026').encode()
        compare(response, expected, body, "home")
        with self.assertRaises(AssertionError):
            compare(response, expected, body, "page")

    def test_python_profile_rejects_the_php_plus_data_loss(self):
        case = next(case for case in cases() if case["id"] == "feedback")
        before = {table: {"rows": [], "columns": []} for table in ("startup", "feedback")}
        after = {table: {"rows": [], "columns": []} for table in ("startup", "feedback")}
        values = {**case["python"]["values"], "email": "user tag@example.org"}
        after["feedback"]["rows"] = [{"id": 1, "time": "2026-10-24 00:00:00", **values}]
        args = SimpleNamespace(profile="python", user_data="unused", php_reference=False,
                               base_url="http://127.0.0.1:8000")
        response = Response(200, [("Content-Type", "text/plain;charset=UTF-8")], b"ok", b"ok")
        with patch("run.request", return_value=response), patch("run.user_data", side_effect=[before, after]):
            with self.assertRaisesRegex(AssertionError, "Committed form values differ"):
                check_case(args, {}, {}, case)

    def test_rejected_request_cannot_hide_a_database_write(self):
        case = next(case for case in cases() if case["id"] == "feedback-duplicate")
        before = {table: {"rows": [], "columns": []} for table in ("startup", "feedback")}
        after = {table: {"rows": [], "columns": []} for table in ("startup", "feedback")}
        after["feedback"]["rows"] = [{"id": 1, "time": "2026-10-24 00:00:00", "message": "bad write"}]
        args = SimpleNamespace(profile="python", user_data="unused", php_reference=False,
                               base_url="http://127.0.0.1:8000")
        response = Response(400, [("Content-Type", "text/plain")], b"bad input", b"bad input")
        with patch("run.request", return_value=response), patch("run.user_data", side_effect=[before, after]):
            with self.assertRaisesRegex(AssertionError, "Rejected request wrote user data"):
                check_case(args, {}, {}, case)

    def test_database_observer_sees_committed_values_without_creating_files(self):
        with tempfile.TemporaryDirectory() as temporary:
            before = user_data(temporary)
            self.assertEqual(list(Path(temporary).iterdir()), [])
            with sqlite3.connect(str(Path(temporary) / "feedback.sqlite")) as db:
                db.execute("CREATE TABLE feedback(id INTEGER PRIMARY KEY,time DATETIME,name VARCHAR,email VARCHAR,message VARCHAR)")
                db.execute("INSERT INTO feedback VALUES(1, '2026-10-24 00:00:00', '', '', ?)", ("C++\r\nالعربية",))
            after = user_data(temporary)
            self.assertEqual(delta(before, after)["feedback"][0]["message"], "C++\r\nالعربية")
            after["feedback"]["rows"][0]["message"] = "changed"
            with self.assertRaises(AssertionError):
                delta(user_data(temporary), after)


if __name__ == "__main__":
    unittest.main()
