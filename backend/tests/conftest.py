"""Disposable API catalog and user data; no shipped database is opened."""

import sqlite3

import pytest
from fastapi.testclient import TestClient

from backend import app, storage

CHAPTER = '<html><body><p dir="rtl"><span class="superscript">Title</span><span verse="1"><b class="verse-label">1</b>שלום я</span><span verse="2">two</span><span verse="3">three A</span></p><p class="poetry"><span verse="3">three B</span></p></body></html>'


@pytest.fixture
def api_data(tmp_path, monkeypatch):
    monkeypatch.setattr(storage, "DB_PATH", tmp_path)
    monkeypatch.setattr(storage, "STARTUP_DB", tmp_path / "startup.sqlite")
    monkeypatch.setattr(storage, "FEEDBACK_DB", tmp_path / "feedback.sqlite")
    with sqlite3.connect(tmp_path / "langs.sqlite") as con:
        con.executescript(
            "CREATE TABLE langs (code, engname, selfname); INSERT INTO langs VALUES ('e', 'English', 'English'), ('u', 'Ukrainian', 'Українська');"
        )
    with sqlite3.connect(tmp_path / "trans.sqlite") as con:
        con.executescript("""
            CREATE TABLE translations (transCode, name, sourceUrl, copyright, rtl);
            INSERT INTO translations VALUES ('sample', 'Sample', 'http://example.invalid/?x=1&y=2', 'Owner <&>', 1), ('nwt', 'NWT', '', '', 0);
            CREATE TABLE translationLangs (transCode, langCode, name);
            INSERT INTO translationLangs VALUES ('sample', 'e', 'Sample <&>'), ('nwt', 'e', 'NWT');
            CREATE TABLE books (transCode, langCode, bookCode, bookName, bookNo);
            INSERT INTO books VALUES ('sample', 'e', '1co', 'Book <&>', 1), ('nwt', 'e', '1co', 'Book', 1);
            CREATE TABLE chapterSize (transCode, bookCode, chapterNo, verseCount);
            INSERT INTO chapterSize VALUES ('sample', '1co', 2, 1), ('sample', '1co', 1, 3), ('nwt', '1co', 1, 3);
        """)
    (tmp_path / "nwt").mkdir()
    for name in ("sample.sqlite", "nwt/nwt_e.sqlite"):
        with sqlite3.connect(tmp_path / name) as con:
            con.execute("CREATE TABLE html (langCode, bookCode, chapterNo, html)")
            con.executemany(
                "INSERT INTO html VALUES (?, ?, ?, ?)",
                [("e", "1co", 1, CHAPTER), ("e", "1co", 2, "<p>next</p>")],
            )
    return tmp_path


@pytest.fixture
def client(api_data):
    # Observe actual HTTP 500 responses rather than re-raising application errors.
    with TestClient(
        app, raise_server_exceptions=False, follow_redirects=False
    ) as test_client:
        yield test_client
