from urllib.parse import unquote_to_bytes


def parse_qt_form(body: bytes) -> dict[str, str]:
    fields = {}

    for pair in body.split(b"&"):
        if not pair:
            continue

        name, separator, value = pair.partition(b"=")
        name = unquote_to_bytes(name).decode("utf-8")
        value = unquote_to_bytes(value).decode("utf-8")

        if not separator or not name or name in fields or "[" in name or "]" in name:
            raise ValueError("Invalid form")

        fields[name] = value

    return fields
