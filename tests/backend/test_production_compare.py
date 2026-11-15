"""Offline checks for the fixed production sample; never connect to a server."""

import contextlib
import gzip
import io
import json
import tempfile
import unittest
from pathlib import Path
from unittest.mock import patch

import compare_production as comparison


class ProductionCaptureTests(unittest.TestCase):
    def test_fixed_get_sample_records_redirect_without_following(self):
        manifest = json.loads((comparison.REFERENCE / "manifest.json").read_text())
        refs = {item["request"]["id"]: item for item in manifest["cases"]}
        for name, trans in [("translation-kjv", "kjv"), ("translation-nwt", "nwt")]:
            refs[name] = next(
                item
                for item in manifest["translations"]
                if item["trans"] == trans and item["lang"] == "e"
            )
        calls = []

        class Connection:
            def __init__(self, host, port, timeout):
                self.assert_origin = (host, port, timeout)

            def request(self, method, path, *, headers):
                calls.append((self.assert_origin, method, path))
                name, expected = comparison.SAMPLES[len(calls) - 1]
                assert path == expected
                body = (comparison.REFERENCE / refs[name]["body_file"]).read_bytes()
                self.status = (
                    301 if len(calls) == 1 else refs[name]["response"]["status"]
                )
                if headers["Accept-Encoding"] == "gzip":
                    body = gzip.compress(body)
                    self.headers = [("Content-Encoding", "gzip")]
                else:
                    self.headers = []
                self.headers += [
                    ("Content-Length", str(len(body))),
                    ("Location", "https://example.invalid/do-not-follow"),
                ]
                self.body = body

            def getresponse(self):
                return self

            def read(self, maximum):
                return self.body[:maximum]

            def getheaders(self):
                return self.headers

            def close(self):
                pass

        with (
            tempfile.TemporaryDirectory() as directory,
            patch.object(comparison.http.client, "HTTPConnection", Connection),
            patch.object(comparison.time, "sleep") as sleep,
            contextlib.redirect_stdout(io.StringIO()),
        ):
            output = Path(directory) / "capture"
            comparison.capture(output)
            report = json.loads((output / "report.json").read_text())
        self.assertEqual(len(calls), 17)
        self.assertTrue(
            all(
                origin == ("meebible.org", 80, 15) and method == "GET"
                for origin, method, _ in calls
            )
        )
        self.assertEqual(sleep.call_count, 16)
        self.assertTrue(report["complete"])
        self.assertEqual(report["cases"][0]["status"], 301)
        self.assertFalse(report["follows_redirects"])

    def test_server_error_stops_without_retry(self):
        with (
            tempfile.TemporaryDirectory() as directory,
            patch.object(comparison.http.client, "HTTPConnection") as network,
            patch.object(comparison.time, "sleep") as sleep,
            contextlib.redirect_stdout(io.StringIO()),
        ):
            connection = network.return_value
            response = connection.getresponse.return_value
            response.status = 503
            response.read.return_value = b"unavailable"
            response.getheaders.return_value = [("Content-Length", "11")]
            output = Path(directory) / "capture"
            with self.assertRaises(RuntimeError):
                comparison.capture(output)
            self.assertEqual(connection.request.call_count, 1)
            sleep.assert_not_called()
            report = json.loads((output / "report.json").read_text())
            self.assertNotIn("complete", report)
            self.assertEqual(report["cases"][0]["status"], 503)

    def test_existing_output_is_rejected_before_network(self):
        with (
            tempfile.TemporaryDirectory() as directory,
            patch.object(comparison.http.client, "HTTPConnection") as network,
        ):
            with self.assertRaises(FileExistsError):
                comparison.capture(Path(directory))
            network.assert_not_called()


if __name__ == "__main__":
    unittest.main()
