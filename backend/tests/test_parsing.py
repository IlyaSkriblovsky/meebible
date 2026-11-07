"""Contract-level parser examples; no HTTP server or reference corpus required."""

import pytest

from backend.utils import parse_qt_form
from backend.verse_spec import InvalidVerseSpec, build_verses_spec, parse_verse_spec


class TestQtForm:
    def test_qt_encoding(self):
        assert parse_qt_form(
            b"email=user+tag%40example.org&text=a%20b%26%3D%25%0D%0A&plus=%2B&literal=%252B"
        ) == {
            "email": "user+tag@example.org",
            "text": "a b&=%\r\n",
            "plus": "+",
            "literal": "%2B",
        }

    def test_unicode_keys_and_values(self):
        assert parse_qt_form("имя=שלום&x=%D1%8F".encode()) == {"имя": "שלום", "x": "я"}

    def test_empty_and_absent_are_distinct(self):
        assert parse_qt_form(b"os=&device_id=") == {"os": "", "device_id": ""}
        assert parse_qt_form(b"&&") == {}

    def test_split_only_first_equals_and_decode_once(self):
        assert parse_qt_form(b"x=a=b&%2526=%2525") == {"x": "a=b", "%26": "%25"}

    @pytest.mark.parametrize(
        "body",
        (
            b"x",
            b"=x",
            b"x=1&x=2",
            b"x=1&%78=2",
            b"x[]=1",
            b"x%5Ba%5D=1",
            b"x=\xff",
            b"%FF=x",
            b"x=%C3%28",
        ),
    )
    def test_invalid_scalar_fields(self, body):
        with pytest.raises(ValueError):
            parse_qt_form(body)


class TestVerse:
    @pytest.mark.parametrize(
        "spec,verses,canonical",
        (
            ("3,1,2,5,5", [1, 2, 3, 5], "1-3,5"),
            ("001,2-4,3", [1, 2, 3, 4], "1-4"),
            ("0,301,5-3,2", [2], "2"),
            ("1,300", [1, 300], "1,300"),
            ("1-300", list(range(1, 301)), "1-300"),
        ),
    )
    def test_selection_and_normalization(self, spec, verses, canonical):
        assert parse_verse_spec(spec) == verses
        assert build_verses_spec(verses) == canonical
        assert parse_verse_spec(canonical) == verses

    @pytest.mark.parametrize(
        "spec", ("", "0", "301", "3-1", "x", "1/2", "1 2", "١", "-1")
    )
    def test_empty_or_non_numeric_selection(self, spec):
        with pytest.raises(InvalidVerseSpec):
            parse_verse_spec(spec)

    @pytest.mark.parametrize("spec", ("1,", ",1", "1,,2", "1,-", "1,2-", "1,--"))
    def test_malformed_entries_are_ignored_when_valid_verses_remain(self, spec):
        assert parse_verse_spec(spec) == ([1, 2] if spec == "1,,2" else [1])

    def test_builder_sorts_and_handles_empty_iterable(self):
        assert build_verses_spec(iter([5, 2, 1, 3])) == "1-3,5"
        assert build_verses_spec([]) == ""


def test_repeated_verse_ranges_are_bounded():
    assert parse_verse_spec(",".join(["1-300"] * 10)) == list(range(1, 301))
    with pytest.raises(InvalidVerseSpec):
        parse_verse_spec(",".join(["1-300"] * 1000))
