"""Small, fixed, read-only production sample. Never used by the HTTP test suite."""

import argparse
import gzip
import hashlib
import http.client
import io
import json
import time
import xml.etree.ElementTree as ET
from datetime import UTC, datetime
from pathlib import Path

REFERENCE = Path(__file__).resolve().parent / "reference"
SAMPLES = (
    ("home", "/"),
    ("home-gzip", "/"),
    ("privacy", "/privacy"),
    ("meta", "/meta"),
    ("meta-gzip", "/meta"),
    ("translation-kjv", "/translation?trans=kjv&lang=e"),
    ("translation-nwt", "/translation?trans=nwt&lang=e"),
    ("chapter-kjv-e-ge-1", "/chapter?trans=kjv&lang=e&book=ge&chapter=1"),
    ("chapter-rus-u-ge-1", "/chapter?trans=rus&lang=u&book=ge&chapter=1"),
    ("chapter-ara-a-ge-1", "/chapter?trans=ara&lang=a&book=ge&chapter=1"),
    ("chapter-gzip", "/chapter?trans=ara&lang=a&book=ge&chapter=1"),
    ("chapter-nwt-e-ge-1", "/chapter?trans=nwt&lang=e&book=ge&chapter=1"),
    ("passage-single", "/t/kjv/e/ge/1/1"),
    ("passage-normalize-sort", "/t/kjv/e/ge/1/3,1,2"),
    ("static-css", "/static/css/style.css"),
    ("favicon", "/favicon.ico"),
    ("static-range", "/static/meebible_3.5.7_signed.sis"),
)
MAX_BYTES = 16 * 1024 * 1024


def sha(body):
    return hashlib.sha256(body).hexdigest()


def headers(values):
    return {key.lower(): value for key, value in values}


def xml_equivalent(a, b):
    try:
        return ET.canonicalize(a.decode(), strip_text=True) == ET.canonicalize(
            b.decode(), strip_text=True
        )
    except (ET.ParseError, UnicodeError):
        return False


def capture(output):
    reference_bytes = (REFERENCE / "manifest.json").read_bytes()
    manifest = json.loads(reference_bytes)
    cases = {item["request"]["id"]: item for item in manifest["cases"]}
    for name, trans in [("translation-kjv", "kjv"), ("translation-nwt", "nwt")]:
        cases[name] = next(
            item
            for item in manifest["translations"]
            if item["trans"] == trans and item["lang"] == "e"
        )
    # Check the entire fixed sample before sending any request.
    for name, path in SAMPLES:
        ref = cases[name]
        request = ref.get("request", {"path": ref.get("path")})
        assert request["path"] == path
        assert request.get("method", "GET") == "GET" and not request.get("body")
        assert (
            sha((REFERENCE / ref["body_file"]).read_bytes())
            == ref["response"]["body_sha256"]
        )
    output.mkdir(parents=True, exist_ok=False)
    report = {
        "started_at_utc": datetime.now(UTC).isoformat(),
        "origin": "http://meebible.org",
        "method": "GET",
        "interval_seconds": 1,
        "concurrency": 1,
        "follows_redirects": False,
        "reference_manifest_sha256": sha(reference_bytes),
        "cases": [],
    }
    try:
        for index, (name, path) in enumerate(SAMPLES):
            if index:
                time.sleep(1)
            ref = cases[name]
            sent = {
                "Accept-Encoding": "gzip" if name.endswith("-gzip") else "identity",
                "User-Agent": "MeeBible-migration-readonly-comparison/1.0",
            }
            if name == "static-range":
                sent["Range"] = "bytes=0-63"
            connection = http.client.HTTPConnection("meebible.org", 80, timeout=15)
            try:
                connection.request("GET", path, headers=sent)
                response = connection.getresponse()
                wire = response.read(MAX_BYTES + 1)
                assert len(wire) <= MAX_BYTES, "Response too large"
                original_headers = response.getheaders()
                received = headers(original_headers)
                body = wire
                if received.get("content-encoding") == "gzip":
                    with gzip.GzipFile(fileobj=io.BytesIO(wire)) as stream:
                        body = stream.read(MAX_BYTES + 1)
                    assert len(body) <= MAX_BYTES, "Decoded response too large"
                if received.get("content-length"):
                    assert int(received["content-length"]) == len(wire)
                (output / (name + ".body")).write_bytes(body)
                (output / (name + ".wire")).write_bytes(wire)
                expected = (REFERENCE / ref["body_file"]).read_bytes()
                ref_headers = headers(ref["response"]["headers"])
                record = {
                    "id": name,
                    "path": path,
                    "request_headers": sent,
                    "received_at_utc": datetime.now(UTC).isoformat(),
                    "status": response.status,
                    "headers": original_headers,
                    "wire_bytes": len(wire),
                    "body_bytes": len(body),
                    "body_sha256": sha(body),
                    "reference_status": ref["response"]["status"],
                    "body_equal": body == expected,
                    "header_differences": {
                        key: {
                            "reference": ref_headers.get(key),
                            "production": received.get(key),
                        }
                        for key in sorted(ref_headers.keys() | received.keys())
                        if ref_headers.get(key) != received.get(key)
                    },
                }
                if body != expected and "xml" in received.get("content-type", ""):
                    record["xml_equivalent"] = xml_equivalent(expected, body)
                report["cases"].append(record)
                print(
                    name,
                    response.status,
                    "same" if body == expected else "DIFFERENT",
                    flush=True,
                )
                if response.status >= 500:
                    raise RuntimeError(
                        "Stopping on production server error; no retries"
                    )
            finally:
                connection.close()
        report["complete"] = True
    finally:
        report["finished_at_utc"] = datetime.now(UTC).isoformat()
        (output / "report.json").write_text(json.dumps(report, indent=2) + "\n")
        assert (REFERENCE / "manifest.json").read_bytes() == reference_bytes


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--output", required=True, type=Path)
    capture(parser.parse_args().output)
