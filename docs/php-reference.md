# Local PHP 5.6 reference server

Last checked: 2026-10-24. Migration step 3 is complete. Local responses and external tests are available in the [test guide](backend-tests.md). The owner is writing the Python service.

This server runs the old application from a fixed Git revision. Use it for local reference capture, POST tests, and invalid requests. Its settings are known; matching all production settings is a later step.

## Requirements and files

Use Python 3.11 or later, Git, and Docker with Compose. The checker uses `docker image inspect --platform`; use Docker Engine 29 or later. On Apple Silicon, enable support for running amd64 images. No local PHP, Qt, Python packages, or image build is needed. Allow about 2 GB of disk space for the images and snapshot.

The setup was checked on macOS arm64 with Docker Desktop 4.93.0, Engine 29.8.1, and Compose 5.5.1. Both containers run as linux/amd64 so the old PHP runtime is the same on different hosts.

| File | Purpose |
| --- | --- |
| [`compose.yml`](../tools/php-reference/compose.yml) | Separate local project, pinned images, mounts, network, and limits |
| [`reference.py`](../tools/php-reference/reference.py) | Prepare the snapshot and check the running server |
| [`content.sha256`](../tools/php-reference/content.sha256) | SHA-256 of all 101 SQLite content files |
| [`fpm.conf`](../tools/php-reference/fpm.conf), [`php.ini`](../tools/php-reference/php.ini) | FPM pool and explicit UTC timezone |
| [`nginx.conf`](../tools/php-reference/nginx.conf) | Local HTTP, application routing, static files, and gzip |
| [`verification.md`](../tools/php-reference/verification.md) | Recorded environment and check results |
| `tools/php-reference/.local/` | Ignored snapshot and local reports; never production data |

## Start and check

Run from the repository root. Start Docker first. Port 10056 must be free.

```sh
python3 tools/php-reference/reference.py prepare
docker compose -f tools/php-reference/compose.yml pull
docker compose -f tools/php-reference/compose.yml up -d
python3 tools/php-reference/reference.py check
```

Run `prepare` once. It exports only committed `www` files from revision `ebfbcd85cd01cf8c62f0fb59cf7533f5befc85c3`, checks the content hashes, and records all source file hashes in `.local/snapshot.json`. It does not copy the working tree, real startup/feedback databases, or compiled templates. It refuses to overwrite an existing snapshot. Later starts reuse this snapshot even when the checkout changes. Changing the reference source or content requires an explicit review of the revision and checksum file.

The endpoint is `http://127.0.0.1:10056`. The checker uses this fixed address and sends `Host: meebible.org`. It does not follow redirects, execute page scripts, submit payment forms, or contact production. It writes one synthetic startup row and one synthetic feedback row per successful run. Repeated checks add rows; reset before collecting references that depend on an empty database.

A passing check prints `PASS: 20 HTTP checks` and saves `.local/verification.json`. This report includes image identities, configuration hashes, PHP settings, status and headers, body hashes, row counts, and synthetic stored values. `.local/fpm-info.txt` contains FPM settings. These are setup checks, not the complete response fixtures or the external BASE_URL test suite from step 4.

## Isolation and temporary data

- nginx publishes only `127.0.0.1:10056`. FPM has no published port. Use a current Docker Engine; Docker documents a localhost publishing issue in versions before 28 in its [port publishing guide](https://docs.docker.com/engine/network/port-publishing/).
- PHP joins only an `internal: true` network, with no default IPv4 route. nginx also joins a separate bridge because Docker 29 did not publish the port on an internal-only network during the check. nginx can make outbound connections; PHP cannot route through that bridge. There is no public reverse proxy, `VIRTUAL_HOST`, host networking, Docker socket, or other service connection.
- Both processes run as non-root users, with a read-only root filesystem, all capabilities dropped, and no new privileges. All host mounts are read-only. Only the exported `www` directory and specific configuration files are mounted, not the repository or its parent directories.
- Each container has a 256 MiB memory limit, one CPU, and a 64-process limit. Logs use two rotating files of up to 5 MiB each. There is no automatic restart policy.
- `/www/startup.sqlite` and `/www/feedback.sqlite` are links to `/data/` inside PHP. `/data` is a private 32 MiB tmpfs. SQLite creates its files and journals there. The handlers create the original schemas and the check confirms committed writes through these links.
- Smarty compile/cache directories and `/tmp` also use bounded tmpfs mounts. Code and content cannot be written. The check verifies mount settings, attempts writes to code/content directories, checks the route table, and compares every snapshot file hash before and after requests.

Local users with Docker access can still inspect the containers. This is a local test environment, not a public deployment configuration.

## Runtime and HTTP settings

The image references and amd64 identities are recorded in [verification.md](../tools/php-reference/verification.md). PHP is 5.6.40, SQLite is 3.8.10.2, libxml is 2.9.4, and nginx is 1.25.4. SQLite3, DOM, and mbstring are already present in the official PHP image.

FPM runs in `/www` with five workers. There is no main php.ini. The only added PHP setting is `date.timezone = UTC`. Without it, Smarty's first template compilation emits a timezone warning before response headers. In the initial check this changed the first 404 to a 200. Setting UTC explicitly makes PHP's observed fallback timezone stable and removes that configuration warning. This is a documented reference setting, not a claim that production uses the same setting.

Other image defaults remain: `default_charset=UTF-8`, `display_errors=On`, `html_errors=On`, `log_errors=Off`, `output_buffering=0`, `variables_order=EGPCS`, and no explicit `request_order`. `post_max_size=8M`, `max_input_vars=1000`, and PHP zlib output compression is off. Error and omitted-field details still need the step 4 captures; do not hide PHP diagnostics by turning them off during reference collection.

nginx follows the repository's application FastCGI parameters, `/static/` alias, favicon path, and gzip settings. It uses a local port and container logs instead of the deployment paths. It includes the pinned image's MIME table and has one worker. There is no outer proxy, HTTPS redirect, or extra application route. Kernel AIO was unavailable under amd64 emulation and nginx logged `io_setup() failed`; normal HTTP, static files, and gzip still passed. No AIO option is enabled in the configuration.

## Stop, reset, and inspect

```sh
docker compose -f tools/php-reference/compose.yml logs --tail 50
docker compose -f tools/php-reference/compose.yml ps -a
docker compose -f tools/php-reference/compose.yml down
```

`down` removes only this reference project's containers and networks. It discards user databases, Smarty output, temporary files, and container logs. It leaves the images, fixed host snapshot, and local reports. To start from empty user databases and a cold Smarty cache, run `up -d` and `check` again. A stop or restart can also discard tmpfs data; never use this setup to store real user records.

The full `down` → `up -d` → `check` cycle was tested. Both database counts returned to zero, and all 20 response statuses and decoded body hashes matched the first run. The server was stopped after setup verification. The [external suite](backend-tests.md) now uses saved local responses and a complete chapter hash index. It has separate profiles for the old PHP observations and the future Python contract.
