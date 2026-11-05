# MeeBible Python backend

**Status: the Python Docker environment is built and locally checked.** All 93 unit/API cases pass. The Compose project runs Uvicorn directly behind the server's existing external reverse proxy. Direct container HTTP tests passed 93 quick checks and 109,718 full checks, including all 109,505 chapters and POST writes, with no failures or skips. External proxy validation remains pending. Container failure injection and release preparation remain pending; production has not been switched.

See the [migration plan](python-migration-plan.md) for work order, status, completion criteria, and decisions. Keep this guide in simple English.

The [backend contract](backend-contract.md) describes current source behavior, client requirements, intended fixes, and questions for reference capture.

## Purpose and limits

The service must support existing mobile clients without client updates. Keep HTTP, existing SQLite files with Bible translations, and the external contract of active features. Keep the `nwt` translation. Remove the old `/nwtbible` endpoint, J2ME code, and `/fb`.

## Architecture

The application is in `backend/`, using FastAPI, sqlite3, Jinja templates, and lxml for passage HTML. Uvicorn runs in Docker behind the existing external reverse proxy. Code and content must be read-only. Startup/feedback use `MEEBIBLE_DATA_DIR` (default: the process working directory). Compose sets it to `/data`, bound to `./data/` in the project root. The directory must already exist and be writable by UID/GID 10001. The original schemas are unchanged.

During container and release work, document:

- actual Python and dependency versions, and how they are pinned;
- responsibilities of the external proxy and the application, HTTP/gzip settings, and routing;
- SQLite access, read-only content paths, and writable data paths;
- input limits and important intentional differences from PHP.

## Where to find things

These are the main implementation and historical source locations:

| Path | Contents |
| --- | --- |
| `backend/` | Python application, SQLite access, form/verse parsing, templates, static assets, and Bible content |
| `pyproject.toml`, `uv.lock` | Python requirements and locked dependencies |
| `www/urls.php`, `www/views/` | Old routes and handlers |
| `www/loadchapter.inc.php` | Chapter loading, including the special `nwt` layout |
| `www/content/` | SQLite catalog and translation text databases |
| `www/templates/`, `www/static/` | Templates and static files |
| `src/Paths.cpp`, `src/Translation.cpp`, `src/Languages.cpp` | Client URLs and content loading |
| `src/StartupTracker.cpp`, `src/Feedback.cpp` | Client POST requests |
| `docker-compose.yml`, `proxy.conf` | Python container configuration and the existing outer host proxy example |
| `tools/php-reference/` | Tested, isolated PHP 5.6.40 reference setup and checksum manifest |
| `docs/php-reference.md` | PHP reference startup, settings, checks, reset, and stop commands |
| `tests/backend/` | External HTTP tests, reference capture, and saved responses |
| `docs/backend-tests.md` | Quick/full commands, BASE_URL, data observation, and test expectations |
| `docs/python-migration-plan.md` | Plan, status, and decisions |
| `docs/backend-contract.md` | Source-based contract and reference test checklist |

Implementation unit tests are in `backend/tests/`; run `uv run --locked pytest -q` from the repository root. See the test guide for dependencies, coverage, and suite selection. During container and release work, add the deployment paths. Remove obsolete paths or clearly label them as historical.

## Local startup

For the old service, follow the tested [PHP reference server guide](php-reference.md). It uses pinned PHP 5.6.40/nginx images, a fixed Git content snapshot, read-only mounts, and temporary startup/feedback databases. Only `http://127.0.0.1:10056` is published. Setup checks and a complete reset passed; image identities and results are in [verification.md](../tools/php-reference/verification.md). The containers were stopped after verification.

The root Dockerfile uses Python 3.12.12 (Debian Bookworm slim) and uv 0.9.10. Both base indexes are pinned by digest. `uv sync --locked --no-dev --no-install-project` installs runtime dependencies from `uv.lock`; no dependency resolution or downloads occur at startup. The image includes Bible content, templates, and static assets. Local user databases, logs, caches, tests, and the old PHP tree are excluded by `.dockerignore`. Image build normalizes content read permissions, including when local source files are mode 600/700.

From the repository root, with Docker Engine and Compose v2 available:

```sh
# Docker Desktop: create the bind source before starting.
mkdir -p data
docker compose build
docker compose up -d --wait
curl --fail http://127.0.0.1:10001/meta
docker compose logs --tail 50
docker compose restart
docker compose down
```

`MEEBIBLE_PORT=10057 docker compose up -d --wait` selects another loopback port. Use a distinct project (`-p meebible-python-check`) for disposable local tests. The default root setup is available only on `127.0.0.1:10001`; production ingress configuration is part of the later release step. It does not attach to a shared nginx-proxy network or change production.

Compose runs one backend service and publishes container port 8000 only on `127.0.0.1:${MEEBIBLE_PORT:-10001}`. It uses the default Compose bridge network; there is no nginx container or additional frontend/backend network. The server's existing external reverse proxy forwards to `http://127.0.0.1:10001` (as in `proxy.conf` when the proxy runs on the host). A proxy in another container needs its own host routing or shared-network configuration; its loopback address does not refer to this host.

The application handles gzip and all static files, including HEAD, ETag/304, and byte ranges. Keep HTTP without a forced HTTPS redirect. Configure request-body limits and upstream connection reuse at the external proxy; the removed internal nginx's 1 MiB body limit no longer applies. Uvicorn currently ignores forwarded headers (`--no-proxy-headers`), so client IP forwarding remains part of release configuration.

The backend has a read-only root filesystem, no Linux capabilities, `no-new-privileges`, and a writable temporary directory in a bounded tmpfs mount. Uvicorn runs as UID/GID 10001 and receives SIGTERM directly; its graceful shutdown timeout is 20 seconds, within Compose's 30-second stop period. A read-only `/meta` healthcheck lets `up --wait` check readiness. Logs go to Docker stdout/stderr and rotate at 5 MiB with two files.

Both SQLite databases and their journal files live in the host directory `./data/`, mounted at `/data`. The directory is excluded from Git and the Docker build context. Compose requires it to exist rather than creating a root-owned directory automatically. On Linux, prepare a new directory with `sudo install -d -o 10001 -g 10001 -m 0700 data` before starting; for an existing directory, ensure its files are writable by UID/GID 10001. On Docker Desktop, `mkdir -p data` was checked locally. Container recreation, rebuilds, `docker compose down`, and `down -v` preserve these host files. Existing production databases are not imported automatically.

For local execution outside Docker:

```sh
mkdir -p /tmp/meebible-dev-data
MEEBIBLE_DATA_DIR=/tmp/meebible-dev-data uv run --locked uvicorn backend:app
```

For the external tests, use disposable data visible on the host. Create a temporary Compose override (replace the source with an absolute path):

```yaml
services:
  backend:
    volumes:
      - type: bind
        source: /tmp/meebible-contract-data
        target: /data
```

On Linux, create that directory with ownership 10001:10001 and mode 0700 before starting. Docker Desktop must also allow access to the path. Start with `docker compose -p meebible-python-check -f docker-compose.yml -f /tmp/meebible-test.yml up -d --wait`, then use `BASE_URL=http://127.0.0.1:10001 USER_DATA_DIR=/tmp/meebible-contract-data python3 tests/backend/run.py quick`. Keep the same project and `-f` arguments for stop/restart commands. These commands write synthetic records to the selected test directory.

Historical Qt source is used to define client encoding fixtures. A local Qt installation is not a prerequisite; see the form encoding section in the [backend contract](backend-contract.md).

## Tests

Use the [external test guide](backend-tests.md) while implementing the service. The tests need only Python's standard library and a separately running server. Set `BASE_URL` and `USER_DATA_DIR` for temporary SQLite files, then run `python3 tests/backend/run.py quick` or `full`. The default Python profile checks deliberate fixes; the PHP profile validates the captured old behavior. Local reports are in `tests/backend/.local/`. Updating references is an explicit capture and review; failures never update them automatically.

The smaller PHP setup check remains `python3 tools/php-reference/reference.py check` after local startup. It checks the isolated server itself. The external suite adds saved responses, malformed requests, form storage checks, and full catalog/content coverage. Both are separate from checks on the final Python image and real phones.

Production comparison uses a separate, limited read-only procedure described in the plan. Do not point the normal test suite, which includes POST and invalid requests, at production.

## Deployment and switching servers

Before release, document image building and identification, mounts and permissions, external proxy settings, checks before switching, and the upstream switch procedure. Moving startup/feedback requires stopping old writes and taking a consistent SQLite copy. Define recovery steps for a failed release. Do not treat a public rollback to vulnerable PHP as a safe default.

## Updates and maintenance

Before release, add a tested procedure for updating Python, dependencies, and base images: change pinned versions, rebuild, run quick/full checks, and deploy. Document log locations and size limits, and how to diagnose write errors, missing chapters, and service outages.

## Backup and restore

Before release, document independent content and configuration backups, consistent user SQLite backups, restore steps, and checks after restore. Do not copy active SQLite files without accounting for journal/WAL files. Do not include user data or production logs in test reference files or Git.
