import re
from pathlib import Path

from fastapi import FastAPI, Request
from fastapi.exceptions import HTTPException, RequestValidationError
from fastapi.middleware.gzip import GZipMiddleware
from fastapi.responses import (
    FileResponse,
    HTMLResponse,
    PlainTextResponse,
    RedirectResponse,
    Response,
)
from fastapi.staticfiles import StaticFiles
from jinja2 import Environment, FileSystemLoader
from lxml import etree

from backend.storage import (
    ChapterNotFoundError,
    load_book_name,
    load_chapter_html,
    load_library,
    load_publication_structure,
    save_feedback,
    save_startup_event,
)
from backend.utils import parse_qt_form
from backend.verse_spec import InvalidVerseSpec, build_verses_spec, parse_verse_spec

TEMPLATES = Path(__file__).parent / "templates"
STATIC = Path(__file__).parent / "static"

templates_env = Environment(loader=FileSystemLoader(TEMPLATES), autoescape=True)


app = FastAPI()
app.add_middleware(GZipMiddleware, minimum_size=1000, compresslevel=5)
# app.add_middleware(ETagMiddleware)
static_files = StaticFiles(directory=STATIC)
app.mount("/static", static_files, name="static")


class XMLResponse(Response):
    media_type = "application/xml"


@app.get("/", response_class=HTMLResponse)
def get_root():
    library = load_library()
    return templates_env.get_template("index.html").render(library=library)


@app.exception_handler(404)
def handle_404(request: Request, exc: HTTPException):
    return HTMLResponse(
        templates_env.get_template("404.html").render(), status_code=404
    )


@app.exception_handler(405)
def handle_405(request: Request, exc: HTTPException):
    return PlainTextResponse("Method Not Allowed", status_code=405, headers=exc.headers)


@app.exception_handler(RequestValidationError)
def handle_422(request: Request, exc: RequestValidationError):
    return PlainTextResponse(content=str(exc), status_code=400)


@app.get("/favicon.ico", include_in_schema=False)
async def favicon(request: Request):
    return await static_files.get_response("img/favicon.ico", request.scope)


@app.get("/privacy", response_class=HTMLResponse)
def get_privacy():
    return templates_env.get_template("privacy.html").render()


@app.get("/donate", response_class=HTMLResponse)
def get_donate():
    return templates_env.get_template("donate.html").render()


@app.get("/donate-mobile", response_class=HTMLResponse)
def get_donate_mobile():
    return templates_env.get_template("donate-mobile.html").render()


@app.post("/startup")
async def post_startup(request: Request):
    try:
        form = parse_qt_form(await request.body())
        if not {"os", "device_id"}.issubset(form):
            raise ValueError("Missing fields")
    except ValueError:
        return PlainTextResponse("Invalid form", status_code=400)

    save_startup_event(
        platform=form["os"],
        device_id=form["device_id"],
        trans_code=form.get("trans"),
        lang_code=form.get("lang"),
    )
    return PlainTextResponse("ok")


@app.api_route("/meta", methods=["GET", "HEAD"], response_class=XMLResponse)
def get_meta():
    library = load_library()
    return templates_env.get_template("meta.xml").render(library=library)


@app.api_route("/translation", methods=["GET", "HEAD"])
def get_translation(trans: str = "", lang: str = ""):
    if not trans or not lang:
        return Response(
            content="<error>trans and lang parameters are required</error>",
            media_type="application/xml",
            status_code=200,
        )

    structure = load_publication_structure(trans, lang)
    xml = templates_env.get_template("translation.xml").render(structure=structure)
    return XMLResponse(content=xml)


@app.api_route("/chapter", methods=["get", "head"])
def get_chapter(trans: str = "", lang: str = "", book: str = "", chapter: str = ""):
    if not all([trans, lang, book, chapter]) or chapter == "0":
        return PlainTextResponse("Not all parameters specified", status_code=400)

    try:
        chapter_no = int(chapter)
        assert 0 < chapter_no
        if chapter_no > 1000:
            # Reproducing PHP value for the sake of automatic external tests
            return PlainTextResponse("Chapter does not exists", status_code=400)
        assert re.match(r"^[a-z0-9_]+$", trans)
        assert re.match(r"^[a-z]+$", lang)
    except (ValueError, AssertionError):
        return PlainTextResponse("Invalid parameters", status_code=400)

    try:
        content = load_chapter_html(trans, lang, book, chapter_no)
        return HTMLResponse(content=content)
    except ChapterNotFoundError:
        return PlainTextResponse("Chapter does not exists", status_code=400)


@app.api_route(
    "/t/{trans_code}/{lang_code}/{book_code}/{chapter_no}/{verses_spec}",
    methods=["GET", "HEAD"],
)
def get_place(
    trans_code: str, lang_code: str, book_code: str, chapter_no: int, verses_spec: str
):
    try:
        verses = parse_verse_spec(verses_spec)
    except InvalidVerseSpec:
        return PlainTextResponse("Strange verse specification", status_code=400)

    normalized_verse_spec = build_verses_spec(verses)
    if verses_spec != normalized_verse_spec:
        return RedirectResponse(
            f"/t/{trans_code}/{lang_code}/{book_code}/{chapter_no}/{normalized_verse_spec}",
            status_code=301,
        )

    try:
        content = load_chapter_html(trans_code, lang_code, book_code, chapter_no)
    except ChapterNotFoundError:
        return PlainTextResponse("Chapter does not exists", status_code=400)

    parser = etree.HTMLParser()
    html_root = etree.fromstring(content, parser)

    out_root = etree.Element("div")

    prev_parent = None
    out_parent = None
    for i, verse in enumerate(verses):
        if i > 0 and verses[i - 1] + 1 != verse:
            hole = etree.Element("span")
            hole.set("class", "hole")
            hole.text = "[...] "
            out_root.append(hole)

        nodes = []

        if verse == 1:
            nodes.extend(html_root.xpath(".//*[@class='superscript']"))
        nodes.extend(html_root.xpath(f".//*[@verse='{verse}']"))

        for node in nodes:
            node_parent = node.getparent()
            if node_parent != prev_parent:
                out_parent = etree.Element(node_parent.tag)
                for k, v in node_parent.items():
                    out_parent.set(k, v)
                out_root.append(out_parent)
                prev_parent = node_parent

            out_parent.append(node)
            node.tail = " "

    book_name = load_book_name(trans_code, lang_code, book_code)

    html = etree.tostring(out_root, pretty_print=True).decode()

    for element in out_root.xpath(".//*[@class='verse-label']"):
        element.text = ""
    for element in out_root.xpath(".//br"):
        element.tail = " " + (element.tail or "")
    text = (
        etree.tostring(out_root, method="text", encoding="utf-8")
        .decode()
        .replace("  ", " ")
        .strip()
    )
    place = f"{book_name} {chapter_no}:{normalized_verse_spec}".replace("-", "–")

    content = templates_env.get_template("place.html").render(
        html=html,
        text=text,
        place=place,
    )
    return HTMLResponse(content=content)


@app.post("/feedback")
async def post_feedback(request: Request):
    try:
        form = parse_qt_form(await request.body())
    except (ValueError, AssertionError):
        return PlainTextResponse("Invalid parameters", status_code=400)

    try:
        name = form["name"]
        email = form["email"]
        message = form["message"]
    except KeyError:
        return PlainTextResponse("Missing parameters", status_code=400)

    save_feedback(
        name=name,
        email=email,
        message=message,
    )
    return PlainTextResponse("ok")
