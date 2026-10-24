"""HTTP and read-only test observations; no imports from a backend implementation."""

import base64
import gzip
import hashlib
import http.client
import json
import sqlite3
import subprocess
from dataclasses import dataclass
from html.parser import HTMLParser
from pathlib import Path
from urllib.parse import urlsplit
import xml.etree.ElementTree as ET

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
REFERENCE = HERE / "reference"
COMPOSE = ["docker", "compose", "-f", str(ROOT / "tools/php-reference/compose.yml")]


def need(condition, message):
    if not condition:
        raise AssertionError(message)


def sha(data):
    return hashlib.sha256(data).hexdigest()


def local_url(value):
    url = urlsplit(value)
    if (url.scheme != "http" or url.hostname not in {"127.0.0.1", "localhost", "::1"}
            or url.username or url.password or url.path not in {"", "/"}
            or url.query or url.fragment):
        raise ValueError("BASE_URL must be a plain HTTP loopback origin, for example http://127.0.0.1:8000")
    return url


@dataclass
class Response:
    status: int
    headers: list
    wire: bytes
    body: bytes

    def header(self, name, default=""):
        return next((value for key, value in self.headers if key.lower() == name.lower()), default)

    def record(self):
        return {"status": self.status, "headers": self.headers, "wire_bytes": len(self.wire),
                "body_bytes": len(self.body), "body_sha256": sha(self.body)}


def request(base_url, case):
    url = local_url(base_url)
    connection = http.client.HTTPConnection(url.hostname, url.port or 80, timeout=15)
    headers = {"Host": "meebible.org", "Accept-Encoding": "identity", **case.get("headers", {})}
    body = case.get("body")
    if body is not None:
        body = body.encode("ascii")
        headers.setdefault("Content-Type", "application/x-www-form-urlencoded")
    try:
        connection.request(case.get("method", "GET"), case["path"], body, headers)
        result = connection.getresponse()
        wire = result.read(16 * 1024 * 1024 + 1)
        need(len(wire) <= 16 * 1024 * 1024, "Response exceeds test size limit")
        response = Response(result.status, result.getheaders(), wire, wire)
        if case.get("method") != "HEAD":
            length = response.header("Content-Length")
            need(not length or int(length) == len(wire), "Content-Length does not match the entity bytes")
            if response.header("Content-Encoding").lower() == "gzip":
                response.body = gzip.decompress(wire)
        else:
            need(not wire, "HEAD returned a body")
        return response
    finally:
        connection.close()


def xml_value(body):
    def node(element):
        return (element.tag, sorted(element.attrib.items()), (element.text or "").strip(),
                [node(child) for child in element])
    return node(ET.fromstring(body))


def words(text):
    return " ".join(text.split())


class Page(HTMLParser):
    """Extract visible content and passage features without executing scripts."""

    def __init__(self, body):
        super().__init__(convert_charrefs=True)
        self.stack = []
        self.text = []
        self.links = []
        self.images = []
        self.forms = []
        self.meta = {}
        self.titles = []
        self.verses = []
        self.superscripts = []
        self.holes = []
        self.feed(body.decode("utf-8"))

    def handle_starttag(self, tag, attributes):
        attrs = dict(attributes)
        node = {"tag": tag, "attrs": attrs, "text": []}
        if tag == "meta" and attrs.get("property", "").startswith("og:"):
            self.meta[attrs["property"]] = attrs.get("content", "")
        if tag in {"a", "link"} and "href" in attrs:
            self.links.append(attrs["href"])
        if tag == "img" and "src" in attrs:
            self.images.append(attrs["src"])
        if tag == "form":
            self.forms.append({"attrs": sorted(attrs.items()), "inputs": []})
        if tag == "input" and self.forms:
            self.forms[-1]["inputs"].append(sorted(attrs.items()))
        if "verse" in attrs:
            self.verses.append(node)
        if attrs.get("class") == "superscript":
            self.superscripts.append(node)
        if attrs.get("class") == "hole":
            self.holes.append(node)
        if tag in {"title", "h2"}:
            self.titles.append(node)
        if tag not in {"area", "base", "br", "col", "embed", "hr", "img", "input", "link", "meta", "param", "source", "track", "wbr"}:
            self.stack.append(node)

    def handle_startendtag(self, tag, attrs):
        self.handle_starttag(tag, attrs)
        if self.stack and self.stack[-1]["tag"] == tag:
            self.handle_endtag(tag)

    def handle_endtag(self, tag):
        for index in range(len(self.stack) - 1, -1, -1):
            if self.stack[index]["tag"] == tag:
                del self.stack[index:]
                break

    def handle_data(self, data):
        if any(node["tag"] in {"script", "style"} for node in self.stack):
            return
        self.text.append(data)
        for node in self.stack:
            node["text"].append(data)

    def value(self, passage=False):
        result = {"text": words(" ".join(self.text)), "links": sorted(self.links),
                  "images": sorted(self.images), "forms": self.forms,
                  "titles": [words("".join(n["text"])) for n in self.titles]}
        if passage:
            result.update(meta={key: words(value) for key, value in self.meta.items()},
                          verses=[(n["attrs"]["verse"], words("".join(n["text"]))) for n in self.verses],
                          superscripts=[words("".join(n["text"])) for n in self.superscripts],
                          holes=[words("".join(n["text"])) for n in self.holes])
        return result


def database_value(value):
    try:
        return value.decode("utf-8")
    except UnicodeDecodeError:
        return {"invalid_utf8_base64": base64.b64encode(value).decode("ascii")}


def user_data(directory=None, php_reference=False):
    if php_reference:
        code = '''
        $out = array();
        foreach (array("startup", "feedback") as $table) {
            $out[$table] = array("rows" => array(), "columns" => array());
            if (!file_exists("/data/$table.sqlite")) continue;
            $db = new SQLite3("/data/$table.sqlite", SQLITE3_OPEN_READONLY);
            if (!$db->querySingle("SELECT count(*) FROM sqlite_master WHERE name='$table'")) continue;
            $r = $db->query("PRAGMA table_info($table)");
            while ($row = $r->fetchArray(SQLITE3_ASSOC)) $out[$table]["columns"][] = $row;
            $r = $db->query("SELECT * FROM $table ORDER BY id");
            while ($row = $r->fetchArray(SQLITE3_ASSOC)) {
                foreach ($row as $key => $v)
                    if (is_string($v) && !mb_check_encoding($v, "UTF-8"))
                        $row[$key] = array("invalid_utf8_base64" => base64_encode($v));
                $out[$table]["rows"][] = $row;
            }
        }
        echo json_encode($out);
        '''
        return json.loads(subprocess.check_output(
            [*COMPOSE, "exec", "-T", "php", "php", "-r", code], text=True))
    need(directory is not None, "POST checks need USER_DATA_DIR / --user-data, or --php-reference")
    result = {}
    for table in ("startup", "feedback"):
        result[table] = {"rows": [], "columns": []}
        path = Path(directory).resolve() / f"{table}.sqlite"
        if not path.exists():
            continue
        with sqlite3.connect(path.as_uri() + "?mode=ro", uri=True) as db:
            db.text_factory = database_value
            db.row_factory = sqlite3.Row
            if db.execute("SELECT count(*) FROM sqlite_master WHERE name=?", (table,)).fetchone()[0]:
                result[table] = {"rows": [dict(row) for row in db.execute(f"SELECT * FROM {table} ORDER BY id")],
                                 "columns": [dict(row) for row in db.execute(f"PRAGMA table_info({table})")]}
    return result


def delta(before, after):
    result = {}
    for table in ("startup", "feedback"):
        old = before[table]["rows"]
        current = after[table]["rows"]
        need(current[:len(old)] == old, f"{table}: existing rows changed or disappeared")
        result[table] = [{key: value for key, value in row.items() if key not in {"id", "time"}}
                         for row in current[len(old):]]
    return result


def load_reference():
    manifest = json.loads((REFERENCE / "manifest.json").read_text())
    for name, expected in manifest["files"].items():
        need(sha((REFERENCE / name).read_bytes()) == expected, f"Reference file changed: {name}")
    return manifest
