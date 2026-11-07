"""Run inside a disposable backend container, using only HTTP and SQLite.

Requires /data/.container-check-only and synthetic databases. See backend-tests.md.
"""

import concurrent.futures
import errno
import json
import os
import shutil
import sqlite3
import time
import urllib.error
import urllib.request
from contextlib import closing
from pathlib import Path

DATA = Path("/data")
BASE = "http://127.0.0.1:8000"
FORM = {"Content-Type": "application/x-www-form-urlencoded"}
RESULTS = []


def request(path, body=None, timeout=10):
    req = urllib.request.Request(BASE + path, data=body, headers=FORM)
    try:
        response = urllib.request.urlopen(req, timeout=timeout)
    except urllib.error.HTTPError as error:
        response = error
    with response:
        return response.status, response.read()


def rows(path, table):
    with closing(sqlite3.connect(f"file:{path}?mode=ro", uri=True)) as con:
        return con.execute(f"SELECT * FROM {table} ORDER BY id").fetchall()


def check(name, action):
    try:
        action()
    except Exception as error:  # noqa: BLE001 - collect independent check failures
        RESULTS.append({"check": name, "error": repr(error)})
    else:
        RESULTS.append({"check": name, "result": "pass"})


def failed_write(table, body):
    status, response = request("/" + table, body)
    assert 500 <= status < 600, (status, response[:200])
    assert response != b"ok" and b"Traceback" not in response


def scenarios(table, body):
    path = DATA / (table + ".sqlite")
    assert request("/" + table, body) == (200, b"ok")
    before = rows(path, table)

    def readonly():
        path.chmod(0o400)
        try:
            failed_write(table, body)
        finally:
            path.chmod(0o600)
        assert rows(path, table) == before

    check(table + ": read-only file", readonly)

    def no_journal():
        DATA.chmod(0o500)
        try:
            failed_write(table, body)
        finally:
            DATA.chmod(0o700)
        assert rows(path, table) == before

    check(table + ": unwritable journal directory", no_journal)

    def rejected_insert():
        with closing(sqlite3.connect(path)) as con:
            con.execute(
                f"CREATE TRIGGER reject_insert BEFORE INSERT ON {table} BEGIN SELECT RAISE(ABORT, 'injected'); END"
            )
        try:
            failed_write(table, body)
            assert rows(path, table) == before
        finally:
            with closing(sqlite3.connect(path)) as con:
                con.execute("DROP TRIGGER reject_insert")

    check(table + ": rejected insert", rejected_insert)

    def invalid_file():
        backup = path.with_suffix(".saved")
        path.rename(backup)
        try:
            path.write_bytes(b"not a SQLite database")
            failed_write(table, body)
        finally:
            path.unlink()
            backup.rename(path)
        assert rows(path, table) == before

    check(table + ": corrupt database", invalid_file)

    def full_disk():
        space = Path("/failure-space")
        assert space.is_dir(), "Dedicated 2 MiB test tmpfs required"
        backup = path.with_suffix(".saved")
        target = space / path.name
        filler = space / "filler"
        path.rename(backup)
        try:
            shutil.copyfile(backup, target)
            path.symlink_to(target)
            with filler.open("wb", buffering=0) as out:
                try:
                    for _ in range(129):
                        out.write(b"x" * 16384)
                except OSError as error:
                    assert error.errno == errno.ENOSPC
                else:
                    raise AssertionError("Test tmpfs did not fill within 2 MiB")
            failed_write(table, body)
            assert rows(path, table) == before
        finally:
            filler.unlink(missing_ok=True)
            path.unlink(missing_ok=True)
            backup.rename(path)
            target.unlink(missing_ok=True)

    check(table + ": full temporary filesystem", full_disk)

    def locked():
        con = sqlite3.connect(path)
        try:
            con.execute("BEGIN EXCLUSIVE")
            with concurrent.futures.ThreadPoolExecutor(max_workers=1) as pool:
                future = pool.submit(failed_write, table, body)
                time.sleep(0.2)
                started = time.monotonic()
                try:
                    assert request("/meta", timeout=2)[0] == 200
                    assert time.monotonic() - started < 2
                finally:
                    future.result(timeout=8)
        finally:
            con.rollback()
            con.close()
        assert rows(path, table) == before

    check(table + ": locked write keeps reads responsive", locked)

    def recovery():
        assert request("/" + table, body) == (200, b"ok")
        assert len(rows(path, table)) == len(before) + 1
        assert rows(path, table)[:-1] == before

    check(table + ": recovery commits exactly one row", recovery)


def main():
    assert os.getuid() == 10001
    assert (DATA / ".container-check-only").is_file(), "Disposable data marker required"
    # Never operate on preexisting user databases, even with the marker present.
    assert not (DATA / "startup.sqlite").exists()
    assert not (DATA / "feedback.sqlite").exists()
    scenarios("startup", b"os=container-test&device_id=synthetic")
    scenarios("feedback", b"name=container-test&email=&message=synthetic")
    print(json.dumps(RESULTS, indent=2))
    return any("error" in result for result in RESULTS)


if __name__ == "__main__":
    raise SystemExit(main())
