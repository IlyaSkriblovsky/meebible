# MeeBible Python backend

**Status: PHP reference server and external tests are ready for implementation work.** The owner is writing the Python service. The architecture and operating sections below remain plans until that service is built and checked.

See the [migration plan](python-migration-plan.md) for work order, status, completion criteria, and decisions. Keep this guide in simple English.

The [backend contract](backend-contract.md) describes current source behavior, client requirements, intended fixes, and questions for reference capture.

## Purpose and limits

The service must support existing mobile clients without client updates. Keep HTTP, existing SQLite files with Bible translations, and the external contract of active features. Keep the `nwt` translation. Remove the old `/nwtbible` endpoint, J2ME code, and `/fb`.

## Architecture

The plan is to use FastAPI + Uvicorn behind nginx, the standard sqlite3 module for existing databases, XML serialization for metadata, and HTML templates for website pages. Code and content must be read-only. Store startup/feedback data separately with write access.

After implementation, document:

- actual Python and dependency versions, and how they are pinned;
- responsibilities of nginx and the application, HTTP/gzip settings, and routing;
- SQLite access, read-only content paths, and writable data paths;
- input limits and important intentional differences from PHP.

## Where to find things

Before the migration, these are the main locations in the project:

| Path | Contents |
| --- | --- |
| `www/urls.php`, `www/views/` | Old routes and handlers |
| `www/loadchapter.inc.php` | Chapter loading, including the special `nwt` layout |
| `www/content/` | SQLite catalog and translation text databases |
| `www/templates/`, `www/static/` | Templates and static files |
| `src/Paths.cpp`, `src/Translation.cpp`, `src/Languages.cpp` | Client URLs and content loading |
| `src/StartupTracker.cpp`, `src/Feedback.cpp` | Client POST requests |
| `docker-compose.yml`, `nginx.conf`, `proxy.conf` | Existing startup and proxy configuration; check whether it matches production |
| `tools/php-reference/` | Tested, isolated PHP 5.6.40 reference setup and checksum manifest |
| `docs/php-reference.md` | PHP reference startup, settings, checks, reset, and stop commands |
| `tests/backend/` | External HTTP tests, reference capture, and saved responses |
| `docs/backend-tests.md` | Quick/full commands, BASE_URL, data observation, and test expectations |
| `docs/python-migration-plan.md` | Plan, status, and decisions |
| `docs/backend-contract.md` | Source-based contract and reference test checklist |

After implementation, add the actual paths for Python code, tests, reference responses, dependencies, and configuration. Remove obsolete paths or clearly label them as historical.

## Local startup

For the old service, follow the tested [PHP reference server guide](php-reference.md). It uses pinned PHP 5.6.40/nginx images, a fixed Git content snapshot, read-only mounts, and temporary startup/feedback databases. Only `http://127.0.0.1:10056` is published. Setup checks and a complete reset passed; image identities and results are in [verification.md](../tools/php-reference/verification.md). The containers were stopped after verification.

Python startup commands have not been defined yet. During implementation, document requirements, dependency installation, data setup, configuration variables, Docker startup, and a basic availability check.

Historical Qt source is used to define client encoding fixtures. A local Qt installation is not a prerequisite; see the form encoding section in the [backend contract](backend-contract.md).

## Tests

Use the [external test guide](backend-tests.md) while implementing the service. The tests need only Python's standard library and a separately running server. Set `BASE_URL` and `USER_DATA_DIR` for temporary SQLite files, then run `python3 tests/backend/run.py quick` or `full`. The default Python profile checks deliberate fixes; the PHP profile validates the captured old behavior. Local reports are in `tests/backend/.local/`. Updating references is an explicit capture and review; failures never update them automatically.

The smaller PHP setup check remains `python3 tools/php-reference/reference.py check` after local startup. It checks the isolated server itself. The external suite adds saved responses, malformed requests, form storage checks, and full catalog/content coverage. Both are separate from checks on the final Python image and real phones.

Production comparison uses a separate, limited read-only procedure described in the plan. Do not point the normal test suite, which includes POST and invalid requests, at production.

## Deployment and switching servers

Before release, document image building and identification, mounts and permissions, nginx settings, checks before switching, and the upstream switch procedure. Moving startup/feedback requires stopping old writes and taking a consistent SQLite copy. Define recovery steps for a failed release. Do not treat a public rollback to vulnerable PHP as a safe default.

## Updates and maintenance

Before release, add a tested procedure for updating Python, dependencies, and base images: change pinned versions, rebuild, run quick/full checks, and deploy. Document log locations and size limits, and how to diagnose write errors, missing chapters, and service outages.

## Backup and restore

Before release, document independent content and configuration backups, consistent user SQLite backups, restore steps, and checks after restore. Do not copy active SQLite files without accounting for journal/WAL files. Do not include user data or production logs in test reference files or Git.
