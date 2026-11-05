"""ASGI integration: routing, middleware, rendering and real SQLite storage."""

import sqlite3

import pytest
from conftest import CHAPTER
from lxml import etree, html

pytestmark = pytest.mark.api
FORM = {"Content-Type": "application/x-www-form-urlencoded"}


def test_catalog_translation_chapter_flow(client):
    response = client.get("/meta")
    assert response.status_code == 200
    assert response.headers["content-type"].startswith("application/xml")
    catalog = etree.fromstring(response.content)
    assert catalog.xpath("./translations/trans/@code") == ["sample", "nwt"]
    assert catalog.xpath('./translations/trans[@code="sample"]/@rtl') == ["1"]
    assert catalog.xpath('./translations/trans[@code="sample"]/transLang/@name') == [
        "Sample <&>"
    ]
    assert catalog.xpath("./languages/language/@code") == ["e", "u"]
    for translation in ("sample", "nwt"):
        response = client.get(
            "/translation", params={"trans": translation, "lang": "e"}
        )
        assert response.status_code == 200
        tree = etree.fromstring(response.content)
        assert tree.xpath("./book/@code") == ["1co"]
        assert tree.xpath("./book/chapter/@no") == (
            ["1", "2"] if translation == "sample" else ["1"]
        )
        for chapter in tree.xpath("./book/chapter/@no"):
            response = client.get(
                "/chapter",
                params={
                    "trans": translation,
                    "lang": "e",
                    "book": "1co",
                    "chapter": chapter,
                },
            )
            assert response.status_code == 200
            assert (
                response.content
                == (CHAPTER if chapter == "1" else "<p>next</p>").encode()
            )


@pytest.mark.parametrize("path", ["/startup", "/feedback"])
def test_post_commits_before_success_and_preserves_qt_text(client, api_data, path):
    body = (
        b"os=meego&device_id=a+b"
        if path == "/startup"
        else b"name=%D1%8F&email=user+tag&message=%D7%A9%D7%9C%D7%95%D7%9D%26%3D%0D%0A"
    )
    for _ in range(2):
        response = client.post(path, content=body, headers=FORM)
        assert response.status_code == 200
        assert response.content == b"ok"
        assert response.headers["content-type"].startswith("text/plain")
    table = path[1:]
    with sqlite3.connect(api_data / f"{table}.sqlite") as observer:
        if table == "startup":
            assert (
                observer.execute(
                    "SELECT os, device_id, trans, lang FROM startup ORDER BY id"
                ).fetchall()
                == [("meego", "a+b", None, None)] * 2
            )
        else:
            assert (
                observer.execute(
                    "SELECT name, email, message FROM feedback ORDER BY id"
                ).fetchall()
                == [("я", "user+tag", "שלום&=\r\n")] * 2
            )


@pytest.mark.parametrize(
    "path,valid,invalid",
    [
        (
            "/startup",
            b"os=&device_id=",
            [
                b"",
                b"os=x",
                b"device_id=x",
                b"os=x&device_id=%FF",
                b"os=x&device_id=x&os=y",
                b"os[]=x&device_id=x",
            ],
        ),
        (
            "/feedback",
            b"name=&email=&message=",
            [
                b"",
                b"name=&email=",
                b"name=&message=",
                b"email=&message=",
                b"name=&email=&message=%FF",
                b"name=x&name=y&email=&message=",
                b"name[]=&email=&message=",
            ],
        ),
    ],
)
def test_invalid_forms_return_400_without_changing_existing_rows(
    client, api_data, path, valid, invalid
):
    assert client.post(path, content=valid, headers=FORM).status_code == 200
    table = path[1:]
    with sqlite3.connect(api_data / f"{table}.sqlite") as observer:
        before = observer.execute(f"SELECT * FROM {table}").fetchall()
        for body in invalid:
            response = client.post(path, content=body, headers=FORM)
            assert response.status_code == 400, (body, response.text)
            assert response.headers["content-type"].startswith("text/plain")
            assert observer.execute(f"SELECT * FROM {table}").fetchall() == before


@pytest.mark.parametrize(
    "path,body",
    [("/startup", b"os=&device_id="), ("/feedback", b"name=&email=&message=")],
)
def test_failed_sqlite_insert_returns_500_without_new_row(client, api_data, path, body):
    assert client.post(path, content=body, headers=FORM).status_code == 200
    table = path[1:]
    with sqlite3.connect(api_data / f"{table}.sqlite") as observer:
        before = observer.execute(f"SELECT * FROM {table}").fetchall()
        observer.execute(
            f"CREATE TRIGGER reject_insert BEFORE INSERT ON {table} BEGIN SELECT RAISE(ABORT, 'blocked'); END"
        )
    response = client.post(path, content=body, headers=FORM)
    assert response.status_code == 500
    assert response.content != b"ok"
    with sqlite3.connect(api_data / f"{table}.sqlite") as observer:
        assert observer.execute(f"SELECT * FROM {table}").fetchall() == before


@pytest.mark.parametrize("path", ["/startup", "/feedback"])
@pytest.mark.parametrize("method", ["GET", "HEAD", "PUT", "DELETE"])
def test_write_route_methods_do_not_create_databases(client, api_data, path, method):
    response = client.request(method, path)
    assert response.status_code == 405
    assert "POST" in response.headers["allow"]
    assert not (api_data / "startup.sqlite").exists()
    assert not (api_data / "feedback.sqlite").exists()


def test_passage_redirect_and_rendering(client):
    response = client.get("/t/sample/e/1co/1/3,1,1")
    assert response.status_code in (301, 307)
    assert response.headers["location"] == "/t/sample/e/1co/1/1,3"
    response = client.get(response.headers["location"])
    assert response.status_code == 200
    tree = html.fromstring(response.content)
    assert tree.xpath("//*[@verse]/@verse") == ["1", "3", "3"]
    assert tree.xpath('//*[@class="superscript"]/text()') == ["Title"]
    assert tree.xpath('//*[@class="hole"]/text()') == ["[...] "]
    assert tree.xpath("//h2/text()") == ["Book <&> 1:1,3"]
    description = tree.xpath('//meta[@property="og:description"]/@content')[0]
    assert "שלום я" in description
    assert "three A" in description and "three B" in description
    assert "two" not in description
    assert not tree.xpath("//h2/*")  # Book name is escaped, not interpreted as HTML.


@pytest.mark.parametrize(
    "path,status",
    [
        ("/chapter", 400),
        ("/chapter?trans=absent&lang=e&book=1co&chapter=1", 400),
        ("/chapter?trans=sample&lang=e&book=1co&chapter=99", 400),
        ("/t/sample/e/1co/99/1", 400),
        ("/t/sample/e/1co/1/-1", 400),
        ("/fb", 404),
        ("/nwtbible", 404),
        ("/j2me", 404),
        ("/unknown", 404),
    ],
)
def test_errors(client, path, status):
    response = client.get(path)
    assert response.status_code == status
    assert not response.headers["content-type"].startswith("application/json")


@pytest.mark.parametrize("path", ["/", "/privacy", "/donate", "/donate-mobile"])
def test_retained_pages(client, path):
    response = client.get(path)
    assert response.status_code == 200
    assert response.headers["content-type"].startswith("text/html")
    assert html.fromstring(response.content).xpath("//title")


@pytest.mark.parametrize(
    "path",
    [
        "/meta",
        "/translation?trans=sample&lang=e",
        "/chapter?trans=sample&lang=e&book=1co&chapter=1",
        "/t/sample/e/1co/1/1",
    ],
)
def test_head_has_get_headers_without_body(client, path):
    get = client.get(path, headers={"Accept-Encoding": "identity"})
    head = client.head(path, headers={"Accept-Encoding": "identity"})
    assert head.status_code == get.status_code == 200
    assert head.content == b""
    assert head.headers["content-type"] == get.headers["content-type"]
    assert head.headers["content-length"] == get.headers["content-length"]


def test_gzip_page(client):
    identity = client.get("/", headers={"Accept-Encoding": "identity"})
    compressed = client.get("/", headers={"Accept-Encoding": "gzip"})
    assert compressed.status_code == identity.status_code == 200
    assert compressed.headers["content-encoding"] == "gzip"
    assert "accept-encoding" in compressed.headers["vary"].lower()
    assert compressed.content == identity.content  # httpx decodes gzip.


@pytest.mark.parametrize(
    "spec,expected",
    [
        ("1,", ["1"]),
        ("1,,2", ["1", "2"]),
        ("1,2-", ["1"]),
        ("9", []),
        ("3-4", ["3", "3"]),
    ],
)
def test_passage_malformed_entries_and_missing_verses(client, spec, expected):
    response = client.get(f"/t/sample/e/1co/1/{spec}", follow_redirects=True)
    assert response.status_code == 200
    assert html.fromstring(response.content).xpath("//*[@verse]/@verse") == expected


def test_static_file_head_range_and_validator(client):
    response = client.get("/static/img/favicon.ico")
    assert response.status_code == 200
    assert response.content
    head = client.head("/static/img/favicon.ico")
    assert head.status_code == 200
    assert head.content == b""
    assert int(head.headers["content-length"]) == len(response.content)
    cached = client.get(
        "/static/img/favicon.ico", headers={"If-None-Match": response.headers["etag"]}
    )
    assert cached.status_code == 304
    assert cached.content == b""
    partial = client.get("/static/img/favicon.ico", headers={"Range": "bytes=0-9"})
    assert partial.status_code == 206
    assert partial.content == response.content[:10]
    assert partial.headers["content-range"] == f"bytes 0-9/{len(response.content)}"
