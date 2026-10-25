# MeeBible Python migration plan

Last updated: 2026-10-25.

The goal is to keep existing MeeBible clients on Symbian and MeeGo working by replacing PHP 5.6 with a small Python + FastAPI service. New features, new users, and a redesign are out of scope. We will not migrate to PHP 8.

This document tracks the plan, completion criteria, main results, and decisions. The source-based API and website contract is in [Backend contract](backend-contract.md). Keep the description of the built service and operating instructions in the separate [Python backend guide](python-backend.md). Write project documents in simple English.

## Work plan

Statuses: **Not started**, **In progress**, **Done**, **Blocked**. Mark a step as done only when its completion criteria are met. Add a short note about important results and changes to the log below. Update this plan when the scope changes.

| No. | Step | Completion criteria | Status |
| --- | --- | --- | --- |
| 1 | Agree on scope and workflow | Features to keep and remove are agreed; the plan and separate guide exist | Done |
| 2 | Describe the external contract from PHP, nginx, and client code | Routes, methods, parameters, responses, errors, and side effects are listed; HTTP, encodings, Qt forms, and client parsers are covered | Done |
| 3 | Set up an isolated PHP 5.6 reference server | The old service can be started in a repeatable way without public access; it uses a fixed content snapshot and temporary startup/feedback databases; commands and settings are documented | Done |
| 4 | Capture local reference responses and build external tests | Responses and environment details are saved; tests accept BASE_URL; routes to keep, errors, and POST writes are covered; quick and full test modes exist | Done |
| 5 | Compare reference responses with meebible.org | A small set of safe GET requests is compared at a low rate; differences in status, headers, and bodies are classified; configuration differences are separated from data and application differences; unexplained differences are recorded | Not started |
| 6 | Build the Python backend | All features to keep are implemented using the existing SQLite files; removed routes return 404; agreed bugs are fixed; quick contract tests pass | In progress |
| 7 | Check all content and the final environment | All translations, languages, books, and chapters are checked; POST, errors, permissions, nginx, gzip, and HTTP are tested; every difference from the reference is explained and accepted | Not started |
| 8 | Prepare the release and operating guide | Tested commands cover startup, tests, updates, backup, restore, and switching servers; dependency versions are pinned; project layout and limits are documented | Not started |
| 9 | Switch production to Python | User SQLite databases are copied consistently with old writes stopped; the upstream is switched; reads, writes, logs, and redirects are checked; old PHP no longer serves public requests | Not started |

Implementation can proceed in parts as contracts become clear. Run the full content checks and final comparison on the final image and configuration. This plan does not mean that production has already been changed.

The owner will write the Python service. The assistant is preparing the external tests and reference evidence. Keep test tooling independent of the service implementation.

## Agreed scope

| Part | Decision |
| --- | --- |
| `/meta`, `/translation`, `/chapter` | Keep the external contract and existing data |
| `/startup`, `/feedback` | Keep the POST contract and data writes; both are active in production |
| `/t/...` | Keep verse links, passage extraction, and verse range normalization |
| `/`, `/privacy`, `/donate`, `/donate-mobile`, static files, and favicon | Port without a redesign |
| `/fb` | Remove from the new version; a web interface for reading feedback is not needed |
| `/nwtbible` | Remove the old Maemo compatibility endpoint |
| J2ME | Remove unfinished and unused code |
| `nwt` translation | Keep it, including its special database layout; removing `/nwtbible` does not mean removing this translation |

Keep plain HTTP without a forced HTTPS redirect. Return chapter content without reformatting it. Keep the Bible data storage format. Keep the startup/feedback schemas at first to make it easier to move existing records.

Unsafe behavior in the old implementation is not a compatibility requirement. Controlled errors instead of crashes, prevention of arbitrary SQLite file creation, input limits, and correct escaping are allowed. Document and test specific behavior changes. FastAPI must not replace the old contract with automatic JSON errors or redirects by accident.

## Reference responses and production comparison

- Capture the main reference responses from the local old application with known code and a fixed data snapshot. Record the image version, configuration, source revision, and content checksums.
- Treat responses from meebible.org as extra evidence, not as a trusted source of code, data, or required behavior. Do not use them to overwrite local reference responses automatically.
- Compare production only after local reference responses are ready. Use a small, representative set of GET requests with a low request rate and limited concurrency. Check the full content set locally.
- Earlier safe read-only observations from `http://meebible.org/` can help answer configuration questions. Record them separately; they do not replace the later comparison or trusted local reference captures.
- Record original status codes and headers, including Content-Type/charset, Content-Length, Content-Encoding, Location, Cache-Control, and Vary. Check responses with and without gzip. Do not hide redirects by following them automatically. Compare decoded bodies; compressed bytes do not have to match.
- Keep Date and other changing headers for diagnosis, but do not require exact matches. Check Content-Length against the actual body representation and compression. The Server version is not part of the client contract.
- Classify differences by cause: content version, application version, internal nginx, external proxy, changing values, or unknown cause. Reproduce relevant configuration details in the test environment.
- Do not send test POST requests to production: they write startup/feedback records. Test them locally. Do not access `/fb` or download user databases over HTTP.
- Test invalid and potentially harmful requests on the isolated server, not on public PHP 5.6.

## Overall completion criteria

- External tests cover every route we keep, including important errors and side effects.
- The full set of available chapters is checked, including `nwt`, Unicode, and RTL text. Chapter bodies match the local reference byte for byte. XML structure, values, and element order are checked separately; serialization differences are reviewed.
- The flow from the catalog to selecting a translation and downloading all chapters is tested. Server limits do not break full translation downloads.
- Qt form-urlencoded requests, missing optional fields, and special characters are tested. A successful POST confirms that the data was written.
- The service cannot write to code or content. User data is stored separately. Invalid input does not cause unhandled exceptions or file operations outside allowed paths.
- No unexplained differences affect existing clients. Intentional fixes are listed separately from behavior kept for compatibility.
- Final Docker/nginx settings and operating commands are tested. Protocol tests are not presented as proof of correct display on a real phone.

## Results and decision log

### 2026-10-23 — migration approach and plan

- Chose Python + FastAPI and ruled out an intermediate PHP upgrade. The project is maintained for existing users, with no plans for new features.
- Agreed to keep SQLite, HTTP, startup/feedback, and the `nwt` translation. The new version will remove J2ME, `/nwtbible`, and `/fb`.
- Created this plan and the outline of a separate guide. Implementation, reference capture, and production checks have not started.
- Added a comparison of local reference responses with production to find server configuration differences. Production responses are not a trusted reference.
- The owner reports that production FPM runs as www-data behind nginx, without a Docker socket or connections to other services. Only `www` is mounted. File permissions are 644/755, content databases are 644, startup/feedback are 666, and the Smarty compile directory is 777. File owners and directory permissions have not been checked separately; mode bits alone do not prove that files cannot be changed.
- The owner is reviewing temporary PHP protections separately. Closing `/fb` immediately is outside the current work. No server settings were changed while creating these documents.
- Project documents use simple English so they remain accessible to other contributors.

### 2026-10-23 — step 2: source-based contract

- Added [Backend contract](backend-contract.md), covering routes, HTTP behavior, response formats and errors, SQLite reads/writes, public pages, static files, and client expectations. Runtime-dependent details remain explicit questions for reference capture.
- Found that the client ignores XML chapter numbers and relies on element order. Full-translation download can have 20 chapter requests outstanding. These are required compatibility checks.
- Confirmed that startup language/translation fields can be absent and feedback fields can be empty. Do not reject these legitimate client forms or add strict email validation.
- Found that the old passage route rejects digit-prefixed book codes such as `1co`, although the client creates these links. Fix this during the port, along with loose path parsing and missing-content handling.
- Checked local metadata and sample chapters in read-only mode. All advertised translation/language pairs have a content file; metadata has no chapter-number gaps. A full check of chapter rows remains in the later test steps.
- Step 2 is complete. No application or server configuration changed; the PHP reference server, HTTP captures, and production comparison have not started. The next step is the isolated PHP 5.6 reference server.

### 2026-10-24 — reference environment and historical Qt forms

- The owner confirmed PHP 5.6.40 from official `php:5.6-fpm`. Record the image digest and verify settings when building the reference server.
- No local Qt installation is needed for form encoding research. Reviewed upstream Qt 4.7.4 and 4.8.0 source; the relevant paths agree. Exact shipped patch versions remain unknown.
- Qt sends spaces as `%20` and leaves literal `+` unchanged. PHP 5.6.40's form decoder converts that plus to a space. Added source links and encoding examples to the contract; raw-plus handling in Python remains an explicit decision.
- Production is available for safe GET observations before the formal comparison. Keep public writes and malformed-input tests on the isolated reference server. No production requests were made during this clarification.

### 2026-10-24 — step 3: isolated PHP reference server

- Added a separate Compose project in [`tools/php-reference/`](../tools/php-reference/compose.yml), tested setup/check commands, and a [reference server guide](php-reference.md). The old application and deployment Compose files are unchanged.
- Pinned the official PHP 5.6.40 and nginx 1.25.4 images by digest and selected linux/amd64. Verified SQLite3, DOM, and mbstring; recorded image identities, runtime settings, and check results in [verification.md](../tools/php-reference/verification.md).
- Exported the committed `www` tree from `ebfbcd85cd01cf8c62f0fb59cf7533f5befc85c3`: 290 source/content files and 101 SQLite content hashes. The server uses this fixed copy, not live working files or user databases. Full snapshot hashes are checked before and after requests.
- Kept FPM on a closed internal network with no published port or default IPv4 route. Only nginx publishes `127.0.0.1:10056`. Docker 29 did not publish ports on an internal-only network, so nginx also uses a separate bridge. Both containers run without root, with read-only host mounts/root filesystems and resource limits. PHP has no route to the nginx bridge or the internet.
- Used tmpfs for startup/feedback databases and Smarty output. Relative database paths are links into `/data`; the unmodified handlers create the old schemas and commit writes there. No real user data is copied or kept.
- Made one PHP setting explicit: `date.timezone=UTC`. The unconfigured image falls back to UTC but warns during cold Smarty compilation, before headers; this made the first 404 return 200. The explicit timezone removes this configuration effect. Other PHP defaults, including displayed diagnostics and request ordering, remain unchanged. Production settings are still unverified.
- Passed 20 local HTTP checks, including normal pages, XML, identity/gzip, four byte-exact sample chapters (`kjv/e`, `rus/u`, `ara/a`, `nwt/e`), redirects, HEAD, a normal error, and committed startup/feedback rows. Confirmed the old raw-plus decoding bug with synthetic feedback.
- Tested a full reset and another cold-cache run: both user tables started empty, then gained one row each. All 20 response statuses and decoded body hashes matched. Code/content checksums stayed unchanged. Stopped the containers after verification.
- Step 3 is complete. Step 4 remains not started: the setup smoke report is not a reference response corpus or the external quick/full test suite. No requests were sent to production and the Python implementation has not started.

### 2026-10-24 — test ownership and Python form decoding

- The owner will implement the Python backend. This work adds external HTTP tests, references, and documentation without writing the service.
- Agreed to preserve a raw `+` as a plus on startup/feedback. Qt sends spaces as `%20`. Standard HTML forms that use `+` for spaces will therefore be read differently. Keep this deliberate rule in tests; do not use a content-based heuristic.
- Added separate PHP and Python test profiles. PHP observations record old crashes, diagnostics, and writes; the Python profile requires controlled errors, safe methods, removed routes, and corrected passage handling.
- Defined 400 responses without writes for malformed form fields: missing required keys, arrays, duplicate scalar keys, and invalid UTF-8. Empty required values remain valid. Startup language/translation remain optional and use SQL NULL when absent, as observed locally.
- Local capture confirms all 109,505 advertised chapter rows exist. The full test mode checks their exact bytes over HTTP with at most 20 outstanding requests. This is not a check of the future final Python image.

### 2026-10-24 — step 4: local references and external tests

- Saved 95 local request/response cases, XML for all 94 translation/language pairs, all static asset hashes, and the complete 109,505-chapter byte/hash index in [`tests/backend/reference/`](../tests/backend/reference/manifest.json). Original headers, identity/gzip entities, environment details, raw Qt-style forms, and synthetic stored values are retained.
- Added standard-library test commands with `BASE_URL`, quick/full modes, and a read-only SQLite observer for the target's temporary user data. No application imports or test-only server routes are needed. The runner rejects public targets before requests and never rewrites references. See the [test guide](backend-tests.md).
- Final quick run passed all 104 checks. A full local run checked every advertised chapter, all translation XML, and all static assets with no failures. A separate nine-check tool test covers negative cases such as changed bytes/order, JSON errors, plus-sign data loss, and writes hidden behind an error response. Details and validation scope are in [test verification](../tests/backend/verification.md).
- The full sweep used the same fixed chapter index as the final capture. Its PHP CPU limit was raised from one to the two CPUs available in the local VM to reduce emulation time, then restored. No image, application, content, PHP setting, mount, or network changed. The final isolation check passed and the containers were removed.
- Step 4 is complete. No production requests were made. Production comparison remains step 5; final environment and failure injection remain later checks. The owner has started the Python implementation in parallel and owns step 6. Only test tooling and documentation are part of this result.

### 2026-10-25 — catalog route boundary tests

- Rechecked the client: `Languages::reload` calls `Paths::wsUrl("meta")`, which builds the exact `/meta` URL without a query or trailing slash. The client does not generate the two noncanonical variants in normal use.
- Removed `meta-query` and `meta-slash` from the Python profile. Their old 404 responses describe PHP's raw-URI matching and are not needed for client compatibility. Kept both saved responses and both checks in the PHP profile.
- Python may ignore extra query parameters or use normal framework behavior for `/meta/`. Canonical `/meta` must still return 200 directly. The owner's backend code is unchanged by this test correction.

### 2026-10-25 — home page translation order

- The owner confirmed that PHP's historical database insertion order is a bug. Sort translation names alphabetically within each language on the Python home page. Keep the client-facing `/meta` XML order unchanged.
- Updated the Python home page checks to require sorted names and preserve language groups, translation contents, counts, and links. The old fixed copyright end year may be updated. Saved PHP responses and the PHP profile remain unchanged.
- Backend implementation stays with the owner. Connection transaction handling and the corrected GET/HEAD Allow header were checked using temporary databases and local in-process requests; no backend source was edited.

### 2026-10-25 — translation query boundary tests

- Rechecked `Translation::reload`, `MetaInfoParser`, and the fixed catalog. The client sends one `trans` and one `lang`, taken from `/meta`. No published pair uses an empty or `0` code. The client does not generate any of the four disputed requests in normal use.
- Moved `translation-zero` and `translation-bare` to PHP-only observations. Python does not need to copy PHP's `empty("0")` behavior or its raw-URI route matching.
- Made `translation-array` and `translation-duplicate` optional Python validation checks, enabled by `--include-optional`. They no longer block the default quick/full runs. Their strict rejection policy remains available without treating it as a client requirement.
- Kept the saved PHP responses and PHP profile, required SQL/file/POST safety checks, and all advertised translation XML checks. No backend implementation was changed.
