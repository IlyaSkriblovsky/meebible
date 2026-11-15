"""Temporary-data tests for operations that must not lose user records."""

import json
import sqlite3
import tempfile
import unittest
from contextlib import ExitStack, closing
from pathlib import Path

import user_data


class UserDataTests(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.addCleanup(self.tmp.cleanup)
        self.root = Path(self.tmp.name)
        self.source = self.root / "source"
        self.source.mkdir()
        self.connections = ExitStack()
        self.addCleanup(self.connections.close)
        for table, columns in user_data.TABLES.items():
            con = self.connections.enter_context(
                closing(sqlite3.connect(self.source / (table + ".sqlite")))
            )
            con.execute("PRAGMA journal_mode=WAL")
            con.execute("PRAGMA wal_autocheckpoint=0")
            definition = ", ".join(
                "id INTEGER PRIMARY KEY"
                if c == "id"
                else c + (" DATETIME" if c == "time" else " VARCHAR")
                for c in columns
            )
            con.execute(f"CREATE TABLE {table} ({definition})")
            values = [
                None,
                "2026-11-07 00:00:00",
                *(["я + שלום\n"] * (len(columns) - 2)),
            ]
            con.execute(
                f"INSERT INTO {table} VALUES ({','.join('?' for _ in columns)})", values
            )
            con.commit()

    def backup(self):
        output = self.root / "backup"
        user_data.backup(self.source, output, True)
        return output

    def test_committed_wal_data_restores_exactly_without_original(self):
        backup = self.backup()
        self.connections.close()
        self.source.rename(self.root / "unavailable-source")
        restored = self.root / "restored"
        user_data.restore(backup, restored)
        for table in user_data.TABLES:
            name = table + ".sqlite"
            self.assertEqual(
                (backup / name).read_bytes(), (restored / name).read_bytes()
            )
            self.assertEqual(user_data.inspect(restored / name, table)["rows"], 1)
            self.assertEqual((restored / name).stat().st_mode & 0o777, 0o600)
        self.assertEqual(backup.stat().st_mode & 0o777, 0o700)
        self.assertFalse(list(backup.glob("*-wal")))

    def test_requires_stopped_writers_and_never_creates_missing_source(self):
        with self.assertRaises(ValueError):
            user_data.backup(self.source, self.root / "refused", False)
        self.assertFalse((self.root / "refused").exists())
        with self.assertRaises(ValueError):
            user_data.backup(self.root / "missing", self.root / "refused", True)
        self.assertFalse((self.root / "missing").exists())

    def test_wrong_column_types_rejected_before_backup_creation(self):
        self.connections.close()
        path = self.source / "feedback.sqlite"
        path.unlink()
        with closing(sqlite3.connect(path)) as db:
            db.execute(
                "CREATE TABLE feedback (id INTEGER PRIMARY KEY, time, name, email, message)"
            )
        output = self.root / "refused"
        with self.assertRaises(ValueError):
            user_data.backup(self.source, output, True)
        self.assertFalse(output.exists())

    def test_existing_backup_and_restore_destinations_are_untouched(self):
        backup = self.backup()
        old = (backup / "manifest.json").read_bytes()
        with self.assertRaises(FileExistsError):
            user_data.backup(self.source, backup, True)
        with self.assertRaises(FileExistsError):
            user_data.restore(backup, self.source)
        self.assertEqual((backup / "manifest.json").read_bytes(), old)

    def test_corrupt_or_incomplete_backup_cannot_create_restore(self):
        backup = self.backup()
        target = self.root / "refused"
        (backup / "startup.sqlite").write_bytes(b"corrupt")
        with self.assertRaises(sqlite3.DatabaseError):
            user_data.restore(backup, target)
        self.assertFalse(target.exists())
        (backup / "manifest.json").unlink()
        with self.assertRaises(FileNotFoundError):
            user_data.restore(backup, target)
        self.assertFalse(target.exists())

    def test_changed_values_and_sidecars_are_rejected(self):
        backup = self.backup()
        with closing(sqlite3.connect(backup / "feedback.sqlite")) as con:
            con.execute("UPDATE feedback SET message='changed'")
            con.commit()
        with self.assertRaises(ValueError):
            user_data.validate_bundle(backup)
        manifest = json.loads((backup / "manifest.json").read_text())
        manifest["databases"]["feedback.sqlite"]["sha256"] = user_data.digest(
            backup / "feedback.sqlite"
        )
        (backup / "manifest.json").write_text(json.dumps(manifest))
        (backup / "feedback.sqlite-wal").touch()
        with self.assertRaises(ValueError):
            user_data.validate_bundle(backup)


if __name__ == "__main__":
    unittest.main()
