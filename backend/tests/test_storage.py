# ruff: noqa: SIM117
"""SQLite tests use disposable files, never the shipped content or user data."""

import re
import sqlite3
from unittest.mock import patch

import pytest

from backend import storage


class TestStorage:
    @pytest.fixture(autouse=True)
    def isolated_storage(self, tmp_path, monkeypatch):
        self.root = tmp_path
        monkeypatch.setattr(storage, "DB_PATH", tmp_path)
        monkeypatch.setattr(storage, "STARTUP_DB", tmp_path / "startup.sqlite")
        monkeypatch.setattr(storage, "FEEDBACK_DB", tmp_path / "feedback.sqlite")

    def create(self, name, sql):
        path = self.root / name
        path.parent.mkdir(parents=True, exist_ok=True)
        with sqlite3.connect(path) as con:
            con.executescript(sql)
        return path

    def test_missing_content_does_not_create_file(self):
        with pytest.raises(FileNotFoundError):
            with storage.connect_content_db("absent.sqlite"):
                pytest.fail("missing database opened")
        with pytest.raises(sqlite3.OperationalError):
            with storage.connect_db(self.root / "absent.sqlite"):
                pytest.fail("missing database opened")
        assert not (self.root / "absent.sqlite").exists()

    def test_content_is_read_only_and_unchanged(self):
        path = self.create(
            "sample.sqlite",
            "CREATE TABLE sample (value); INSERT INTO sample VALUES ('שלום');",
        )
        before = path.read_bytes()
        with storage.connect_content_db("sample.sqlite") as con:
            assert con.execute("SELECT value FROM sample").fetchone() == ("שלום",)
            with pytest.raises(sqlite3.OperationalError):
                con.execute("INSERT INTO sample VALUES ('bad')")
        assert path.read_bytes() == before
        with pytest.raises(sqlite3.ProgrammingError):
            con.execute("SELECT 1")

    def test_commit_rollback_and_close(self):
        path = self.create("data.sqlite", "CREATE TABLE data (value UNIQUE);")
        with storage.connect_db(path, write=True) as con:
            con.execute("INSERT INTO data VALUES ('kept')")
        for failure in (RuntimeError, sqlite3.IntegrityError):
            with pytest.raises(failure):
                with storage.connect_db(path, write=True) as con:
                    con.execute("INSERT INTO data VALUES ('rolled back')")
                    if failure is RuntimeError:
                        raise RuntimeError("abort")
                    con.execute("INSERT INTO data VALUES ('kept')")
            with sqlite3.connect(path) as observer:
                assert observer.execute("SELECT * FROM data").fetchall() == [("kept",)]
            with pytest.raises(sqlite3.ProgrammingError):
                con.execute("SELECT 1")

    def test_committed_startup_null_empty_and_repeated_values(self):
        storage.save_startup_event("symbian", "", None, None)
        storage.save_startup_event("symbian", "", "", "")
        with sqlite3.connect(storage.STARTUP_DB) as con:
            assert con.execute(
                "SELECT os, device_id, trans, lang FROM startup ORDER BY id"
            ).fetchall() == [("symbian", "", None, None), ("symbian", "", "", "")]
            assert [r[1] for r in con.execute("PRAGMA table_info(startup)")] == [
                "id",
                "time",
                "device_id",
                "os",
                "lang",
                "trans",
            ]
            assert re.search(
                "^\\d{4}-\\d\\d-\\d\\d \\d\\d:\\d\\d:\\d\\d$",
                con.execute("SELECT time FROM startup").fetchone()[0],
            )

    def test_feedback_preserves_text_and_repeated_submissions(self):
        text = "שלום я + '&=\r\n'); DROP TABLE feedback;--"
        for _ in range(2):
            storage.save_feedback("", "user+tag", text)
        with sqlite3.connect(storage.FEEDBACK_DB) as con:
            assert (
                con.execute(
                    "SELECT name, email, message FROM feedback ORDER BY id"
                ).fetchall()
                == [("", "user+tag", text)] * 2
            )
            assert [r[1] for r in con.execute("PRAGMA table_info(feedback)")] == [
                "id",
                "time",
                "name",
                "email",
                "message",
            ]

    def test_failed_inserts_propagate_and_preserve_existing_rows(self):
        for save, path, table in (
            (
                lambda: storage.save_feedback("n", "e", "m"),
                storage.FEEDBACK_DB,
                "feedback",
            ),
            (
                lambda: storage.save_startup_event("o", "d", None, None),
                storage.STARTUP_DB,
                "startup",
            ),
        ):
            save()
            with sqlite3.connect(path) as con:
                con.execute(
                    f"CREATE TRIGGER reject_insert BEFORE INSERT ON {table} BEGIN SELECT RAISE(ABORT, 'blocked'); END"
                )
            with pytest.raises(sqlite3.IntegrityError):
                save()
            with sqlite3.connect(path) as con:
                assert con.execute(f"SELECT count(*) FROM {table}").fetchone()[0] == 1

    def test_unwritable_database_target_propagates(self):
        with patch.object(storage, "FEEDBACK_DB", self.root):
            with pytest.raises(sqlite3.OperationalError):
                storage.save_feedback("", "", "")

    def test_chapter_selection_and_nwt_layout(self):
        sql = "CREATE TABLE html (langCode, bookCode, chapterNo, html); INSERT INTO html VALUES ('e','1co',1,'<p>שלום я</p>'), ('u','1co',1,'other'), ('e','1co',2,'next');"
        self.create("sample.sqlite", sql)
        self.create("nwt/nwt_e.sqlite", sql)
        for translation in ("sample", "nwt"):
            assert (
                storage.load_chapter_html(translation, "e", "1co", 1) == "<p>שלום я</p>"
            )
            with pytest.raises(storage.ChapterNotFoundError):
                storage.load_chapter_html(translation, "e", "1co", 3)
            with pytest.raises(storage.ChapterNotFoundError):
                storage.load_chapter_html(translation, "e", "' OR 1=1 --", 1)
        with pytest.raises(storage.ChapterNotFoundError):
            storage.load_chapter_html("absent", "e", "ge", 1)

    def test_commit_failure_rolls_back(self):
        path = self.create(
            "deferred.sqlite",
            "CREATE TABLE parent (id PRIMARY KEY); CREATE TABLE child (parent_id REFERENCES parent(id) DEFERRABLE INITIALLY DEFERRED);",
        )
        with pytest.raises(sqlite3.IntegrityError):
            with storage.connect_db(path, write=True) as con:
                con.execute("PRAGMA foreign_keys=ON")
                con.execute("INSERT INTO child VALUES (99)")
                assert con.execute("SELECT count(*) FROM child").fetchone()[0] == 1
        with sqlite3.connect(path) as observer:
            assert observer.execute("SELECT * FROM child").fetchall() == []
        with pytest.raises(sqlite3.ProgrammingError):
            con.execute("SELECT 1")
