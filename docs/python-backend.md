# MeeBible Python backend

**Status: planning. The Python backend is not implemented yet.** This guide will describe the built service and tested commands. The plans below do not mean that the files or commands already exist.

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
| `docs/python-migration-plan.md` | Plan, status, and decisions |
| `docs/backend-contract.md` | Source-based contract and reference test checklist |

After implementation, add the actual paths for Python code, tests, reference responses, dependencies, and configuration. Remove obsolete paths or clearly label them as historical.

## Local startup

Commands have not been defined or tested yet. During implementation, document requirements, dependency installation, data setup, configuration variables, Docker startup, and a basic availability check. Document the isolated old PHP setup for reference capture separately.

## Tests

During implementation, add tested commands for quick and full test runs, the target BASE_URL setting, and report locations. Updating reference responses must be an explicit step with a review of differences. Do not update them automatically when tests fail.

Production comparison uses a separate, limited read-only procedure described in the plan. Do not point the normal test suite, which includes POST and invalid requests, at production.

## Deployment and switching servers

Before release, document image building and identification, mounts and permissions, nginx settings, checks before switching, and the upstream switch procedure. Moving startup/feedback requires stopping old writes and taking a consistent SQLite copy. Define recovery steps for a failed release. Do not treat a public rollback to vulnerable PHP as a safe default.

## Updates and maintenance

Before release, add a tested procedure for updating Python, dependencies, and base images: change pinned versions, rebuild, run quick/full checks, and deploy. Document log locations and size limits, and how to diagnose write errors, missing chapters, and service outages.

## Backup and restore

Before release, document independent content and configuration backups, consistent user SQLite backups, restore steps, and checks after restore. Do not copy active SQLite files without accounting for journal/WAL files. Do not include user data or production logs in test reference files or Git.
