# MeeBible Python migration plan

Last updated: 2026-11-05.

The goal is to keep existing MeeBible clients on Symbian and MeeGo working by replacing PHP 5.6 with a small Python + FastAPI service. New features, new users, and a redesign are out of scope. We will not migrate to PHP 8.

This document tracks the plan, completion criteria, main results, and decisions. The source-based API and website contract is in [Backend contract](backend-contract.md). Keep the description of the built service and operating instructions in the separate [Python backend guide](python-backend.md). Write project documents in simple English.

**Current status:** steps 6–9 are complete for the local final image and prepared external proxy configuration. All 99 pytest cases pass. Through the local copy of the external proxy, quick passed 93 checks and full passed 109,718 checks, including all 109,505 chapters and POST storage checks, without failures or skips. Fourteen SQLite failure/recovery checks and twelve HTTP input-limit checks pass. Production comparison, applying the prepared proxy changes, backup/restore rehearsal, and switching production remain pending. Production has not been changed.

## Work plan

Statuses: **Not started**, **In progress**, **Done**, **Blocked**. Mark a step as done only when its completion criteria are met. Add a short note about important results and changes to the log below. Update this plan when the scope changes.

| No. | Step | Completion criteria | Status |
| --- | --- | --- | --- |
| 1 | Agree on scope and workflow | Features to keep and remove are agreed; the plan and separate guide exist | Done |
| 2 | Describe the external contract from PHP, nginx, and client code | Routes, methods, parameters, responses, errors, and side effects are listed; HTTP, encodings, Qt forms, and client parsers are covered | Done |
| 3 | Set up an isolated PHP 5.6 reference server | The old service can be started in a repeatable way without public access; it uses a fixed content snapshot and temporary startup/feedback databases; commands and settings are documented | Done |
| 4 | Capture local reference responses and build external tests | Responses and environment details are saved; tests accept BASE_URL; routes to keep, errors, and POST writes are covered; quick and full test modes exist | Done |
| 5 | Compare reference responses with meebible.org | A small set of safe GET requests is compared at a low rate; differences in status, headers, and bodies are classified; configuration differences are separated from data and application differences; unexplained differences are recorded | Not started |
| 6 | Build the Python backend | All features to keep are implemented using the existing SQLite files; removed routes return 404; agreed bugs are fixed; default quick contract tests pass | Done |
| 7 | Add Python unit tests | Form and verse parsing, passage extraction, and SQLite behavior are tested in isolation; temporary databases cover commits, rollback, and failed writes; one documented command runs the tests | Done |
| 8 | Build the Python Docker environment | A Dockerfile and updated Compose configuration run Python; dependencies use the lock file; code/content are read-only; user data has a separate persistent writable path; startup, shutdown, and restart are checked | Done |
| 9 | Check all content and the final environment | Quick and full suites pass on the final Python image; external proxy behavior is checked, including all 109,505 advertised chapters; POST, errors, permissions, persistence, gzip, caching, and concurrent downloads are checked; differences are explained and accepted | Done |
| 10 | Prepare the release and operating guide | Tested commands cover startup, tests, updates, backup, restore, and switching servers; dependency versions are pinned; project layout and limits are documented; user-data transfer and recovery are rehearsed | Not started |
| 11 | Switch production to Python | User SQLite databases are copied consistently with old writes stopped; the upstream is switched; reads, writes, logs, and redirects are checked; old PHP no longer serves public requests | Not started |

Implementation can proceed in parts as contracts become clear. Run the full content checks and final comparison on the final image and configuration. This plan does not mean that production has already been changed.

The owner has written the Python service. The assistant maintains external tests and reference evidence. Implementation unit tests belong to the Python code; the external HTTP suite stays independent of that implementation.

## Next work

1. **Production comparison (step 5).** Make the planned small, slow, safe GET comparison with `meebible.org`. Classify content/header/configuration differences. Do not send production POST requests or malformed input. Keep production evidence separate from local references.
2. **Release and operating guide (step 10).** Rehearse consistent SQLite backup, transfer with old writes stopped, restore, and recovery. Verify the deployment architecture and document tested commands. Apply the prepared `proxy.conf` changes only during the authorized release work: upstream HTTP/1.1 keepalive and the 1 MiB body limit. The owner reports that the existing external proxy otherwise matches the repository config, with a CORS-header difference; CORS is not needed by the native clients.
3. **Switch production (step 11).** Move the user databases consistently, switch the existing external proxy upstream, check live reads/writes/logs/redirects, and try a real mobile client where possible. Local checks do not mean that this switch has happened.

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
- Final Docker and external proxy settings and operating commands are tested. Protocol tests are not presented as proof of correct display on a real phone.

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

### 2026-10-25 — chapter query and method boundary tests

- Rechecked `Translation::requestChapter` and its callers. Normal reading and full download use GET with one `trans`, `lang`, `book`, and `chapter`. The client does not send duplicate keys, bare `/chapter`, or POST `/chapter`.
- Moved `chapter-bare` to PHP-only observations. Python may return its normal missing-parameter response instead of reproducing the old raw-URI 404.
- Made `chapter-duplicate` and `chapter-method-post` optional Python checks, enabled by `--include-optional`. The existing Python endpoint already returns 405 and the correct Allow header for POST; the optional test additionally requires a text/HTML error body. A framework JSON 405 is allowed by default.
- Kept the PHP references/profile, required chapter content and file-access checks, and validation on writable routes. No backend source was edited.
- All 13 test-tool checks passed. A local in-process run against the current Python app passed all 23 mandatory chapter request cases, including sample bytes, errors, HEAD, and gzip. Reference checksums stayed unchanged; this was not the full 109,505-chapter sweep.

### 2026-10-27 — browser passage behavior

- Rechecked `Place::siteUrl` and local GET responses on port 8000. The app shares URLs for a browser; chapter numbers are formatted from integers. A nonnumeric chapter path is not generated by the client, so `passage-bad-path` is now PHP-only.
- Accepted 301 or 307 for verse normalization without requiring a redirect body or Content-Type. Accepted a trailing-slash redirect to the same canonical passage; direct rendering of the same page is also allowed. Tests check the canonical destination and final content on the loopback BASE_URL, without following public URLs.
- The owner chose to retain PHP's 200 empty/partial page behavior for missing verses. Both the saved PHP responses and current Python responses do this. Keep checks of page content, metadata, and existing selected verses; missing chapters still require controlled errors.
- Updated the contract and test rules. Saved PHP responses, the PHP profile, chapter bytes, and the owner's backend source are unchanged.
- All 17 test-tool checks passed, including redirect target/loop checks and empty/partial page checks. All 19 Python passage cases passed over HTTP against the owner's server at `localhost:8000`. All 95 PHP case IDs remain present and saved reference checksums match.

### 2026-10-28 — SIS download MIME type

- Accepted `application/vnd.symbian.install` as well as the reference `application/octet-stream` for the SIS range case. This is a binary download MIME mapping difference, not a change to the package or range behavior.
- Local GET checks confirmed 206, `Content-Range: bytes 0-63/281860`, Content-Length 64, and exact reference range bytes. The full 281,860-byte package matches its saved SHA-256.
- Kept `static-range` required, preserved its range and byte checks, and added negative tool checks for wrong types, statuses, ranges, and content. Other asset MIME checks, PHP references/profile, and backend source are unchanged.

### 2026-10-28 — privacy query boundary test

- Confirmed that the feedback page opens exactly `http://meebible.org/privacy`, without query parameters. The old 404 for `/privacy?x=1` comes from PHP's raw-URI matching.
- Moved `privacy-query` to PHP-only observations. Python may ignore extra query parameters. Canonical `/privacy` and its existing policy content remain required.
- Updated the selection check and documentation. Saved references, the PHP profile, and backend source are unchanged.

### 2026-11-04 — feedback query boundary test

- Confirmed that `Feedback::send` posts to exactly `/feedback`, with all fields in the body. The client does not send `GET /feedback?x=1`; the case's old 404 comes from PHP's raw-URI matching.
- Moved `feedback-query` to PHP-only observations. A normal framework JSON 405 is allowed for this unused GET variant in Python. It must not write feedback; POST submission, validation, and method checks remain required.
- Updated the selection check and documentation. Saved references, the PHP profile, and backend source are unchanged.

### 2026-11-04 — Python implementation passes quick tests

- The owner reports that the implementation now passes all required tests. The saved local `tests/backend/.local/quick-python.json` confirms a default Python quick run on `localhost:8000`: 93 passed, zero failed, zero skipped, `read_only=false`, and `include_optional=false`. The run includes POST and committed-storage checks. The optional strict checks were not enabled, and this report does not cover the full chapter sweep.
- Marked step 6 complete. Added separate unit-test and Docker steps, and expanded the remaining work on persistent data, final-image testing, production comparison, and backup/restore. Steps 7–9 from the earlier table are now steps 9–11; existing steps 1–6 keep their numbers.
- The root Compose/nginx files still use PHP/FastCGI. Python user databases still use paths relative to the process working directory. These are concrete tasks for the container stage, not completed deployment settings.
- Unit tests and container conversion are the next implementation tasks; neither was started in this documentation update. No application code, deployment files, user databases, or production services were changed.

### 2026-11-04 — step 7: Python unit tests

- Added 28 unittest methods in `backend/tests/`, separate from external tests and references. No dependencies were added.
- Tested form/verse parsing, passage extraction, and temporary SQLite reads/writes, including commit failure and rollback. The test guide documents one command and the isolation boundaries.
- The run passed 25 methods; three methods reported eight subtest errors exposing two defects: missing feedback name/email validation and empty verse components raising ValueError. Tests retain the required behavior and remain failing for the owner to fix. Step 7's test-writing criteria are met; the implementation is not fully validated.
- Implementation, external references, Docker, and Compose were unchanged. Full validation remains pending.

### 2026-11-04 — pytest and API integration tests

- Migrated implementation tests to native pytest assertions, fixtures, and parameterized parser cases. Added explicit pytest/httpx development dependencies and updated `uv.lock`.
- Added API integration tests through TestClient with disposable synthetic SQLite content and user data, real templates, and middleware. External tests and references remain independent.
- All 93 cases passed (53 unit, 40 API), including regressions after the owner's feedback and verse-parser fixes. Ruff checks and formatting passed. No backend implementation, Docker, or Compose changes were made by this task. Full external validation remains pending.

### 2026-11-05 — full-run transport diagnosis

- The original full run reached 16,000 chapter checks, then failed with connection errors. A separate reproduction showed that opening a new TCP connection for each chapter exhausts this Mac's 16,384 ephemeral ports after roughly 16,000 rapid requests, while the server remains alive.
- Fixed the external client to reuse one connection per chapter worker, keep the 20-request limit, and close all connections after the workers finish. Failed requests are still reported without retries; chapter status, size, and hash checks are unchanged. All 21 test-tool checks passed.
- After the owner restarted the server without reload, a full read-only HTTP run checked all 109,505 chapters and passed 109,700 checks with zero failures in 27.73 seconds. The 18 POST/write-method cases were intentionally skipped, so the report is marked partial. Original failure evidence and user databases were preserved.
- Details and local report paths are in [`tests/backend/verification.md`](../tests/backend/verification.md). This confirms development-server content and transport behavior; final Docker/nginx validation remains pending. No backend implementation or deployment files were changed by this investigation.

### 2026-11-05 — step 8: Python Docker environment

- Added a multi-stage Dockerfile and allowlist-based `.dockerignore`. Python 3.12.12, uv 0.9.10, and nginx 1.28.0 use pinned image indexes; runtime packages install from `uv.lock` without development dependencies. The image excludes user databases, tests, local caches/logs, and PHP sources.
- Replaced PHP/FastCGI in the root Compose project with Uvicorn and HTTP proxying. The application owns static files, gzip, ranges, and conditional responses. nginx reuses upstream connections and refreshes Docker DNS after backend recreation. Only the nginx loopback port is published; backend uses an internal network. The PHP reference project is unchanged.
- Added `MEEBIBLE_DATA_DIR`, defaulting to the process working directory for local compatibility. Compose uses a separate `/data` named volume. Both services run without root on read-only filesystems, with bounded writable tmpfs directories and rotating Docker logs.
- Checked startup, graceful SIGTERM shutdown, restart, read-only code/content, named-volume initialization, and preservation of both user tables after backend recreation. Initial container checks found restrictive host content permissions and nginx temporary paths; the image now normalizes read permissions and nginx uses `/tmp` for every temporary path.
- Fixed an OS-dependent favicon MIME mapping by explicitly retaining `image/x-icon`; added an API regression assertion. No saved reference or test expectation was relaxed.
- The first full container run exhausted nginx upstream TCP ports after 28,000 chapters because upstream connections were not pooled. Added upstream keepalive; the complete rerun passed 109,718 checks in 44.61 seconds with 20 workers, including 109,505 chapter hashes and POST writes. Quick passed 93 checks with no failures or skips. ETag/304 and HEAD were also checked through nginx. All 93 pytest cases, Ruff checks, and formatting passed.
- Step 8 is complete. Step 9 is in progress: container database lock/read-only/write-failure tests and input/resource-limit review remain. See [container verification](../tests/backend/verification.md#python-container-checks-2026-11-05) and the updated [backend guide](python-backend.md). No production requests, real user-data changes, or server switching were performed.

### 2026-11-05 — user-data bind mount

- At the owner's request, replaced the root Compose named volume with `./data:/data`. The bind source must already exist; Compose will not create a root-owned directory. The guide documents UID/GID 10001 permissions on Linux. Host data is excluded from Git and the Docker build context. Existing named volumes and databases are not moved or deleted automatically.
- Validated Compose, SQLite writes as UID 10001 into `./data`, persistence after forced backend recreation, and `/meta` through nginx. Removed only the synthetic probe database and stopped the test containers.

### 2026-11-05 — use the existing external reverse proxy

- The owner confirmed that the server already has a reverse proxy. Removed the nginx service, the root `nginx.conf`, and the explicit frontend/backend networks. Compose now has one backend on its default bridge, publishing Uvicorn only at `127.0.0.1:${MEEBIBLE_PORT:-10001}`. Kept the `./data` bind mount, read-only filesystem, non-root user, healthcheck, and graceful shutdown settings. The isolated PHP reference configuration remains unchanged.
- Validated the simplified Compose and repeated quick/full HTTP tests directly against the container on loopback port 10057 with disposable bind-mounted data. Quick passed 93 checks; full passed 109,718, including all 109,505 chapters and POST/storage checks, with zero failures or skips. Full used 20 workers and took 43.51 seconds. Reports: `tests/backend/.local/direct-quick-python.json` and `direct-full-python.json`.
- The external proxy should forward to the host loopback port; request limits, upstream keepalive, and forwarded-header policy belong to its release configuration. No external proxy or production server was changed.

### 2026-11-05 — step 9: final image, SQLite failures, and limits

- Added `tests/backend/container_checks.py` and disposable Linux tmpfs test configuration. All 14 checks pass: read-only database, unwritable journal directory, rejected insert, corrupt file, full filesystem, locked write with responsive reads, and recovery, for both startup and feedback. Failed requests return HTTP 500 without `ok`, existing rows stay unchanged, and recovery adds exactly one row.
- Found that synchronous SQLite writes in async handlers blocked the event loop for the five-second SQLite lock wait. Moved the writes to the thread pool; `/meta` remains responsive during the same locked-write test. Commit-before-success behavior is unchanged.
- Added streamed POST limits of 1 MiB of encoded bytes, including chunked requests. Excess returns 413 without writes; exactly 1 MiB remains accepted. Bounded verse specifications to 4096 characters and range parsing to 300 unique values. All 12 direct/proxy HTTP limit checks pass. Qt plus decoding and normal client forms remain unchanged.
- The owner reports that the external proxy matches `proxy.conf` except for CORS. Prepared upstream HTTP/1.1 keepalive and an explicit 1 MiB body limit in that file. Tested a local copy with only addresses, ports, and log paths adapted to Docker. No production proxy was changed. The root Compose still runs only Python; nginx exists solely in the test companion configuration.
- The final arm64 image passed 99 pytest cases, Ruff lint/format checks, quick (93 checks, 0.60 seconds), and full (109,718 checks, 40.65 seconds), with zero failures/skips. Full checked all 109,505 chapters with 20 workers, plus POST/storage. ETag/304, HEAD, permissions, and persistence were checked separately. See [verification](../tests/backend/verification.md#stage-9-final-validation-2026-11-05).
- Reviewed resource bounds: 128 processes/threads, 16 MiB `/tmp`, bounded forms and verse expansion. No hard CPU/total-memory limits are set. One full-run sample showed 67.16 MiB and 25 tasks for Python and 3.191 MiB and 2 tasks for the test proxy; these are observations, not capacity guarantees. Target host capacity and live ingress remain release checks.
- Permission-fault fixture setup on Docker Desktop bind mounts produced intermittent SQLite I/O errors; final permission/full-disk injection ran on Linux tmpfs. Final HTTP and persistence checks used a separate disposable bind directory. Existing local/production user databases, old reports, and saved references were preserved.
- Step 9 is complete within the local validation scope. Steps 5, 10, and 11 remain; target-architecture and real-client checks are still release work.
