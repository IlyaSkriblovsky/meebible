from pathlib import Path

from fastapi import FastAPI
from fastapi.middleware.gzip import GZipMiddleware
from fastapi.responses import HTMLResponse, Response
from fastapi.staticfiles import StaticFiles
from jinja2 import Environment, FileSystemLoader

from backend.storage import load_library, load_publication_structure

TEMPLATES = Path(__file__).parent / "templates"
STATIC = Path(__file__).parent / "static"

templates_env = Environment(loader=FileSystemLoader(TEMPLATES), autoescape=True)

app = FastAPI()
app.add_middleware(GZipMiddleware, minimum_size=1000, compresslevel=5)
app.mount("/static", StaticFiles(directory=STATIC), name="static")


class XMLResponse(Response):
    media_type = "application/xml"


@app.get("/")
def get_root():
    library = load_library()
    html = templates_env.get_template("index.html").render(library=library)
    return HTMLResponse(content=html)


@app.api_route("/meta", methods=["GET", "HEAD"])
def get_meta():
    library = load_library()
    xml = templates_env.get_template("meta.xml").render(library=library)
    return XMLResponse(content=xml)


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
