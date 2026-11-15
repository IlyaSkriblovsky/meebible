"""Create synthetic records using the captured PHP schemas; no production reads."""

import argparse
import json
import sqlite3
from contextlib import closing
from pathlib import Path

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("--output", required=True, type=Path)
args = parser.parse_args()
root = Path(__file__).resolve().parents[2]
schemas = json.loads((root / "tests/backend/reference/manifest.json").read_text())[
    "schemas"
]
args.output.mkdir(mode=0o700, parents=False, exist_ok=False)
for table in ("startup", "feedback"):
    columns = schemas[table]
    definition = ", ".join(
        col["name"] + " " + col["type"] + (" PRIMARY KEY" if col["pk"] else "")
        for col in columns
    )
    with closing(sqlite3.connect(args.output / (table + ".sqlite"))) as db:
        db.execute(f"CREATE TABLE {table} ({definition})")
        for _ in range(3):
            values = [
                None,
                "2026-11-07 00:00:00",
                *(["synthetic + я שלום"] * (len(columns) - 2)),
            ]
            if table == "startup":
                values[-2:] = [None, None]
            db.execute(
                f"INSERT INTO {table} VALUES ({','.join('?' for _ in values)})", values
            )
        db.commit()
    (args.output / (table + ".sqlite")).chmod(0o600)
print("Created 3 synthetic rows per table with captured PHP schemas")
