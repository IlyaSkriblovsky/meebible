#!/usr/bin/env python3
"""External MeeBible contract checks. Default profile is the future Python contract."""

import argparse
import concurrent.futures
import gzip
import json
import os
import sys
import time
import xml.etree.ElementTree as ET
from datetime import datetime, timezone
from pathlib import Path

from cases import cases
from support import (
    HERE,
    REFERENCE,
    Page,
    delta,
    load_reference,
    local_url,
    need,
    request,
    sha,
    user_data,
    xml_value,
)


def media(response):
    parts = [
        part.strip().lower() for part in response.header("Content-Type").split(";")
    ]
    return parts[0], {
        part.split("=", 1)[0].strip(): part.split("=", 1)[1].strip(' "')
        for part in parts[1:]
        if "=" in part
    }


def clean_error(response):
    need(
        media(response)[0] in {"text/plain", "text/html"},
        "Error must use text or HTML, not automatic JSON",
    )
    need(
        not any(
            marker in response.body
            for marker in (
                b"Traceback",
                b"Stack trace",
                b"Fatal error",
                b"<b>Warning</b>",
                b"<b>Notice</b>",
            )
        ),
        "Error leaked runtime diagnostics",
    )


def compare(response, expected, body, kind):
    need(
        response.status == expected["status"],
        f"HTTP {response.status}, expected {expected['status']}",
    )
    old_headers = {key.lower(): value for key, value in expected["headers"]}
    old_media = old_headers.get("content-type", "").split(";", 1)[0].lower()
    need(
        media(response)[0] == old_media,
        f"Content-Type: {response.header('Content-Type')}, expected {old_media}",
    )
    if (
        old_media.startswith("text/")
        and "utf-8" in old_headers.get("content-type", "").lower()
    ):
        need(
            media(response)[1].get("charset") == "utf-8",
            "Text response needs UTF-8 charset",
        )
    for header in ("location", "content-range"):
        need(
            response.header(header) == old_headers.get(header, ""),
            f"Unexpected {header}",
        )
    if old_headers.get("content-encoding") == "gzip":
        need(
            response.header("Content-Encoding").lower() == "gzip",
            "gzip was not negotiated",
        )
        need(
            "accept-encoding" in response.header("Vary").lower(),
            "gzip needs Vary: Accept-Encoding",
        )
    if kind == "xml":
        need(
            xml_value(response.body) == xml_value(body),
            "XML values, attributes, or element order differ",
        )
    elif kind == "home":
        need(
            Page(response.body).home_value()
            == Page(body).home_value(sort_publications=True),
            "Home content differs or publications are not sorted by name within each language",
        )
    elif kind in {"page", "passage"}:
        need(
            Page(response.body).value(kind == "passage")
            == Page(body).value(kind == "passage"),
            "Page content, links, forms, or passage features differ",
        )
    else:
        need(
            response.body == body,
            f"Body bytes differ ({len(response.body)} vs {len(body)} bytes)",
        )


def check_case(args, manifest, indexed, case):
    entry = indexed.get(case["id"])
    policy = case.get("python", {}) if args.profile == "python" else {}
    before = (
        user_data(args.user_data, args.php_reference) if case.get("writes") else None
    )
    started = time.time()
    response = request(args.base_url, case)
    if policy:
        statuses = policy.get("statuses", [policy.get("status", 200)])
        need(
            response.status in statuses, f"HTTP {response.status}, expected {statuses}"
        )
        kind = policy["kind"]
        if kind == "home":
            need(entry is not None, "Missing captured case")
            compare(response, entry["response"], (REFERENCE / entry["body_file"]).read_bytes(), "home")
        if kind in {"controlled", "method"}:
            clean_error(response)
        if kind == "method":
            need(
                set(response.header("Allow").replace(" ", "").split(","))
                == set(policy["allow"]),
                "Wrong Allow header",
            )
        if kind == "write":
            need(
                response.body == b"ok" and media(response)[0] == "text/plain",
                "Successful POST must return text ok",
            )
        if kind == "selection":
            chapter = Page(
                (REFERENCE / indexed[policy["chapter"]]["body_file"]).read_bytes()
            )
            expected = [
                (n["attrs"]["verse"], " ".join("".join(n["text"]).split()))
                for n in chapter.verses
                if int(n["attrs"]["verse"]) in policy["verses"]
            ]
            need(
                Page(response.body).value(True)["verses"] == expected,
                "Digit-prefixed passage selected the wrong verses",
            )
    else:
        need(entry is not None, "Missing captured case")
        compare(
            response,
            entry["response"],
            (REFERENCE / entry["body_file"]).read_bytes(),
            case["kind"],
        )
    if case.get("method") == "HEAD" and not case.get("writes"):
        get_response = request(args.base_url, {**case, "method": "GET"})
        need(
            response.header("Content-Length") == str(len(get_response.wire)),
            "HEAD length differs from GET",
        )
    if before is not None:
        after = user_data(args.user_data, args.php_reference)
        changes = delta(before, after)
        if policy.get("no_write"):
            need(
                changes == {"startup": [], "feedback": []},
                "Rejected request wrote user data",
            )
        elif policy.get("kind") == "write":
            wanted = {"startup": [], "feedback": []}
            wanted[policy["table"]] = [policy["values"]]
            need(changes == wanted, f"Committed form values differ: {changes!r}")
        else:
            need(changes == entry["writes"], f"PHP stored values changed: {changes!r}")
        for table in ("startup", "feedback"):
            if after[table]["columns"]:
                need(
                    after[table]["columns"] == manifest["schemas"][table],
                    f"{table}: original schema changed",
                )
            for row in after[table]["rows"][len(before[table]["rows"]) :]:
                need(isinstance(row["id"], int), "SQLite id must be an integer")
                need(
                    not before[table]["rows"]
                    or row["id"] > before[table]["rows"][-1]["id"],
                    "New row id did not increase",
                )
                stamp = (
                    datetime.strptime(row["time"], "%Y-%m-%d %H:%M:%S")
                    .replace(tzinfo=timezone.utc)
                    .timestamp()
                )
                need(
                    started - 30 <= stamp <= time.time() + 30,
                    "Stored time must be current UTC with second precision",
                )
    return response


def run(args):
    url = local_url(args.base_url)
    need(
        args.profile != "python" or url.port != 10056,
        "Port 10056 is reserved for old PHP. Run the Python server on a distinct port.",
    )
    if args.php_reference:
        need(
            url.hostname in {"127.0.0.1", "localhost"} and url.port == 10056,
            "--php-reference may observe only the fixed local PHP server",
        )
        need(
            args.profile == "php",
            "Use --user-data for the Python profile; PHP tmpfs belongs to the PHP server",
        )
    if not args.read_only:
        need(
            args.user_data or args.php_reference,
            "Set USER_DATA_DIR / --user-data for temporary SQLite files, or explicitly use --read-only",
        )
        user_data(
            args.user_data, args.php_reference
        )  # Validate the observer before sending requests.
    manifest = load_reference()
    indexed = {entry["request"]["id"]: entry for entry in manifest["cases"]}
    report = {
        "started_at_utc": datetime.now(timezone.utc).isoformat(),
        "base_url": args.base_url,
        "profile": args.profile,
        "mode": args.mode,
        "read_only": args.read_only,
        "reference_manifest_sha256": sha((REFERENCE / "manifest.json").read_bytes()),
        "chapter_index_sha256": sha((REFERENCE / "chapters.json.gz").read_bytes()),
        "passed": 0,
        "failed": [],
        "skipped": [],
        "http_responses": 0,
    }
    output = HERE / ".local"
    output.mkdir(exist_ok=True)
    evidence = gzip.open(output / (args.mode + "-responses.jsonl.gz"), "wt")

    def save_response(name, response):
        report["http_responses"] += 1
        evidence.write(json.dumps({"test": name, **response.record()}) + "\n")

    def check(name, callback):
        try:
            response = callback()
            if response is not None:
                save_response(name, response)
            report["passed"] += 1
            print(f"PASS {name}", flush=True)
        except Exception as error:
            report["failed"].append(
                {"test": name, "error": f"{type(error).__name__}: {error}"}
            )
            print(f"FAIL {name}: {error}", flush=True)

    started = time.monotonic()
    for case in cases(args.profile):
        if args.read_only and (
            case.get("writes") or case.get("method", "GET") not in {"GET", "HEAD"}
        ):
            report["skipped"].append(case["id"])
            continue
        check(case["id"], lambda case=case: check_case(args, manifest, indexed, case))

    def catalog_flow():
        response = request(args.base_url, {"path": "/meta"})
        catalog = ET.fromstring(response.body)
        languages = {item.attrib["code"] for item in catalog.find("languages")}
        pairs = [
            (trans.attrib["code"], lang.attrib["code"])
            for trans in catalog.find("translations")
            for lang in trans
        ]
        need(
            all(lang in languages for _, lang in pairs),
            "Translation refers to an unknown language",
        )
        need(
            pairs == [(t["trans"], t["lang"]) for t in manifest["translations"]],
            "Catalog pair order changed",
        )
        return response

    check("catalog-flow", catalog_flow)
    selected_translations = (
        manifest["translations"]
        if args.mode == "full"
        else [
            entry
            for entry in manifest["translations"]
            if (entry["trans"], entry["lang"])
            in {("kjv", "e"), ("rus", "u"), ("ara", "a"), ("heb", "heb"), ("nwt", "e")}
        ]
    )
    for translation in selected_translations:

        def check_translation(translation=translation):
            response = request(args.base_url, {"path": translation["path"]})
            compare(
                response,
                translation["response"],
                (REFERENCE / translation["body_file"]).read_bytes(),
                "xml",
            )
            for book in ET.fromstring(response.body):
                need(
                    [int(ch.attrib["no"]) for ch in book]
                    == list(range(1, len(book) + 1)),
                    "Chapter numbers have gaps or wrong order",
                )
            return response

        check(translation["path"], check_translation)

    # Quick mode also exercises HTTP validators and byte ranges for a real phone package.
    for path in (
        "/static/css/style.css",
        "/favicon.ico",
        "/static/meebible_3.5.7_signed.sis",
    ):

        def asset_check(path=path):
            response = request(args.base_url, {"path": path})
            asset = next(item for item in manifest["assets"] if item["path"] == path)
            need(
                response.status == 200 and sha(response.body) == asset["sha256"],
                "Static file bytes changed",
            )
            validator = response.header("ETag")
            if validator:
                cached = request(
                    args.base_url,
                    {"path": path, "headers": {"If-None-Match": validator}},
                )
                need(
                    cached.status == 304 and not cached.body,
                    "Static ETag does not support conditional GET",
                )
            return response

        check("asset:" + path, asset_check)

    if args.mode == "full":
        for asset in manifest["assets"]:

            def static_check(asset=asset):
                response = request(args.base_url, {"path": asset["path"]})
                need(
                    response.status == 200
                    and len(response.body) == asset["bytes"]
                    and sha(response.body) == asset["sha256"],
                    "Static file differs from pinned content",
                )
                return response

            check("asset-full:" + asset["path"], static_check)
        chapters = json.loads(
            gzip.decompress((REFERENCE / "chapters.json.gz").read_bytes())
        )
        report["chapters_expected"] = len(chapters)
        report["chapters_checked"] = 0

        def fetch_chapter(chapter):
            try:
                return chapter, request(args.base_url, chapter), None
            except Exception as error:
                return chapter, None, error

        # Bound both client concurrency and queued work. A whole download can keep 20
        # requests outstanding. Each path comes from the captured catalog/translation flow.
        with concurrent.futures.ThreadPoolExecutor(max_workers=args.workers) as pool:
            for offset in range(0, len(chapters), 1000):
                for chapter, response, error in pool.map(
                    fetch_chapter, chapters[offset : offset + 1000]
                ):

                    def verify(chapter=chapter, response=response, error=error):
                        if error:
                            raise error
                        need(response.status == 200, f"Chapter HTTP {response.status}")
                        need(
                            media(response) == ("text/html", {"charset": "utf-8"}),
                            "Wrong chapter Content-Type/charset",
                        )
                        need(
                            len(response.body) == chapter["bytes"]
                            and sha(response.body) == chapter["sha256"],
                            "Chapter bytes changed",
                        )
                        return response

                    check(chapter["path"], verify)
                    report["chapters_checked"] += 1
                print(
                    f"Chapters: {report['chapters_checked']}/{len(chapters)}; {time.monotonic() - started:.1f}s",
                    flush=True,
                )
                if len(report["failed"]) >= 20:
                    print(
                        "Stopping after 20 failures; the full run is incomplete.",
                        flush=True,
                    )
                    break
        need(
            report["chapters_checked"] == len(chapters) or report["failed"],
            "Incomplete full run",
        )
    evidence.close()
    report["seconds"] = round(time.monotonic() - started, 2)
    report["result"] = (
        "fail" if report["failed"] else ("partial" if report["skipped"] else "pass")
    )
    report_path = output / (args.mode + "-" + args.profile + ".json")
    report_path.write_text(json.dumps(report, indent=2) + "\n")
    print(
        f"{report['result'].upper()}: {report['passed']} passed, {len(report['failed'])} failed, "
        f"{len(report['skipped'])} skipped; {report['seconds']}s"
    )
    print(f"Report: {report_path}")
    return bool(report["failed"])


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("mode", choices=("quick", "full"))
    parser.add_argument(
        "--base-url", default=os.environ.get("BASE_URL", "http://127.0.0.1:8000")
    )
    parser.add_argument("--profile", choices=("php", "python"), default="python")
    parser.add_argument(
        "--user-data", type=Path, default=os.environ.get("USER_DATA_DIR")
    )
    parser.add_argument("--php-reference", action="store_true")
    parser.add_argument(
        "--read-only",
        action="store_true",
        help="Explicit partial run without POST/method write checks",
    )
    parser.add_argument("--workers", type=int, choices=range(1, 21), default=20)
    args = parser.parse_args()
    try:
        sys.exit(run(args))
    except (AssertionError, ValueError, OSError) as error:
        parser.exit(2, f"Error: {error}\n")
