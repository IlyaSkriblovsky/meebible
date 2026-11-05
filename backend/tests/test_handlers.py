"""Call handlers with synthetic requests and isolated storage/rendering seams."""

import asyncio
import sqlite3
from unittest.mock import AsyncMock, Mock, patch

import pytest
from lxml import etree

import backend
from backend.storage import ChapterNotFoundError


class TestPost:
    def call(self, handler, body):
        return asyncio.run(handler(Mock(body=AsyncMock(return_value=body))))

    def test_startup_missing_required_keys_never_writes(self):
        for body in (b"", b"os=x", b"device_id=x"):
            with patch.object(backend, "save_startup_event") as save:
                assert self.call(backend.post_startup, body).status_code == 400
                save.assert_not_called()

    def test_feedback_missing_required_keys_never_writes(self):
        for body in (b"", b"name=&email=", b"email=&message=", b"name=&message="):
            with patch.object(backend, "save_feedback") as save:
                assert self.call(backend.post_feedback, body).status_code == 400
                save.assert_not_called()

    def test_malformed_forms_never_write(self):
        for handler, target in (
            (backend.post_startup, "save_startup_event"),
            (backend.post_feedback, "save_feedback"),
        ):
            for body in (b"name=x&name=y", b"name[]=x", b"message=%FF"):
                with patch.object(backend, target) as save:
                    assert self.call(handler, body).status_code == 400
                    save.assert_not_called()

    def test_empty_fields_and_optional_startup_values(self):
        with patch.object(backend, "save_startup_event") as save:
            assert self.call(backend.post_startup, b"os=&device_id=").body == b"ok"
            save.assert_called_once_with(
                platform="", device_id="", trans_code=None, lang_code=None
            )
        with patch.object(backend, "save_feedback") as save:
            assert (
                self.call(backend.post_feedback, b"name=&email=&message=").body == b"ok"
            )
            save.assert_called_once_with(name="", email="", message="")

    def test_write_errors_cannot_return_ok(self):
        for handler, target, body in (
            (backend.post_startup, "save_startup_event", b"os=x&device_id=x"),
            (backend.post_feedback, "save_feedback", b"name=&email=&message="),
        ):
            with (
                patch.object(
                    backend,
                    target,
                    side_effect=sqlite3.OperationalError("write failed"),
                ),
                pytest.raises(sqlite3.OperationalError),
            ):
                self.call(handler, body)


class TestPassage:
    @pytest.fixture(autouse=True)
    def passage_dependencies(self, monkeypatch):
        self.chapter = '<html><body><p dir="rtl"><span class="superscript">Title</span><span verse="1"><b class="verse-label">1</b>שלום<br/>я</span><span verse="2">two</span><span verse="3">three A</span></p><p class="poetry"><span verse="3">three B</span></p></body></html>'
        self.load = Mock(return_value=self.chapter)
        monkeypatch.setattr(backend, "load_chapter_html", self.load)
        monkeypatch.setattr(backend, "load_book_name", Mock(return_value="Book <&>"))
        self.template = Mock()
        self.template.render.return_value = "rendered"
        monkeypatch.setattr(
            backend.templates_env, "get_template", Mock(return_value=self.template)
        )

    def call(self, spec):
        return backend.get_place("sample", "e", "1co", 1, spec)

    def test_split_verses_wrappers_superscript_gaps_and_metadata(self):
        assert self.call("1,3").status_code == 200
        context = self.template.render.call_args.kwargs
        tree = etree.fromstring(context["html"])
        assert tree.xpath(".//*[@verse]/@verse") == ["1", "3", "3"]
        assert tree.xpath(".//*[@class='superscript']/text()") == ["Title"]
        assert tree.xpath(".//*[@class='hole']/text()") == ["[...] "]
        assert tree.xpath("./p/@dir") == ["rtl"]
        assert tree.xpath("./p/@class") == ["poetry"]
        assert "שלום я" in context["text"]
        assert "three A" in context["text"]
        assert "three B" in context["text"]
        assert "1" not in context["text"]
        assert "two" not in context["text"]
        assert context["place"] == "Book <&> 1:1,3"
        self.load.assert_called_once_with("sample", "e", "1co", 1)

    def test_missing_and_partial_verses(self):
        for spec, expected in (("9", []), ("3-4", ["3", "3"])):
            assert self.call(spec).status_code == 200
            tree = etree.fromstring(self.template.render.call_args.kwargs["html"])
            assert tree.xpath(".//*[@verse]/@verse") == expected
            assert not tree.xpath(".//*[@class='superscript']")

    def test_normalization_precedes_content_loading(self):
        response = self.call("3,2,1,1")
        assert response.status_code == 301
        assert response.headers["location"] == "/t/sample/e/1co/1/1-3"
        self.load.assert_not_called()

    def test_missing_chapter_stops_before_rendering(self):
        self.load.side_effect = ChapterNotFoundError
        assert self.call("1").status_code == 400
        self.template.render.assert_not_called()

    def test_invalid_spec_stops_before_loading(self):
        assert self.call("abc").status_code == 400
        self.load.assert_not_called()
