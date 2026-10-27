"""
Verse Spec is a sorted list of verse numbers or ranges:
1-5,7,9-10  -->  [1,2,3,4,5,7,9,10]

Normalized Verse Spec also guarantees:
- all verse numbers and ranges are sorted
- any consecutive verse numbers are represented as a range

Verse Set is a set of individual verse numbers as ints
"""

import re
from collections.abc import Iterable


class InvalidVerseSpec(Exception):
    pass


def parse_verse_spec(spec: str) -> list[int]:
    """Converts Verse Spec to a Verse Set"""
    if not re.match(r"^[0-9,-]+$", spec):
        raise InvalidVerseSpec()

    ranges = spec.split(",")
    verses: list[int] = []

    for verse_range in ranges:
        match verse_range.split("-"):
            case [verse] if 0 < int(verse) <= 300:
                verses.append(int(verse))
            case [start, end] if all(0 < int(x) <= 300 for x in (start, end)):
                verses.extend(range(int(start), int(end) + 1))

    if not verses:
        raise InvalidVerseSpec()

    return sorted(set(verses))


def build_verses_spec(verses: Iterable[int]) -> str:
    """Converts Verse Set to a Normalized Verse Spec"""
    verses = sorted(verses)
    ranges: list[str] = []

    cur = 0
    while cur < len(verses):
        begin = cur
        cur += 1
        while cur < len(verses) and verses[cur - 1] + 1 == verses[cur]:
            cur += 1

        end = cur - 1

        if begin == end:
            ranges.append(f"{verses[begin]}")
        else:
            ranges.append(f"{verses[begin]}-{verses[end]}")

    return ",".join(ranges)
