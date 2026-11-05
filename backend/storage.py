import os
import sqlite3
from collections.abc import Iterator
from contextlib import contextmanager
from dataclasses import dataclass
from functools import partial
from itertools import groupby
from operator import itemgetter
from pathlib import Path

DB_PATH = Path(__file__).parent / "content"
USER_DATA_DIR = Path(os.environ.get("MEEBIBLE_DATA_DIR", "."))
STARTUP_DB = USER_DATA_DIR / "startup.sqlite"
FEEDBACK_DB = USER_DATA_DIR / "feedback.sqlite"


class BookNotFoundError(Exception):
    pass


class ChapterNotFoundError(Exception):
    pass


@contextmanager
def connect_db(filename: str | Path, write=False) -> Iterator[sqlite3.Connection]:
    suffix = "" if write else "?mode=ro"
    connection = sqlite3.connect(f"file:{filename}{suffix}", uri=True)
    try:
        with connection:
            yield connection
    finally:
        connection.close()


@contextmanager
def connect_content_db(relative_filename: str | Path) -> Iterator[sqlite3.Connection]:
    abs_path = DB_PATH / relative_filename

    if not os.path.exists(abs_path):
        raise FileNotFoundError(abs_path)

    with connect_db(abs_path) as con:
        yield con


connect_langs_db = partial(connect_content_db, "langs.sqlite")
connect_translations_db = partial(connect_content_db, "trans.sqlite")


@contextmanager
def connect_publication_db(
    trans_code: str, lang_code: str
) -> Iterator[sqlite3.Connection]:
    if trans_code == "nwt":
        db_path = Path("nwt") / f"nwt_{lang_code}.sqlite"
    else:
        db_path = f"{trans_code}.sqlite"

    with connect_content_db(db_path) as con:
        yield con


@dataclass(frozen=True)
class Lang:
    code: str
    engname: str
    selfname: str


@dataclass(frozen=True)
class Publication:
    """Pair (Translation, Language). Called trans_lang in PHP."""

    trans_code: str
    lang_code: str
    name: str


@dataclass(frozen=True)
class Translation:
    code: str
    name: str
    source_url: str
    copyright: str
    rtl: bool


@dataclass(frozen=True)
class ChapterHeader:
    no: int
    verse_count: int


@dataclass(frozen=True)
class Book:
    code: str
    name: str  # localized
    chapters: list[ChapterHeader]


@dataclass(frozen=True)
class PublicationStructure:
    books: list[Book]


class Library:
    def __init__(
        self,
        langs: dict[str, Lang],
        translations: dict[str, Translation],
        publications: dict[tuple[str, str], Publication],
    ):
        self._langs = langs
        self._translations = translations
        self._publications = publications

    @property
    def total_publications(self) -> int:
        return len(self._publications)

    @property
    def languages(self) -> list[Lang]:
        return sorted(self._langs.values(), key=lambda lang: lang.selfname)

    @property
    def translations(self) -> list[Translation]:
        # Intentionally unsorted, because PHP implementation didn't sort translations in /meta.
        return list(self._translations.values())

    @property
    def languages_with_publications(self) -> list[Lang]:
        return [
            lang
            for lang in self.languages
            if any(
                publication.lang_code == lang.code
                for publication in self._publications.values()
            )
        ]

    def publications_by_lang(self, lang: Lang) -> list[Publication]:
        publications = [
            publication
            for publication in self._publications.values()
            if publication.lang_code == lang.code
        ]
        return sorted(publications, key=lambda publication: publication.name)

    def publications_by_translation(
        self, translation: Translation
    ) -> list[Publication]:
        publications = [
            publication
            for publication in self._publications.values()
            if publication.trans_code == translation.code
        ]
        return sorted(publications, key=lambda publication: publication.name)


def load_library() -> Library:
    langs: list[Lang] = []
    with connect_langs_db() as con:
        res = con.execute("SELECT code, engname, selfname FROM langs")
        for code, engname, selfname in res:
            langs.append(Lang(code=code, engname=engname, selfname=selfname))

    translations: list[Translation] = []
    publication: list[Publication] = []

    with connect_translations_db() as con:
        res = con.execute(
            "SELECT transCode, name, sourceUrl, copyright, rtl FROM translations"
        )
        for trans_code, name, source_url, copyright, rtl in res:
            translations.append(
                Translation(
                    code=trans_code,
                    name=name,
                    source_url=source_url,
                    copyright=copyright,
                    rtl=bool(rtl),
                )
            )

        res = con.execute("SELECT transCode, langCode, name FROM translationLangs")
        for trans_code, lang_code, name in res:
            publication.append(
                Publication(
                    trans_code=trans_code,
                    lang_code=lang_code,
                    name=name,
                )
            )

    return Library(
        langs={lang.code: lang for lang in langs},
        translations={trans.code: trans for trans in translations},
        publications={
            (publication.trans_code, publication.lang_code): publication
            for publication in publication
        },
    )


def load_publication_structure(trans_code: str, lang_code: str) -> PublicationStructure:
    with connect_translations_db() as con:
        res = con.execute(
            "SELECT bookCode, chapterNo, verseCount FROM chapterSize WHERE transCode=:trans_code ORDER BY bookCode, chapterNo",
            {"trans_code": trans_code},
        )

        chapters_by_book_code: dict[str, list[ChapterHeader]] = {}
        for bookCode, group in groupby(res, itemgetter(0)):
            chapters_by_book_code[bookCode] = [
                ChapterHeader(no=chapter_no, verse_count=verse_count)
                for _, chapter_no, verse_count in group
            ]

        res = con.execute(
            "SELECT bookCode, bookName FROM books WHERE transCode=:trans_code AND langCode=:lang_code ORDER BY bookNo",
            {"trans_code": trans_code, "lang_code": lang_code},
        )

        books: list[Book] = [
            Book(code=bookCode, name=bookName, chapters=chapters_by_book_code[bookCode])
            for bookCode, bookName in res
        ]

    return PublicationStructure(books=books)


def load_chapter_html(
    trans_code: str, lang_code: str, book_code: str, chapter_no: int
) -> str:
    try:
        with connect_publication_db(trans_code, lang_code) as con:
            res = con.execute(
                "SELECT html FROM html WHERE langCode=:lang_code AND bookCode=:book_code AND chapterNo=:chapter_no",
                {
                    "lang_code": lang_code,
                    "book_code": book_code,
                    "chapter_no": chapter_no,
                },
            )

            try:
                first = next(iter(res))
                return first[0]
            except StopIteration:
                raise ChapterNotFoundError()
    except FileNotFoundError:
        raise ChapterNotFoundError()


def load_book_name(trans_code: str, lang_code: str, book_code: str) -> str:
    with connect_translations_db() as con:
        res = con.execute(
            "SELECT bookName FROM books WHERE transCode=:trans_code AND langCode=:lang_code AND bookCode=:book_code",
            {"trans_code": trans_code, "lang_code": lang_code, "book_code": book_code},
        )
        try:
            first = next(iter(res))
            return first[0]
        except StopIteration:
            raise BookNotFoundError()


def save_startup_event(
    platform: str,
    device_id: str,
    trans_code: str | None,
    lang_code: str | None,
):
    with connect_db(STARTUP_DB, write=True) as con:
        con.execute(
            """
            CREATE TABLE IF NOT EXISTS startup (
                id INTEGER PRIMARY KEY, 
                time DATETIME,
                device_id VARCHAR,
                os VARCHAR,
                lang VARCHAR,
                trans VARCHAR
            )
            """
        )
        con.execute(
            """
            INSERT INTO startup (time, device_id, os, lang, trans)
                VALUES (datetime('now'), :device_id, :os, :lang, :trans)
            """,
            {
                "device_id": device_id,
                "os": platform,
                "lang": lang_code,
                "trans": trans_code,
            },
        )


def save_feedback(name: str, email: str, message: str) -> None:
    with connect_db(FEEDBACK_DB, write=True) as con:
        con.execute(
            """
            CREATE TABLE IF NOT EXISTS feedback (
                id INTEGER PRIMARY KEY,
                time DATETIME,
                name VARCHAR,
                email VARCHAR,
                message VARCHAR
            )
            """
        )
        con.execute(
            """
            INSERT INTO feedback (time, name, email, message)
            VALUES (datetime('now'), :name, :email, :message)
            """,
            {
                "name": name,
                "email": email,
                "message": message,
            },
        )
