# External backend tests

The tests send real HTTP requests. They do not import application code or require a particular Python framework. Python 3.11 or later is enough; there are no packages to install. The server must run separately.

Use these tests while writing the Python backend. The default `python` profile keeps the client contract and checks the agreed fixes. The `php` profile checks the old local server against saved observations, including its known bugs.

## Run against your implementation

Start your server with the fixed Bible content and a **temporary** directory for `startup.sqlite` and `feedback.sqlite`. Set `USER_DATA_DIR` to that directory as seen by the test process. If the server runs in Docker, bind this temporary directory to its user-data path. The tests read SQLite in read-only mode to confirm writes; they do not add a test endpoint or import the server.

From the repository root:

```sh
BASE_URL=http://127.0.0.1:8000 USER_DATA_DIR=/tmp/meebible-contract-data python3 tests/backend/run.py quick
BASE_URL=http://127.0.0.1:8000 USER_DATA_DIR=/tmp/meebible-contract-data python3 tests/backend/run.py full
```

Create the temporary directory before starting your service. The service creates the original tables on its first write, or can initialize them at startup. Existing rows are allowed and must stay unchanged. Each successful form submission must commit a new row. Repeated runs add synthetic test rows; reset your own test data when needed. The runner never deletes databases or resets a running server.

`BASE_URL` can also be passed as `--base-url`. It must be a plain HTTP loopback origin. Public hosts, HTTPS, credentials, URL prefixes, and query strings are rejected before requests. Production comparison has a separate procedure in migration step 5.

Reports are saved in `tests/backend/.local/quick-python.json` or `full-python.json`. They list failing case names and messages. A nonzero exit code means a failure or invalid setup. Look up a named case in [`cases.py`](../tests/backend/cases.py); the request and any intentional Python rule are together. The runner saves original response headers and body hashes in a compressed local response log. It never updates references when a test fails.

For early work, `--read-only` runs a **partial** suite without write checks. It is useful before startup/feedback exists, but cannot prove the complete contract. Full mode still checks every chapter. Use `--workers 1` to investigate concurrency problems; the normal full run uses 20 outstanding chapter requests, as the mobile client can do.

## What the modes check

| Mode | Coverage |
| --- | --- |
| `quick` | Catalog; representative translation XML; Unicode, RTL, `nwt`, first/last and digit-prefixed chapters; normal and malformed requests; passage extraction, gaps, split verses, superscripts, redirects; public pages; static files, ranges, HEAD and validators; Qt forms; committed rows and unchanged schemas |
| `full` | Everything in quick, all 94 translation/language XML responses, all 109,505 advertised chapter bodies, and all static assets including `.sis` and `.deb` packages |

Counts describe the fixed reference snapshot, not permanent API constants. Full mode obtains paths from the captured catalog and translation flow and uses at most 20 concurrent requests. It compares each chapter's byte count and SHA-256 against the original SQLite text. No chapter is reformatted. Full runs can take many minutes under amd64 emulation.

XML comparison checks element names, values, all attributes, and child order. It allows different attribute order and formatting whitespace. This keeps the chapter order required by the client. Pages compare visible content, titles, links, image URLs, and payment form fields without executing scripts or contacting external services. Passage checks also compare ordered verse pieces, superscripts, gap markers, and Open Graph text. HTML whitespace and serializer spelling can differ. Chapter bodies and static assets remain byte-exact.

HTTP checks keep original statuses and Location values and do not follow redirects. They check media types, UTF-8 for existing text responses, Content-Length against the actual entity, identity/gzip, Vary, HEAD, ranges, and static validators. Date, Server, header order, and compressed byte spelling are not equality requirements. The HTTP library removes chunk framing; saved wire bodies are the entity bytes before gzip decoding, not TCP packet captures.

## Python rules that differ from PHP

These are test expectations, not an implementation. Keep the code of the Python backend your own.

- Preserve raw `+` on `/startup` and `/feedback`, as agreed on 2026-10-24. A Qt space is `%20`; `%2B` is also a plus and `%252B` is literal `%2B`. This deliberately changes standard HTML forms that use `+` for spaces. Do not guess a rule from the presence of `%20`.
- Accept omitted startup `lang`/`trans` and store SQL NULL, as PHP does. Accept empty device IDs and empty feedback fields. Keep Unicode, apostrophes, delimiters, percent signs, CR/LF, and repeated submissions.
- Reject missing required form fields, array-valued fields, duplicate scalar keys, and invalid UTF-8 with a controlled 400 and no new row. The required fields are `os`/`device_id` for startup and `name`/`email`/`message` for feedback. Required fields may be empty strings; `lang`/`trans` remain optional.
- Read routes use GET/HEAD. Startup/feedback use POST only. Unsupported tested methods return 405 with an appropriate Allow header and cannot write user data. Custom text/HTML errors are needed; automatic framework JSON errors do not satisfy these tests.
- Accept digit-prefixed book codes in passage links. Reject trailing path garbage/slashes and missing or partly missing passage content with a controlled 400 or 404. The exact choice between those two statuses remains open where the contract has not fixed it.
- Unknown content files and array parameters must produce a controlled 400/404, without PHP-style diagnostics. Normal legacy errors keep their documented text and status. Unknown application paths keep the existing HTML 404. Removed `/fb`, `/nwtbible`, and J2ME routes return 404.
- Keep canonical raw URL matching for now: for example `/meta?x=1` and `/meta/` still return 404. There is no automatic slash or HTTPS redirect.

The PHP profile records several unsafe cases as 200 with diagnostics, empty XML, converted strings, or unexpected writes. Those observations are evidence, not Python requirements. It never fetches `/fb`. The Python profile checks `/fb` only as a removed route on the new service.

## Check the old local server

Prepare and start the isolated server using [the PHP guide](php-reference.md). Then:

```sh
BASE_URL=http://127.0.0.1:10056 python3 tests/backend/run.py quick --profile php --php-reference
BASE_URL=http://127.0.0.1:10056 python3 tests/backend/run.py full --profile php --php-reference
docker compose -f tools/php-reference/compose.yml down
```

`--php-reference` reads only that server's tmpfs databases through `docker compose exec`. It is restricted to the PHP profile and local port 10056. The `python` profile normally uses a host directory instead. Use a distinct port for your implementation, so the test profile and data observer cannot get mixed up.

For a faster full check under emulation, the tested local container can temporarily use both CPUs available in this Docker VM:

```sh
docker update --cpus 2 meebible-php56-reference-php-1
```

The normal Compose limit is one CPU. Restore it with `docker update --cpus 1 meebible-php56-reference-php-1`, or remove the containers with `down` and start again. This changes only a resource limit; the image, PHP settings, content, mounts, and network stay the same. The capture manifest records the temporary increase used during verification.

To run the test tools' own checks without a server:

```sh
python3 -m unittest discover -s tests/backend -p test_harness.py -v
```

These checks prove that the tools reject public targets, changed chapter bytes, XML order changes, and automatic JSON errors, and preserve the form/HTML/database evidence needed by the suite.

## Reference capture and review

The committed references are in [`tests/backend/reference/`](../tests/backend/reference/manifest.json). The manifest records the source revision, pinned images, PHP settings, configuration hashes, original status/headers, raw ASCII form bodies, synthetic write results, and file hashes. Decoded sample responses and gzip entities are saved in `bodies/`. All translation XML is included. `chapters.json.gz` stores the complete chapter byte/hash index from the fixed SQLite snapshot; the bodies already exist in that snapshot and are not duplicated in Git. Full mode checks those expected bytes over HTTP.

Capture is an explicit command that only targets the verified PHP server:

```sh
python3 tests/backend/capture.py --output tests/backend/.local/reference-new
```

The command first verifies the old server's isolation and snapshot. The output directory must not exist. It will not overwrite a reference or read production. It makes local POST and malformed requests and records only synthetic user values. Run it with fresh tmpfs databases when reviewing a new capture.

Review differences in status, headers, body bytes/XML values, and stored form fields before replacing anything. Compare content/image/configuration hashes first. Different Date values, synthetic timestamps, and compressed bytes can be expected; changes to decoded chapter bytes need explanation. The normal test runner has no capture/update option.

The full test suite is available for implementation work, but this is not release approval. Production comparison, failure injection for database/disk errors, final filesystem permissions, final nginx limits, and the full checks on the final Python image still belong to later migration steps. These protocol checks do not prove rendering on a real phone.
