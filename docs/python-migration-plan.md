# MeeBible Python migration plan

Last updated: 2026-10-23.

The goal is to keep existing MeeBible clients on Symbian and MeeGo working by replacing PHP 5.6 with a small Python + FastAPI service. New features, new users, and a redesign are out of scope. We will not migrate to PHP 8.

This document tracks the plan, completion criteria, main results, and decisions. Keep the description of the built service and operating instructions in the separate [Python backend guide](python-backend.md). Write project documents in simple English.

## Work plan

Statuses: **Not started**, **In progress**, **Done**, **Blocked**. Mark a step as done only when its completion criteria are met. Add a short note about important results and changes to the log below. Update this plan when the scope changes.

| No. | Step | Completion criteria | Status |
| --- | --- | --- | --- |
| 1 | Agree on scope and workflow | Features to keep and remove are agreed; the plan and separate guide exist | Done |
| 2 | Describe the external contract from PHP, nginx, and client code | Routes, methods, parameters, responses, errors, and side effects are listed; HTTP, encodings, Qt forms, and client parsers are covered | Not started |
| 3 | Set up an isolated PHP 5.6 reference server | The old service can be started in a repeatable way without public access; it uses a fixed content snapshot and temporary startup/feedback databases; commands and settings are documented | Not started |
| 4 | Capture local reference responses and build external tests | Responses and environment details are saved; tests accept BASE_URL; routes to keep, errors, and POST writes are covered; quick and full test modes exist | Not started |
| 5 | Compare reference responses with meebible.org | A small set of safe GET requests is compared at a low rate; differences in status, headers, and bodies are classified; configuration differences are separated from data and application differences; unexplained differences are recorded | Not started |
| 6 | Build the Python backend | All features to keep are implemented using the existing SQLite files; removed routes return 404; agreed bugs are fixed; quick contract tests pass | Not started |
| 7 | Check all content and the final environment | All translations, languages, books, and chapters are checked; POST, errors, permissions, nginx, gzip, and HTTP are tested; every difference from the reference is explained and accepted | Not started |
| 8 | Prepare the release and operating guide | Tested commands cover startup, tests, updates, backup, restore, and switching servers; dependency versions are pinned; project layout and limits are documented | Not started |
| 9 | Switch production to Python | User SQLite databases are copied consistently with old writes stopped; the upstream is switched; reads, writes, logs, and redirects are checked; old PHP no longer serves public requests | Not started |

Implementation can proceed in parts as contracts become clear. Run the full content checks and final comparison on the final image and configuration. This plan does not mean that production has already been changed.

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
