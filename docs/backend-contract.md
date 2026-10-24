# MeeBible backend contract

Last updated: 2026-10-24.

This is the source-based contract for the Python migration. It describes what the current PHP code does, what the Qt client expects, and which errors we intend to fix. It is not a record of captured HTTP responses. The old server has not been started and production has not been queried for this step.

See the [migration plan](python-migration-plan.md) for scope and progress and the [backend guide](python-backend.md) for setup and operation.

## Sources and confidence

The review covers the PHP routes and handlers, response classes, website templates, nginx configuration, Qt requests and parsers, and a read-only check of local SQLite metadata and sample chapters. The form encoding review also uses the upstream Qt 4.7.4, Qt 4.8.0, and PHP 5.6.40 source code linked below.

| Source | What it defines |
| --- | --- |
| [`www/urls.php`](../www/urls.php), [`www/core/Framework.php`](../www/core/Framework.php), [`www/route.php`](../www/route.php) | Route matching and request dispatch |
| [`www/core/Response.php`](../www/core/Response.php) | Content types, lengths, XML, templates, and redirects |
| [`www/views/`](../www/views/), [`www/loadchapter.inc.php`](../www/loadchapter.inc.php) | Parameters, queries, responses, errors, and writes |
| [`www/templates/`](../www/templates/), [`www/static/`](../www/static/) | Public pages, links, forms, and files |
| [`nginx.conf`](../nginx.conf), [`proxy.conf`](../proxy.conf), [`docker-compose.yml`](../docker-compose.yml) | FastCGI, static files, compression, and proxy settings in the repository |
| [`src/Paths.cpp`](../src/Paths.cpp), [`src/Settings.cpp`](../src/Settings.cpp) | Service URL and client defaults |
| [`src/Languages.cpp`](../src/Languages.cpp), [`src/MetaInfoParser.cpp`](../src/MetaInfoParser.cpp) | Metadata requests, parsing, and caching |
| [`src/Translation.cpp`](../src/Translation.cpp), [`src/TranslationInfoParser.cpp`](../src/TranslationInfoParser.cpp) | Book/chapter metadata and chapter requests |
| [`src/ChapterRequest.cpp`](../src/ChapterRequest.cpp), [`src/BibleView.cpp`](../src/BibleView.cpp), [`share/script.js`](../share/script.js), [`share/style.css`](../share/style.css) | UTF-8 chapter decoding, display, verse markup, and local resources |
| [`src/Fetcher2.cpp`](../src/Fetcher2.cpp) | Whole-translation downloads |
| [`src/StartupTracker.cpp`](../src/StartupTracker.cpp), [`src/NetworkUtils.cpp`](../src/NetworkUtils.cpp), [`src/Utils.h`](../src/Utils.h) | Startup form fields and values |
| [`src/Feedback.cpp`](../src/Feedback.cpp), [`qml/FeedbackPage.qml.in`](../qml/FeedbackPage.qml.in) | Feedback form and success handling |
| [`src/Place.cpp`](../src/Place.cpp), [`src/Place.h`](../src/Place.h) | Shared verse URLs |

Statements about PHP code below are source findings. Actual PHP defaults, emitted headers, body bytes, database error behavior, and production proxy settings still need runtime checks. Examples show structure; they are not saved reference responses.

### Confirmed environment

- The owner confirmed PHP **5.6.40** from the official **`php:5.6-fpm`** Docker image. Use this version for the local reference server. Record the resolved image digest and architecture when setting it up; the tag alone does not identify the exact image.
- No local Qt runtime is available. Installing a current Qt version would not establish the behavior of the old clients. Use versioned historical source code for Qt-specific questions and save source-based fixtures in the test suite later.
- The exact Qt patch version used by each shipped client is not recorded here. The form encoding paths in upstream 4.7.4 and 4.8.0 were reviewed and agree. This is source evidence, not a test on a phone or proof about every vendor patch.
- `http://meebible.org/` is available for safe read-only observations as well as the later comparison with local reference responses. Keep production responses separate from local reference captures. Do not use public POST requests or malformed inputs to investigate form handling.

## Route map

Methods in this table are the methods used by the client or browser. Old PHP does not enforce methods; see the next section.

| Request | Input | Normal result | Data or side effect |
| --- | --- | --- | --- |
| `GET /meta` | None | 200, XML catalog | Reads `langs.sqlite` and `trans.sqlite` |
| `GET /translation?trans=...&lang=...` | Translation and language codes | 200, XML book/chapter list | Reads `trans.sqlite` |
| `GET /chapter?trans=...&lang=...&book=...&chapter=...` | Codes and chapter number | 200, stored HTML | Reads a translation database |
| `POST /startup` | URL-encoded form | 200, text `ok` | Inserts a startup record |
| `POST /feedback` | URL-encoded form | 200, text `ok` | Inserts a feedback record |
| `GET /t/{trans}/{lang}/{book}/{chapter}/{verses}` | Passage in path | 200 HTML, 301 for a non-canonical verse list, or an error | Reads chapter HTML and book name |
| `GET /` | None | 200, HTML home page | Reads catalog for counts and translation list |
| `GET /privacy` | None | 200, HTML privacy policy | Template |
| `GET /donate` | None | 200, HTML donation page | Browser submits a PayPal form |
| `GET /donate-mobile` | None | 200, HTML donation page | Browser shows a PayPal form |
| `GET /static/{file}` | File path | Static file or nginx error | nginx reads `www/static/` |
| `GET /favicon.ico` | None | Icon or nginx error | nginx reads `www/static/img/favicon.ico` |
| Unknown URL | None | 404, HTML error page | Template for application routes |
| `/fb`, `/nwtbible?...` | Legacy functions | Remove: 404 in Python | Do not port their handlers |
| J2ME handlers | Unused code | Remove | No J2ME routes are registered in `urls.php` |

## Routing and HTTP details

### Current PHP routing

- nginx sends application requests to the fixed script `/www/route.php`. A URL such as `/chapter.php` is not direct execution of that file. `/static/` and the exact favicon path are handled separately by nginx.
- PHP matches the raw `REQUEST_URI`, including its query string. `/`, `/privacy`, `/donate`, `/donate-mobile`, `/meta`, `/startup`, `/feedback`, and `/fb` require exact matches.
- As a result, `/meta?x=1`, `/feedback?x=1`, and `/meta/` fall through to 404. There is no automatic slash redirect in the application.
- `/translation` and `/chapter` match only when the raw URI starts with `/translation?` or `/chapter?`. Bare paths return 404. Use a non-empty query when testing missing parameters; how a bare trailing `?` reaches PHP needs capture.
- `/t/` uses a prefix match. Its handler uses another regex without an end anchor, so some trailing text is ignored. This is an old parsing bug, not required behavior.
- Dispatch does not check the HTTP method. A request that reaches `/startup` or `/feedback` can run a write even without POST fields. Do not test this on production.
- Translation and chapter handlers use `$_REQUEST`; startup and feedback use `$_POST`. The merge of query, form, and possibly cookie values depends on PHP configuration. Duplicate keys, array syntax, and collisions need local reference tests.
- Canonical client requests have one scalar value per field. They use query parameters for reads and form bodies for writes. They do not need cookie authentication, sessions, or API keys.

### Responses and proxy behavior

`Response::render()` sets Content-Type and Content-Length, then prints the body. Length is computed with PHP `strlen`, in bytes. Success status is normally 200; handlers set other statuses explicitly. Error strings listed below have no newline in the body passed to Response.

| Response kind | Content-Type set by application | Body |
| --- | --- | --- |
| Metadata XML | `application/xml` | `DOMDocument("1.0")->saveXML()` |
| Chapter or template | `text/html` | Stored chapter string or rendered template |
| Plain error or POST success | `text/plain` | Fixed message |
| Passage redirect | `text/plain` | `HTTP/1.1 301 Moved Permanently\n{location}` |

PHP can add a default charset to text types. Exact XML declaration, escaping, whitespace, charset headers, warnings, and wire bytes must be captured on PHP 5.6. Do not assume that the source Content-Type string is the full wire header.

The application does not set cache headers, ETags, cookies, or CORS headers. Static responses and infrastructure may add their own headers. The repository enables gzip, `gzip_vary`, and `gzip_static`, with minimum length 1000, level 5, and XML/text/CSS/JavaScript types. Text/html is also subject to nginx gzip handling. The separate proxy file contains similar settings and a 300-second read timeout; its use in production is not confirmed. MIME mappings and other nginx/PHP defaults are outside these files.

Keep plain HTTP at the existing URLs. Do not require TLS, authentication, new request headers, redirects, or a new client version. Keep canonical URLs working without slash redirects. Verify HEAD, gzip negotiation, content length, and static conditional/range requests on the reference stack. Header order, Server version, Date, and compressed bytes do not need exact equality. Decoded API content and meaningful status/header behavior do.

## Catalog: `/meta`

Request: `GET /meta`. No parameters.

The response has this structure, with no namespaces:

```xml
<metainfo>
  <languages>
    <language code="e" name="English" selfname="English"/>
  </languages>
  <translations>
    <trans code="kjv" sourceUrl="..." copyright="..." rtl="0">
      <transLang code="e" name="King James Version (1769)"/>
    </trans>
  </translations>
</metainfo>
```

- Languages come from `langs`, sorted by `engname` using SQLite's default ordering. Fields map to `code`, `engname` as `name`, and `selfname`.
- Translations come from `translations` with no ORDER BY. Preserve observed order in the reference snapshot; do not introduce alphabetical sorting by accident.
- Each translation contains its `translationLangs` rows, also without explicit sorting. Attribute `code` is the translation code on `trans` and the language code on `transLang`.
- Keep source URLs, copyright, native names, empty values, and `rtl`. Do not expose the internal `parser` or top-level translation `name` columns as new attributes.
- All languages are emitted, including ones without translations.

The client decodes the body with `QString::fromUtf8`, then uses `QXmlSimpleReader`. It compares element names and reads attributes by name. Attribute order is not significant. Language records must appear before translation-language records: the parser immediately looks up the language and does not check for a missing result. Each `transLang` must stay inside the correct `trans`. The client treats only `rtl="1"` as true.

The client caches the received XML as `meta`. It checks the network error but does not check the XML parser result before saving. A 200 response containing an error page can therefore poison the cache. Unexpected internal failures must use an error status.

## Translation: `/translation?trans=...&lang=...`

Canonical example: `GET /translation?trans=kjv&lang=e`.

| Field | Source behavior |
| --- | --- |
| `trans` | Required by PHP `empty()` check; then used as a bound query value |
| `lang` | Required by PHP `empty()` check; then used as a bound query value |

PHP treats both an empty string and the string `0` as empty. There is no code-format validation here. Extra query fields are unused.

```xml
<translation>
  <book code="ge" name="Genesis">
    <chapter no="1" verses="31"/>
    <!-- more chapters -->
  </book>
  <!-- more books -->
</translation>
```

Books are selected from `books` for `(transCode, langCode)`, ordered by `bookNo`. Chapters come from `chapterSize` for `(transCode, bookCode)`, ordered by `chapterNo`. This table has no language column. Preserve the data model; do not derive new chapter sizes from HTML during the port.

| Condition after route matching | Status | Response |
| --- | --- | --- |
| Either field is missing, empty, or `0` | 200 | XML root `error` with text `trans and lang parameters are required` |
| No books match the translation/language pair | 200 | Empty `translation` root |
| Matching pair | 200 | XML book/chapter list |
| Database/query failure | Not reliably handled | Capture locally; use a controlled server error in Python |

The client decodes UTF-8 and caches the XML as `trans_{trans}_{lang}`. It preserves book order. For every chapter element it appends the `verses` value to a list and **ignores the `no` attribute**. It later indexes that list by chapter number minus one. Chapters must be in order, starting at 1 without gaps. Do not sort by strings (`1, 10, 11, 2`) or omit elements with a zero verse count.

## Chapter: `/chapter?trans=...&lang=...&book=...&chapter=...`

Canonical example: `GET /chapter?trans=kjv&lang=e&book=ge&chapter=1`.

After checking that all four fields are non-empty, PHP applies these regexes:

| Field | Regex in PHP | Meaning for normal client values |
| --- | --- | --- |
| `trans` | `/^[a-z0-9_]+$/` | Lowercase letters, digits, underscore |
| `lang` | `/^[a-z]+$/` | Lowercase letters |
| `book` | `/^[0-9a-z]+$/` | Lowercase letters and digits, including `1co` |
| `chapter` | `/^[0-9]+$/` | Decimal digits; bound to SQLite as an integer |

The string `0` fails the earlier empty check. Leading zeroes pass the regex and are converted for the database lookup. Negative numbers and signs fail validation. PHP regex edge cases and very large integers need local tests; Python must not depend on loose conversions.

Database paths:

- Other translations: `www/content/{trans}.sqlite`.
- `nwt`: `www/content/nwt/nwt_{lang}.sqlite`; keep this layout and all advertised languages.

Both use `SELECT html FROM html WHERE langCode=:langCode AND bookCode=:bookCode AND chapterNo=:chapterNo`. The normal response is the stored `html` string, with no parsing, templates, XSLT, or whitespace changes.

| Condition after route matching | Status | Content-Type set by application | Body |
| --- | --- | --- | --- |
| Any field missing, empty, or `0` | 400 | `text/plain` | `Not all parameters specified` |
| Scalar value fails a regex | 400 | `text/plain` | `Invalid parameters` |
| No matching row, empty content, or missing `nwt` language file | 400 | `text/plain` | `Chapter does not exists` |
| Matching non-empty content | 200 | `text/html` | Stored chapter HTML |
| Missing non-`nwt` file, broken schema, or access failure | Not reliably handled | May include diagnostics | Capture locally; do not reproduce unsafe file creation or crashes |

Keep the misspelled normal error message until an explicit contract change is recorded. The old SQLite constructor can create a missing non-`nwt` file. The new service must use known content paths and read-only connections.

The client decodes UTF-8, displays the HTML in Qt WebKit, and stores it for offline reading and search. Its own CSS and JavaScript work with `class="verse"`, the `verse` attribute, `verse-label`, paragraph structure, and body attributes such as `layout` and `rtl`. Keep chapter response bodies byte for byte. Do not sanitize or serialize all stored chapters during this migration.

Whole-translation download uses the same endpoint, with up to **20 concurrent outstanding chapter requests** in client code. It enumerates chapter numbers from the metadata count. Non-network-error responses are saved without content validation; network errors are logged and the request is not requeued in this download run. Limits must allow a normal full download. No separate bulk download or server-side search API is needed.

## Startup and feedback writes

Both clients build a `QUrl`, call `addQueryItem`, and POST `encodedQuery()` with `Content-Type: application/x-www-form-urlencoded`, without an explicit charset parameter. They use URL query encoding rather than HTML form encoding.

### Historical Qt encoding

The reviewed upstream code follows this path:

- `addQueryItem` converts key and value to UTF-8 and percent-encodes them, forcing the `&` and `=` delimiters to be encoded inside values.
- The allowed query characters include `+`, so a literal plus is left unchanged. A space is encoded as `%20`.
- `encodedQuery()` returns the saved query bytes without another encoding pass. The QByteArray overload of `QNetworkAccessManager::post` places these bytes in a buffer; it does not form-encode the fields again.

Sources: [Qt 4.7.4 addQueryItem](https://github.com/qt/qt/blob/v4.7.4/src/corelib/io/qurl.cpp#L5058), [query character set](https://github.com/qt/qt/blob/v4.7.4/src/corelib/io/qurl.cpp#L3445), [UTF-8 conversion](https://github.com/qt/qt/blob/v4.7.4/src/corelib/io/qurl.cpp#L206), [percent encoder](https://github.com/qt/qt/blob/v4.7.4/src/corelib/tools/qbytearray.cpp#L4098), [encodedQuery](https://github.com/qt/qt/blob/v4.7.4/src/corelib/io/qurl.cpp#L5429), [POST buffer](https://github.com/qt/qt/blob/v4.7.4/src/network/access/qnetworkaccessmanager.cpp#L718). The corresponding [Qt 4.8.0 addQueryItem](https://github.com/qt/qt/blob/v4.8.0/src/corelib/io/qurl.cpp#L5121) and [encodedQuery](https://github.com/qt/qt/blob/v4.8.0/src/corelib/io/qurl.cpp#L5493) behave the same; its percent-encoding helper also matches 4.7.4.

Source-based examples for a value passed to `addQueryItem`:

| Input | Bytes inside the encoded value |
| --- | --- |
| Space | `%20` |
| `+` | `+` |
| `&` | `%26` |
| `=` | `%3D` |
| `%` | `%25` |
| Literal text `%2B` | `%252B` |
| CR followed by LF | `%0D%0A` |
| `é` | `%C3%A9` |

For example, the value `user+tag@example.org` produces `email=user+tag@example.org`. PHP 5.6.40's [POST parser](https://github.com/php/php-src/blob/php-5.6.40/main/php_variables.c#L280) calls [php_url_decode](https://github.com/php/php-src/blob/php-5.6.40/ext/standard/url.c#L534), which converts a raw `+` to a space. The source path therefore predicts that this address is stored as `user tag@example.org`. A percent-encoded `%2B`, if sent by another client, decodes to a literal plus instead.

This is an existing data-loss bug. A standard Python form parser would keep that bug. Preserving raw plus signs on the two legacy POST endpoints is a possible fix, but it changes how ordinary HTML forms using `+` for spaces are read. There is no reliable way to infer both meanings from the same body. Decide the Python rule explicitly, then test raw Qt-style bytes and the old PHP result locally. Do not use a heuristic based on whether a body contains `%20`.

### `/startup`

| Field | Client behavior |
| --- | --- |
| `os` | Sent as `meego`, `symbian`, or fallback `unknown` |
| `device_id` | Sent from network interface hardware addresses; may be empty; multiple addresses may be joined with `_` |
| `lang` | Sent only if a language is selected |
| `trans` | Sent only if a translation is selected |

The PHP handler reads all four keys without checking them, logs their values, creates `startup` if needed, and inserts one row. Missing `lang` or `trans` is a valid client situation despite the old handler's possible notices. Do not make them required in Python. The handler does not validate codes against the catalog.

```sql
CREATE TABLE startup (
    id INTEGER PRIMARY KEY,
    time DATETIME,
    device_id VARCHAR,
    os VARCHAR,
    lang VARCHAR,
    trans VARCHAR
);
```

Insert values are `datetime('now')`, device ID, OS, language, and translation. The client sends the request without checking its response.

### `/feedback`

| Field | Client behavior |
| --- | --- |
| `name` | User text; can be empty |
| `email` | User text; can be empty after a confirmation dialog; no email format check |
| `message` | User text; no non-empty check in the QML form |

The handler logs the sender name, creates `feedback` if needed, and inserts one row. Sending email is commented out: the endpoint does not send mail.

```sql
CREATE TABLE feedback (
    id INTEGER PRIMARY KEY,
    time DATETIME,
    name VARCHAR,
    email VARCHAR,
    message VARCHAR
);
```

Insert values are `datetime('now')`, name, email, and message. The client reports success only if there is no network error and the HTTP status is exactly 200. It ignores the response body. Keep acceptance of empty fields and ordinary text; do not introduce strict email validation.

### Shared write behavior

- Normal result: 200, `text/plain`, body `ok` (two bytes in the Response object).
- Files are currently `www/startup.sqlite` and `www/feedback.sqlite`, relative to the PHP working directory. Python will move writable data to a separate configured directory while retaining the schemas and records.
- IDs are assigned by SQLite. `datetime('now')` uses UTC with second precision. Match the stored timestamp format; IDs and times are variable values in tests.
- Each request inserts a new row. There is no deduplication or idempotency key. Do not add one by guessing from message contents or device ID.
- Old handlers do not inspect write results before returning `ok`. Python must commit before success and return an error on failure. Do not silently drop a message or startup record.
- Missing fields, duplicate fields, arrays, multipart bodies, invalid encoding, and size limits need explicit local tests and documented Python rules. The exact stored value for a missing PHP key needs reference capture; it must not be guessed as SQL NULL or an empty string. Form fixtures must use the historical Qt encoding above; a normal form encoder would incorrectly turn literal plus signs into `%2B`.
- SQL uses bound parameters. Preserve text, special characters, and line breaks. Test that values cannot change the query.

## Shared passages: `/t/{trans}/{lang}/{book}/{chapter}/{verses}`

The client generates absolute plain HTTP links such as `http://meebible.org/t/kjv/e/ge/1/1-3,5`. It sorts selected verse numbers, joins consecutive numbers with ASCII `-`, and joins ranges with commas. The displayed title uses an en dash instead of a hyphen.

The current handler matches this prefix (there is no end anchor):

```text
^/t/([a-zA-Z0-9_-]+)/([a-zA-Z0-9_-]+)/([a-z]+)/(\d+)/([0-9,-]+)
```

This differs from `/chapter` validation. In particular, it rejects digit-prefixed book codes such as `1co`, even though the client generates them. Fix this in Python.

Current verse processing:

1. Split by commas. Each entry is a single integer or an inclusive range. Only numbers from 1 to 300 are kept; invalid entries and reversed ranges add no verses.
2. Remove duplicates and sort numerically.
3. Join consecutive numbers as ranges. For example, `3,1,2,5,5` becomes `1-3,5`.
4. If no numbers remain, return 400 `text/plain` with `Strange verse specification`.
5. If the captured verse string differs from its normalized form, return 301 with Location `/t/{trans}/{lang}/{book}/{chapter}/{normalized}` and the plain redirect body described above.

A path that fails the regex after matching `/t/` returns 400 `text/plain` with `Not all parameters specified`. A bare `/t` falls through to the normal 404 page.

For a canonical request, the handler loads chapter HTML and parses it with DOMDocument's HTML parser. For each selected verse, it copies every node with the matching `verse` attribute, keeping its immediate parent wrapper. If verse 1 is selected, it also copies all nodes with class exactly `superscript`. It adds a `span` with class `hole` and text `[...]` between non-consecutive requested verse numbers. It preserves multiple HTML pieces for one verse.

The result is wrapped in a `div` and placed in `place.tpl`. The title and heading use the localized book name from `books`, chapter number, and normalized verse list. Open Graph metadata contains the passage title and extracted text; verse labels are excluded from the direct text-node selection used for that text. The template includes the existing styles, home links, and social widgets. Metadata is escaped explicitly, but the heading is not; Python must escape ordinary text.

Current weaknesses to fix: loose path matching, digit-prefixed books rejected, missing chapter content not checked before parsing, and verse bounds checked against a fixed 300 rather than whether the requested content exists. A missing verse may currently yield an empty or partial page with 200. Define and test controlled missing-content responses during implementation. Keep working canonical links and normal range redirects. HTML parser whitespace and entity output need fixture comparison; passage pages do not have the byte-for-byte requirement of chapter downloads.

## Website and static files

| Page | Current behavior to preserve |
| --- | --- |
| `/` | Shows app information, download/source/support/donation/privacy links, screenshots, and a catalog list. Languages are sorted by `selfname`; languages without translations are removed. The translation count is the number of `translationLangs` rows, not distinct translation codes. Translation names retain query order. |
| `/privacy` | Renders the existing policy. The client opens this URL from the feedback page. Policy changes are outside this port. |
| `/donate` | Returns HTML with a PayPal form and JavaScript that submits it on page load; this is not an HTTP 301/302 redirect. |
| `/donate-mobile` | Returns HTML with a PayPal form/button and a link to `/donate`; no automatic submit. |
| Unknown application path | Returns status 404 with the existing HTML error template. |

Shared templates declare UTF-8 in HTML and link to existing CSS and favicon. Preserve page content, useful links, and appearance without requiring identical template whitespace. Escape ordinary text in the new template engine; only the passage HTML built from stored content is inserted as markup.

Static files include CSS, JavaScript, images, and the existing `.sis` and `.deb` packages. Preserve URLs and file bytes. MIME types, HEAD, cache validators, range handling, and nginx-generated 404 bodies need runtime capture. `/static/` must not expose code, databases, or writable user data.

External scripts, social widgets, download destinations, and payment services are outside the backend protocol. Tests can check the page's links and form fields without executing scripts, submitting payments, or contacting those services. The old home page loads an unpinned external jQuery script over HTTP; review replacing it with a local resource or small local script as a website safety fix, without redesigning the page.

## Local data observations

A read-only metadata check on 2026-10-23 found:

| Item | Count |
| --- | --- |
| Languages in `langs.sqlite` | 57 |
| Translation records | 51 |
| Translation/language pairs | 94 |
| Languages with at least one translation | 54 |
| Book rows across translation/language pairs | 6052 |
| Chapter size rows, without language dimension | 58378 |
| Languages advertised for `nwt` | 44 |

Every advertised pair has its expected content file, and every pair's language exists in the language catalog. Chapter size groups start at 1 and have no gaps; book order numbers have no duplicates within a translation/language pair. These checks do not prove that every chapter row exists in the content files; that is part of the full content test later.

Local samples include `kjv/e`, `rus/u`, `ara/a`, and `nwt/e`; `ara` and `heb` are marked RTL. Book codes include both `ge` and `1co`. Sample chapters contain complete HTML documents, with different leading whitespace and body attributes. Do not use README counts as a test oracle or silently change catalog data to match them. These counts describe this checkout, not production, and are not permanent API constants.

## Intentional changes and remaining decisions

The changes below are part of the port, not changes to existing PHP in this step.

| Area | Python direction | Detail still to settle |
| --- | --- | --- |
| Removed features | 404 for `/fb`, `/nwtbible`, and unused J2ME URLs; keep `nwt` content | Add regression cases |
| Methods | Reads use GET/HEAD; startup/feedback use POST only; unsupported methods must not write | Check old HEAD behavior and define error headers/bodies |
| File access | Known content paths, read-only SQLite, separate writable data | Exact configuration paths |
| Invalid input | Controlled errors; no arrays used as scalars, loose numeric conversion, or accidental FastAPI 422 JSON | Duplicate keys, collisions, size limits, and exact errors after local capture |
| Normal legacy errors | Keep documented XML/text responses and statuses | Confirm actual wire headers and bytes |
| Write failures | No success before commit | Choose controlled 5xx body and test disk/database failures |
| Optional startup fields and empty feedback | Accept legitimate client input | Verify storage of omitted fields; historical Qt encoding is now documented |
| Literal plus signs in POST forms | Fix the known Qt/PHP data-loss problem deliberately | Decide raw-plus handling for the legacy endpoints and test the difference from standard HTML forms |
| Passage links | Accept digit-prefixed book codes; validate the full path and handle missing content | Exact 400/404 policy, missing/partial verses, suffixes, and trailing slashes |
| HTML output | Escape plain text; keep chapter HTML intact | Compare passage parser output and review external HTTP script |
| Routing cleanup | Preserve canonical client URLs; no automatic redirects | Whether to accept extra query strings on exact routes or bare metadata paths; record any change explicitly |

## Reference and test checklist

This list defines test coverage for later steps. No HTTP test run has been performed yet.

| Group | Required checks |
| --- | --- |
| Catalog | Complete languages/translations, all attributes, ordering, empty values, Unicode, RTL, valid language references |
| Translation | All advertised pairs, book order, chapter order/count/verse count, missing and empty fields, `0`, unknown pairs |
| Chapters | Full advertised content set locally, `nwt` layout, first/last chapters, digit-prefixed books, exact decoded bytes, unchanged file checksums |
| Chapter errors | Missing/empty fields, `0`, invalid codes, unknown codes, missing rows/files, leading zeroes, huge integers, array parameters, no new files |
| Qt forms | Exact encoded bodies, Unicode, spaces, literal `+`, `%`, `&`, `=`, apostrophes, CR/LF, empty feedback, omitted startup language/translation, repeated requests |
| Writes | One committed row per success, unchanged schema, timestamp format, database failure, no mail, no public test writes |
| Passages | Single verse, ranges, gaps, duplicates, normalization redirect, superscripts, split verses, Unicode/RTL, digit-prefixed books, invalid paths and missing content |
| Pages/static | All retained pages and local assets, download file bytes, links/form fields, 404, HEAD, MIME, validators/ranges |
| Routing | Query versus bare path, slash variants, extra fields, duplicate/query-body values, unknown paths, methods, removed endpoints |
| Transport | Plain HTTP, original status and Location, Content-Type/charset, byte length, identity/gzip, changing proxy headers kept as evidence |
| Client flow | Catalog → translation → chapter → full download with up to 20 outstanding requests; no cached 200 error pages |

## Questions for the reference server and production comparison

1. PHP 5.6.40 from official `php:5.6-fpm` is confirmed. Which image digest, architecture, extensions, working directory, charset, error display, and request-order settings reproduce the deployed app? SQLite3 and DOM are required. Old Smarty is loaded for every application route.
2. What are the exact headers and body bytes for success, XML errors, chapter errors, passage redirects, and 404? Does any warning or extra output affect the body or length?
3. Historical Qt 4.7.4/4.8.0 source establishes the encoding above without a local Qt installation. Build fixtures from those rules, verify PHP decoding locally, and decide whether Python preserves raw plus signs on legacy POST endpoints. The exact shipped Qt patch versions remain unknown.
4. How are omitted fields, duplicate fields, arrays, query/form collisions, HEAD, and unsupported methods handled by PHP 5.6 and the proxy? Run write and invalid-input cases only on temporary local data.
5. Do all catalog entries and chapter sizes agree with content rows? Check the whole local data set before release and report existing data gaps separately from port failures.
6. How does PHP's HTML parser serialize passages, especially entities, Cyrillic, RTL, superscripts, and split verses? Does the template add any relevant behavior beyond the source findings?
7. Which repository nginx/proxy settings are active on `http://meebible.org/`? Safe read-only observations can help answer this. Once local responses exist, compare a small safe GET sample, with and without gzip and without following redirects automatically. Classify differences in headers, content, and configuration before changing the contract.

Store reference environment details and captures with the test tooling when it is built. Update this document when an open point is resolved; keep significant decisions in the migration log.
