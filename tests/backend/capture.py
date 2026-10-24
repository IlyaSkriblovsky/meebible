#!/usr/bin/env python3
"""Explicitly capture the isolated PHP server. Existing references are never replaced."""

import argparse
import gzip
import json
import sqlite3
import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path

from cases import cases
from support import COMPOSE, ROOT, REFERENCE, delta, need, request, sha, user_data


def resource_limits():
    result = {}
    for service in ("php", "nginx"):
        container = subprocess.check_output([*COMPOSE, "ps", "-q", service], text=True).strip()
        config = json.loads(subprocess.check_output(["docker", "inspect", container], text=True))[0]["HostConfig"]
        result[service] = {key: config[key] for key in ("NanoCpus", "Memory", "PidsLimit")}
    return result


def capture(output):
    need(not output.exists(), "Output already exists. Capture to a new directory and review the diff.")
    # Reuse the setup's mount/network/image checks before any malformed request.
    subprocess.run([sys.executable, str(ROOT / "tools/php-reference/reference.py"), "check"], check=True)
    local = ROOT / "tools/php-reference/.local"
    environment = json.loads((local / "verification.json").read_text())
    initial_limits = resource_limits()
    snapshot = local / "snapshot/www"
    output.mkdir(parents=True)
    (output / "bodies").mkdir()
    entries = []
    for case in cases():
        if case.get("python_only"):
            continue
        before = user_data(php_reference=True) if case.get("writes") else None
        response = request("http://127.0.0.1:10056", case)
        entry = {"request": case, "response": response.record()}
        name = "bodies/" + case["id"] + ".body"
        (output / name).write_bytes(response.body)
        entry["body_file"] = name
        if response.wire != response.body:
            wire_name = "bodies/" + case["id"] + ".wire"
            (output / wire_name).write_bytes(response.wire)
            entry["wire_file"] = wire_name
        if before is not None:
            after = user_data(php_reference=True)
            entry["writes"] = delta(before, after)
            entry["columns"] = {table: data["columns"] for table, data in after.items()}
        entries.append(entry)
    # All translation XML is captured over HTTP. The client relies on book/chapter order.
    meta = next(entry for entry in entries if entry["request"]["id"] == "meta")
    import xml.etree.ElementTree as ET
    catalog = ET.fromstring((output / meta["body_file"]).read_bytes())
    translations = []
    for translation in catalog.find("translations"):
        for language in translation:
            trans, lang = translation.attrib["code"], language.attrib["code"]
            path = f"/translation?trans={trans}&lang={lang}"
            response = request("http://127.0.0.1:10056", {"path": path})
            need(response.status == 200, path)
            filename = f"bodies/translation-{trans}-{lang}.xml"
            (output / filename).write_bytes(response.body)
            translations.append({"trans": trans, "lang": lang, "path": path,
                                 "body_file": filename, "response": response.record()})
    # The fixed SQLite snapshot already contains the full chapter bodies. Save a compact
    # independent index; full mode verifies every HTTP body against these exact bytes.
    chapters = []
    for translation in translations:
        trans, lang = translation["trans"], translation["lang"]
        filename = snapshot / "content" / (f"nwt/nwt_{lang}.sqlite" if trans == "nwt" else f"{trans}.sqlite")
        with sqlite3.connect(filename.as_uri() + "?mode=ro", uri=True) as db:
            expected = ET.fromstring((output / translation["body_file"]).read_bytes())
            rows = {(book, number): html.encode("utf-8") for book, number, html in db.execute(
                "SELECT bookCode, chapterNo, html FROM html WHERE langCode=?", (lang,)) if html}
            for book in expected:
                for chapter in book:
                    book_code, number = book.attrib["code"], int(chapter.attrib["no"])
                    body = rows.get((book_code, number))
                    need(body is not None, f"Missing source chapter: {trans}/{lang}/{book_code}/{number}")
                    chapters.append({"path": f"/chapter?trans={trans}&lang={lang}&book={book_code}&chapter={number}",
                                     "bytes": len(body), "sha256": sha(body)})
    with (output / "chapters.json.gz").open("wb") as stream:
        with gzip.GzipFile(fileobj=stream, mode="wb", mtime=0) as compressed:
            compressed.write(json.dumps(chapters, separators=(",", ":")).encode())
    # All static assets, including phone packages, are checked in full mode.
    assets = []
    for path in sorted((snapshot / "static").rglob("*")):
        if path.is_file():
            assets.append({"path": "/static/" + path.relative_to(snapshot / "static").as_posix(),
                           "bytes": path.stat().st_size, "sha256": sha(path.read_bytes())})
    icon = snapshot / "static/img/favicon.ico"
    assets.append({"path": "/favicon.ico", "bytes": icon.stat().st_size, "sha256": sha(icon.read_bytes())})
    # Preserve the actual write schemas, including type, nullable columns, and primary keys.
    schemas = {table: data["columns"] for table, data in user_data(php_reference=True).items()}
    manifest = {"format": 1, "captured_at_utc": datetime.now(timezone.utc).isoformat(),
                "base_url": "http://127.0.0.1:10056", "environment": environment,
                "resource_limits_at_start": initial_limits, "resource_limits_at_end": resource_limits(),
                "cases": entries, "translations": translations, "assets": assets,
                "schemas": schemas, "chapter_count": len(chapters)}
    manifest["files"] = {path.relative_to(output).as_posix(): sha(path.read_bytes())
                         for path in sorted(output.rglob("*")) if path.is_file()}
    (output / "manifest.json").write_text(json.dumps(manifest, indent=2, ensure_ascii=False) + "\n")
    print(f"Captured {len(entries)} cases, {len(translations)} translations, {len(chapters)} chapter hashes")
    print(f"Output: {output}")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--output", type=Path, default=REFERENCE)
    args = parser.parse_args()
    capture(args.output.resolve())
