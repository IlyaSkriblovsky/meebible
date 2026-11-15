"""Check that the test tools reject real regressions without a running backend."""

import sqlite3
import tempfile
import threading
import unittest
from concurrent.futures import ThreadPoolExecutor
from types import SimpleNamespace
from unittest.mock import Mock, patch
from pathlib import Path

from cases import cases, form
from run import check_case, clean_error, compare
from support import HTTPConnectionPool, REFERENCE, Page, Response, delta, load_reference, local_url, request, user_data


class HarnessChecks(unittest.TestCase):
    def test_bulk_connections_are_reused_per_thread_and_closed_after_workers_finish(self):
        connections = []
        barrier = threading.Barrier(2, timeout=5)

        def factory(*args, **kwargs):
            connection = Mock()
            owner = threading.get_ident()

            def send(*args, **kwargs):
                self.assertEqual(threading.get_ident(), owner)
                connection.close.assert_not_called()

            connection.request.side_effect = send
            connection.getresponse.return_value = SimpleNamespace(
                status=200, getheaders=lambda: [("Content-Length", "7")], read=lambda limit: b"chapter")
            connections.append(connection)
            return connection

        def worker(index):
            for _ in range(3):
                barrier.wait()
                self.assertEqual(client.request({"path": "/chapter"}).body, b"chapter")

        with patch("support.http.client.HTTPConnection", side_effect=factory):
            with HTTPConnectionPool("http://127.0.0.1:8000") as client:
                with ThreadPoolExecutor(max_workers=2) as executor:
                    list(executor.map(worker, range(2)))
                self.assertEqual(len(connections), 2)
                for connection in connections:
                    self.assertEqual(connection.request.call_count, 3)
                    connection.close.assert_not_called()
            for connection in connections:
                connection.close.assert_called_once()
            with self.assertRaises(AssertionError):
                client.request({"path": "/chapter"})

    def test_connection_errors_are_not_retried_and_connections_are_closed(self):
        with patch("support.http.client.HTTPConnection") as factory:
            connection = factory.return_value
            connection.request.side_effect = ConnectionRefusedError("server unavailable")
            with HTTPConnectionPool("http://127.0.0.1:8000") as client:
                with self.assertRaises(ConnectionRefusedError):
                    client.request({"path": "/chapter"})
                connection.request.assert_called_once()
                connection.close.assert_called_once()
            connection.reset_mock()
            with self.assertRaises(ConnectionRefusedError):
                request("http://127.0.0.1:8000", {"path": "/chapter"})
            connection.request.assert_called_once()
            connection.close.assert_called_once()

    def test_bulk_connection_pool_rejects_public_targets(self):
        with patch("support.http.client.HTTPConnection") as factory:
            with self.assertRaises(ValueError):
                HTTPConnectionPool("http://meebible.org")
            factory.assert_not_called()

    def test_nonclient_cases_are_not_required_for_python(self):
        required = {case["id"] for case in cases("python")}
        extended = {case["id"] for case in cases("python", include_optional=True)}
        observations = {"translation-zero", "translation-bare", "chapter-bare", "passage-bad-path", "privacy-query", "feedback-query"}
        optional = {"translation-array", "translation-duplicate", "chapter-duplicate", "chapter-method-post"}
        self.assertTrue(required.isdisjoint(observations | optional))
        self.assertTrue(extended.isdisjoint(observations))
        self.assertEqual(extended - required, optional)
        self.assertTrue({"chapter-array", "chapter-unknown-file", "chapter-traversal",
                         "translation-unknown", "translation-sql-text",
                         "startup-missing", "feedback-duplicate", "privacy", "feedback",
                         "feedback-empty", "feedback-method-head", "feedback-method-put"}.issubset(required))
        php = {case["id"] for case in cases("php")}
        self.assertTrue((observations | optional).issubset(php))
        self.assertEqual(php, {case["id"] for case in cases("php", include_optional=True)})

    def test_optional_query_checks_keep_their_strict_validation(self):
        args = SimpleNamespace(profile="python", user_data=None, php_reference=False,
                               base_url="http://127.0.0.1:8000")
        for case in cases("python", include_optional=True):
            if not case.get("python_optional") or case["python"]["kind"] != "controlled":
                continue
            with self.subTest(case=case["id"]):
                response = Response(400, [("Content-Type", "text/plain")], b"", b"Bad input")
                with patch("run.request", return_value=response):
                    check_case(args, {}, {}, case)
                response.status = 200
                with patch("run.request", return_value=response), self.assertRaises(AssertionError):
                    check_case(args, {}, {}, case)

    def test_optional_chapter_post_checks_status_allow_and_error_body(self):
        case = next(case for case in cases("python", include_optional=True)
                    if case["id"] == "chapter-method-post")
        args = SimpleNamespace(profile="python", user_data=None, php_reference=False,
                               base_url="http://127.0.0.1:8000")
        headers = [("Content-Type", "text/plain"), ("Allow", "GET, HEAD")]
        response = Response(405, headers, b"", b"Method Not Allowed")
        with patch("run.request", return_value=response):
            check_case(args, {}, {}, case)
        for broken in (
            Response(200, headers, b"", b"chapter content"),
            Response(405, [("Content-Type", "text/plain"), ("Allow", "GET")], b"", b"Method Not Allowed"),
            Response(405, [("Content-Type", "application/json"), ("Allow", "GET, HEAD")], b"", b'{"detail":"Method Not Allowed"}'),
        ):
            with self.subTest(response=broken), patch("run.request", return_value=broken), self.assertRaises(AssertionError):
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

    def test_sis_range_allows_both_types_but_keeps_range_and_byte_checks(self):
        manifest = load_reference()
        indexed = {entry["request"]["id"]: entry for entry in manifest["cases"]}
        case = next(case for case in cases("python") if case["id"] == "static-range")
        entry = indexed[case["id"]]
        body = (REFERENCE / entry["body_file"]).read_bytes()
        args = SimpleNamespace(profile="python", user_data=None, php_reference=False,
                               base_url="http://127.0.0.1:8000")

        def headers(mime):
            return [(key, mime if key.lower() == "content-type" else value)
                    for key, value in entry["response"]["headers"]]

        for mime in ("application/octet-stream", "application/vnd.symbian.install"):
            response = Response(206, headers(mime), body, body)
            with self.subTest(mime=mime), patch("run.request", return_value=response):
                check_case(args, manifest, indexed, case)
        wrong_range = [(key, "bytes 1-64/281860" if key.lower() == "content-range" else value)
                       for key, value in headers("application/vnd.symbian.install")]
        for broken in (
            Response(200, headers("application/vnd.symbian.install"), body, body),
            Response(206, headers("text/html"), body, body),
            Response(206, wrong_range, body, body),
            Response(206, headers("application/vnd.symbian.install"), body[:-1], body[:-1]),
            Response(206, headers("application/vnd.symbian.install"), body, bytes([body[0] ^ 1]) + body[1:]),
        ):
            with self.subTest(response=broken), patch("run.request", return_value=broken), self.assertRaises(AssertionError):
                check_case(args, manifest, indexed, case)
        # The MIME exception is specific to the Python SIS case.
        args.profile = "php"
        with patch("run.request", return_value=response), self.assertRaises(AssertionError):
            check_case(args, manifest, indexed, case)
        css = {"status": 200, "headers": [("Content-Type", "text/css")]}
        with self.assertRaises(AssertionError):
            compare(Response(200, [("Content-Type", "application/octet-stream")], b"", b"body {}"), css, b"body {}", "bytes")

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

    def test_passage_redirect_checks_target_without_requiring_body_or_content_type(self):
        manifest = load_reference()
        indexed = {entry["request"]["id"]: entry for entry in manifest["cases"]}
        case = next(case for case in cases("python") if case["id"] == "passage-normalize-sort")
        args = SimpleNamespace(profile="python", user_data=None, php_reference=False,
                               base_url="http://127.0.0.1:8000")
        entry = indexed["passage-range"]
        target = Response(200, entry["response"]["headers"], b"", (REFERENCE / entry["body_file"]).read_bytes())
        for status in (301, 307):
            for location in ("/t/kjv/e/ge/1/1-3", "http://meebible.org/t/kjv/e/ge/1/1-3"):
                redirect = Response(status, [("Location", location)], b"", b"")
                with self.subTest(status=status, location=location), patch("run.request", side_effect=[redirect, target]) as send:
                    check_case(args, manifest, indexed, case)
                    self.assertEqual(send.call_count, 2)
                    self.assertEqual(send.call_args.args, (args.base_url, {"path": "/t/kjv/e/ge/1/1-3", "headers": {}}))
        # PHP still checks its original redirect status, headers, and body.
        args.profile = "php"
        with patch("run.request", return_value=redirect), self.assertRaises(AssertionError):
            check_case(args, manifest, indexed, case)

    def test_passage_redirect_rejects_wrong_targets_loops_and_wrong_final_content(self):
        manifest = load_reference()
        indexed = {entry["request"]["id"]: entry for entry in manifest["cases"]}
        case = next(case for case in cases("python") if case["id"] == "passage-normalize-sort")
        args = SimpleNamespace(profile="python", user_data=None, php_reference=False,
                               base_url="http://127.0.0.1:8000")
        for location in ("", "/t/kjv/e/ge/1/2", case["path"],
                         "http://example.org/t/kjv/e/ge/1/1-3", "https://meebible.org/t/kjv/e/ge/1/1-3"):
            redirect = Response(307, [("Location", location)], b"", b"")
            with self.subTest(location=location), patch("run.request", return_value=redirect) as send, self.assertRaises(AssertionError):
                check_case(args, manifest, indexed, case)
            self.assertEqual(send.call_count, 1)
        redirect = Response(307, [("Location", "/t/kjv/e/ge/1/1-3")], b"", b"")
        for target in (Response(307, redirect.headers, b"", b""),
                       Response(200, [("Content-Type", "text/html;charset=UTF-8")], b"", b"<html></html>")):
            with self.subTest(target=target), patch("run.request", side_effect=[redirect, target]), self.assertRaises(AssertionError):
                check_case(args, manifest, indexed, case)

    def test_passage_slash_allows_the_page_or_a_redirect_to_it(self):
        manifest = load_reference()
        indexed = {entry["request"]["id"]: entry for entry in manifest["cases"]}
        case = next(case for case in cases("python") if case["id"] == "passage-slash")
        args = SimpleNamespace(profile="python", user_data=None, php_reference=False,
                               base_url="http://127.0.0.1:8000")
        entry = indexed["passage-single"]
        target = Response(200, entry["response"]["headers"], b"", (REFERENCE / entry["body_file"]).read_bytes())
        with patch("run.request", return_value=target):
            check_case(args, manifest, indexed, case)
        for status in (301, 307):
            redirect = Response(status, [("Location", "http://meebible.org/t/kjv/e/ge/1/1")], b"", b"")
            with self.subTest(status=status), patch("run.request", side_effect=[redirect, target]):
                check_case(args, manifest, indexed, case)

    def test_missing_passage_verses_keep_200_and_check_the_remaining_content(self):
        manifest = load_reference()
        indexed = {entry["request"]["id"]: entry for entry in manifest["cases"]}
        args = SimpleNamespace(profile="python", user_data=None, php_reference=False,
                               base_url="http://127.0.0.1:8000")
        for case in cases("python"):
            if case["id"] not in {"passage-missing-verse", "passage-partial-verse"}:
                continue
            entry = indexed[case["id"]]
            body = (REFERENCE / entry["body_file"]).read_bytes()
            response = Response(200, entry["response"]["headers"], body, body)
            with self.subTest(case=case["id"]), patch("run.request", return_value=response):
                check_case(args, manifest, indexed, case)
            response.body = b"<html></html>"
            with patch("run.request", return_value=response), self.assertRaises(AssertionError):
                check_case(args, manifest, indexed, case)

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
