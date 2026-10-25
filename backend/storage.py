import sqlite3
from collections.abc import Iterator
from contextlib import contextmanager
from dataclasses import dataclass
from pathlib import Path

DB_PATH = Path(__file__).parent / "content"


@contextmanager
def connect_db(filename: str, write=False) -> Iterator[sqlite3.Connection]:
    suffix = "" if write else "?mode=ro"
    connection = sqlite3.connect(f"file:{DB_PATH / filename}{suffix}", uri=True)
    try:
        with connection:
            yield connection
    finally:
        connection.close()


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


class Library:
    def __init__(
        self,
        langs: dict[str, Lang],
        translations: dict[str, Translation],
        publications: dict[str, Publication],
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
    with connect_db("langs.sqlite") as con:
        res = con.execute("SELECT code, engname, selfname FROM langs")
        for code, engname, selfname in res:
            langs.append(Lang(code=code, engname=engname, selfname=selfname))

    translations: list[Translation] = []
    publication: list[Publication] = []

    with connect_db("trans.sqlite") as con:
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
