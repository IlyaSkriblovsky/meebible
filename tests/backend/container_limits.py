"""HTTP limits on disposable container data, directly and through the test proxy."""

import json
import sqlite3
import urllib.error
import urllib.request
from contextlib import closing
from pathlib import Path

DATA = Path("/data")
assert (DATA / ".container-check-only").is_file(), "Disposable data marker required"
FORM = {"Content-Type": "application/x-www-form-urlencoded"}
results = []


def snapshot(table):
    path = DATA / (table + ".sqlite")
    if not path.exists():
        return None
    with closing(sqlite3.connect(f"file:{path}?mode=ro", uri=True)) as con:
        if not con.execute(
            "SELECT 1 FROM sqlite_master WHERE type='table' AND name=?", (table,)
        ).fetchone():
            return None
        return con.execute(f"SELECT * FROM {table} ORDER BY id").fetchall()


def request(origin, path, data=None):
    req = urllib.request.Request(origin + path, data=data, headers=FORM)
    try:
        response = urllib.request.urlopen(req, timeout=10)
    except urllib.error.HTTPError as error:
        response = error
    with response:
        return response.status, response.read()


for origin in ("http://127.0.0.1:8000", "http://proxy:8080"):
    for table, prefix in (
        ("startup", b"os=x&device_id="),
        ("feedback", b"name=&email=&message="),
    ):
        before = snapshot(table)
        for chunked in (False, True):
            body = prefix + b"x" * (1024 * 1024)
            data = iter([body[:524288], body[524288:]]) if chunked else body
            status, _ = request(origin, "/" + table, data)
            assert status == 413, (origin, table, chunked, status)
            assert snapshot(table) == before
            results.append(
                {"origin": origin, "route": table, "chunked": chunked, "status": status}
            )
    prefix = b"name=&email=&message="
    body = prefix + b"x" * (1024 * 1024 - len(prefix))
    assert request(origin, "/feedback", body) == (200, b"ok")
    results.append({"origin": origin, "check": "exactly 1 MiB accepted", "status": 200})
    path = "/t/kjv/e/ge/1/" + ",".join(["1-300"] * 1000)
    status, _ = request(origin, path)
    assert status == 400, (origin, status)
    results.append(
        {"origin": origin, "check": "oversized verse list", "status": status}
    )
print(json.dumps(results, indent=2))
