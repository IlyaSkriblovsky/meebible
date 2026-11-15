"""Back up or restore the two user databases into a NEW private directory.

A backup is individually SQLite-consistent, including committed WAL pages.
Stop ALL writers first for a common startup/feedback cutover point.
"""

import argparse
import hashlib
import json
import os
import shutil
import sqlite3
import time
from contextlib import closing
from datetime import UTC, datetime
from pathlib import Path

TABLES = {
    "startup": ["id", "time", "device_id", "os", "lang", "trans"],
    "feedback": ["id", "time", "name", "email", "message"],
}


def digest(path):
    with path.open("rb") as stream:
        return hashlib.file_digest(stream, "sha256").hexdigest()


def inspect(path, table):
    if not path.is_file() or path.is_symlink():
        raise ValueError(f"Missing or non-regular database: {path}")
    with closing(sqlite3.connect(path.resolve().as_uri() + "?mode=ro", uri=True)) as db:
        if db.execute("PRAGMA integrity_check").fetchall() != [("ok",)]:
            raise ValueError(f"Integrity check failed: {path}")
        columns = [row[1] for row in db.execute(f"PRAGMA table_info({table})")]
        if columns != TABLES[table]:
            raise ValueError(f"Unexpected {table} columns: {columns}")
        types = [row[2].upper() for row in db.execute(f"PRAGMA table_info({table})")]
        expected_types = [
            "INTEGER" if name == "id" else "DATETIME" if name == "time" else "VARCHAR"
            for name in columns
        ]
        if types != expected_types:
            raise ValueError(f"Unexpected {table} column types: {types}")
        count = db.execute(f"SELECT COUNT(*) FROM {table}").fetchone()[0]
    return {"columns": columns, "rows": count}


def copy_database(source, target):
    started = time.monotonic()

    def progress(_status, _remaining, _total):
        if time.monotonic() - started > 30:
            raise TimeoutError("SQLite backup exceeded 30 seconds")

    with (
        closing(
            sqlite3.connect(source.resolve().as_uri() + "?mode=ro", uri=True)
        ) as src,
        closing(sqlite3.connect(target)) as dst,
    ):
        src.backup(dst, pages=256, progress=progress, sleep=0.05)
        # WAL mode is copied with the pages; seal the snapshot as one file.
        if dst.execute("PRAGMA journal_mode=DELETE").fetchone() != ("delete",):
            raise ValueError("Could not seal backup without WAL sidecars")
    target.chmod(0o600)


def backup(source, output, writers_stopped):
    if not writers_stopped:
        raise ValueError("Stop all writers and pass --writers-stopped")
    source = source.resolve()
    # Validate both sources before creating any destination files.
    for table in TABLES:
        inspect(source / (table + ".sqlite"), table)
    output.mkdir(mode=0o700, parents=False, exist_ok=False)
    manifest = {
        "format": 1,
        "created_at_utc": datetime.now(UTC).isoformat(),
        "writers_stopped": True,
        "databases": {},
    }
    for table in TABLES:
        name = table + ".sqlite"
        target = output / name
        copy_database(source / name, target)
        manifest["databases"][name] = {
            **inspect(target, table),
            "sha256": digest(target),
        }
    # A failed/incomplete directory has no manifest and cannot be restored.
    path = output / "manifest.json"
    path.write_text(json.dumps(manifest, indent=2) + "\n")
    path.chmod(0o600)
    return manifest


def validate_bundle(source):
    manifest = json.loads((source / "manifest.json").read_text())
    names = {table + ".sqlite" for table in TABLES}
    if manifest.get("format") != 1 or set(manifest.get("databases", {})) != names:
        raise ValueError("Invalid backup manifest")
    for table in TABLES:
        name = table + ".sqlite"
        expected = manifest["databases"][name]
        if any(
            (source / (name + suffix)).exists()
            for suffix in ("-wal", "-shm", "-journal")
        ):
            raise ValueError(f"Backup is not a self-contained SQLite snapshot: {name}")
        if inspect(source / name, table) != {
            key: expected[key] for key in ("columns", "rows")
        }:
            raise ValueError(f"Backup metadata mismatch: {name}")
        if digest(source / name) != expected["sha256"]:
            raise ValueError(f"Backup hash mismatch: {name}")
    return manifest


def restore(source, output):
    manifest = validate_bundle(source)
    output.mkdir(mode=0o700, parents=False, exist_ok=False)
    for table in TABLES:
        name = table + ".sqlite"
        target = output / name
        shutil.copyfile(source / name, target)
        target.chmod(0o600)
        if digest(target) != manifest["databases"][name]["sha256"]:
            raise ValueError(f"Restore hash mismatch: {name}")
        if inspect(target, table) != {
            key: manifest["databases"][name][key] for key in ("columns", "rows")
        }:
            raise ValueError(f"Restore metadata mismatch: {name}")
        # The image need not contain SQLite journals; each restored file is complete.
    return manifest


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("command", choices=("backup", "verify", "restore"))
    parser.add_argument("--source", required=True, type=Path)
    parser.add_argument("--output", type=Path)
    parser.add_argument("--writers-stopped", action="store_true")
    args = parser.parse_args()
    os.umask(0o077)
    if args.command == "verify":
        result = validate_bundle(args.source)
    elif args.output is None:
        parser.error("--output is required")
    elif args.command == "backup":
        result = backup(args.source, args.output, args.writers_stopped)
    else:
        result = restore(args.source, args.output)
    print(json.dumps({"result": "ok", "databases": result["databases"]}, indent=2))


if __name__ == "__main__":
    main()
