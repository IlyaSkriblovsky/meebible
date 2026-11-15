# Python release and recovery

This guide prepares migration steps 10 and 11. Only the local rehearsal has been performed. No production service, proxy, or user database has been changed.

Run commands from the project root. The owner confirmed an x86_64 server (`linux/amd64`) and the old user databases at `www/startup.sqlite` and `www/feedback.sqlite`. The Python service uses `data/`. Host Python 3.11+ and Docker Compose v2 are required for the commands below. Record the actual old PHP/internal-nginx container names, current loopback listener, and external proxy's live config/reload mechanism before the maintenance window; those are installation-specific and cannot be inferred from the repository.

## Build and identify a candidate

Commit the reviewed release files first. Build an image for the server architecture under a unique tag; do not reuse that tag for another build:

```sh
MEEBIBLE_RELEASE=$(git rev-parse --short=12 HEAD)
MEEBIBLE_IMAGE="meebible-python:release-$MEEBIBLE_RELEASE"
docker build --platform linux/amd64 -t "$MEEBIBLE_IMAGE" .
docker image inspect "$MEEBIBLE_IMAGE" --format '{{.Id}} {{.Os}}/{{.Architecture}}'
```

Record the full Git revision, image ID, platform, `uv.lock` hash, and Docker/Compose versions with the release. Base Python/uv images are pinned by digest; runtime dependencies install from the lock. A versioned tag identifies the candidate, and the recorded image ID detects accidental retagging. Keep the previous working Python image. The root Compose image can be selected using `MEEBIBLE_IMAGE`; its default remains the local development image.

If building off-server, transfer the exact candidate with Docker save/load or a registry digest, then verify the image ID and architecture on the server. Do not rebuild after validation and assume the result is the same artifact.

Test the candidate with a disposable bind directory and an alternate loopback port, following the [external test guide](backend-tests.md). Run quick/full on the candidate before the maintenance window. On an ARM development host, use a temporary Compose override with `platform: linux/amd64` for emulated validation. The rehearsal below validates the amd64 candidate under emulation; native server timing and ingress checks still belong to cutover.

## Preserve code, content, and configuration

Use a private backup directory outside the served tree. The root `backups/` directory is excluded from Git and the Docker context. Keep an independent off-host copy according to the server's existing backup process.

```sh
install -d -m 0700 backups
MEEBIBLE_BACKUP="$PWD/backups/pre-python-$(date -u +%Y%m%dT%H%M%SZ)"
export MEEBIBLE_BACKUP
mkdir -m 0700 "$MEEBIBLE_BACKUP"
git rev-parse HEAD > "$MEEBIBLE_BACKUP/source-revision.txt"
git archive --format=tar HEAD > "$MEEBIBLE_BACKUP/source-and-content.tar"
cp docker-compose.yml proxy.conf uv.lock "$MEEBIBLE_BACKUP/"
docker image inspect "$MEEBIBLE_IMAGE" > "$MEEBIBLE_BACKUP/image.json"
```

The source archive includes the tracked Bible content. It excludes untracked production databases and uncommitted live proxy settings: back those up separately. Save the real external proxy configuration, including TLS/CORS settings and its current upstream, to the same private backup location. Do not replace it wholesale with the repository's example. No database backup should be placed under `www/` or made publicly downloadable.

## Consistent user-data backup and transfer

`tools/release/user_data.py` backs up the two databases using SQLite's backup API, including committed WAL pages. It validates integrity, column names, and legacy column types, seals each destination as one DELETE-journal SQLite file, and writes a manifest with row counts and SHA-256 hashes. Output files are private (600), directories 700, and no user values are printed.

The two files are copied sequentially, so stop **all writers** before taking a cutover backup. `--writers-stopped` is an operator assertion, not an automatic lock on other services. The tool deliberately refuses missing databases rather than silently treating missing production data as empty. A failed backup has no completion manifest; verify it before transfer. Backup and restore both require new output directories.

For the cutover window (step 11, not executed as part of preparation):

1. Put the external proxy into the site's maintenance procedure so new writes cannot reach PHP. Wait for in-flight requests to finish. Stop the identified old PHP container and any other process that writes these files. Do not stop an arbitrary container based on an assumed name. Keep maintenance active until the Python health/read checks succeed.
2. Take the final consistent backup from `www/`:

```sh
python3 tools/release/user_data.py backup --source www --output "$MEEBIBLE_BACKUP/user-data" --writers-stopped
python3 tools/release/user_data.py verify --source "$MEEBIBLE_BACKUP/user-data"
python3 tools/release/user_data.py restore --source "$MEEBIBLE_BACKUP/user-data" --output data.next
sudo chown -R 10001:10001 data.next
```

3. With Python also stopped, preserve any existing `data/` under a unique maintenance name and rename `data.next` to `data`. Do not overwrite an existing live directory or copy SQLite files over a running service. Recreating the container is required after changing a bind source directory: a simple restart may retain the old directory mount.
4. Keep the old `www/` files and the final backup private and unchanged. Compare backup manifest row counts and restored file hashes before opening traffic. No schema migration is needed.

A backup directory with a manifest is self-contained: restore rejects WAL/journal sidecars, damaged databases, wrong schemas, changed values/hashes, and incomplete manifests. It restores into a new directory, never over `data/`. Use this staging approach for routine restores as well. File hashes detect accidental corruption; they are not a signature against a hostile party that can rewrite the manifest.

## Start and switch

For the first cutover, use a verified free loopback port (10002 below), because the old internal nginx may still own 10001. Check listeners with the server's `ss -ltnp` or `lsof -iTCP -sTCP:LISTEN` before choosing it. This allows the proxy switch to be explicit and avoids a bind collision. The default development port stays 10001. Persist the reviewed image tag and chosen port in a private environment file; keep the image ID recorded with the release:

```sh
printf 'MEEBIBLE_IMAGE=%s\nMEEBIBLE_PORT=10002\n' "$MEEBIBLE_IMAGE" > .env.release
chmod 0600 .env.release
unset MEEBIBLE_IMAGE MEEBIBLE_PORT
docker compose --env-file .env.release config --quiet
docker compose --env-file .env.release up -d --no-build --pull never --force-recreate --wait
curl --fail http://127.0.0.1:10002/meta
docker compose --env-file .env.release logs --tail 50 backend
```

`.env.release` is excluded from Git. The only published endpoint is loopback. Change the real external proxy upstream from its old listener to the chosen new endpoint (`127.0.0.1:10002` in this example) using its existing administration method, validate its configuration, then reload it. Preserve existing HTTPS handling and CORS behavior; keep plain HTTP available. Incorporate the prepared `proxy.conf` changes: upstream keepalive, HTTP/1.1 with an empty Connection header, and the explicit 1 MiB body limit. Do not add a second nginx to the root Compose project.

Before lifting maintenance, confirm read routes, privacy/home, a passage normalization redirect, static downloads, gzip, HEAD, and conditional/range responses at the loopback endpoint. After opening traffic, check the public HTTP URL, TLS browser loading of HTTPS jQuery, proxy/application logs, and requests from a real client. Confirm a known real startup/feedback submission appears once in SQLite. Do not point the quick/full runner or malformed-input checks at production. Keep PHP stopped, remove it from public routing, and stop the obsolete internal nginx after the new path is verified.

The upstream switch is a production action and has not been run in preparation. Record the actual proxy validation/reload command in the release record before performing it.

## Updates and failed releases

For a Python/base-image/dependency update, change pins/lock in a new commit, build a new unique tag, and repeat unit, quick/full, and container failure checks on disposable data. Never update packages inside a running container. Keep code/content immutable and user data in `data/`. Validate and record the candidate's exact image ID before changing `.env.release`.

Before changing `.env.release`, copy the current private file into the pre-update backup directory and record the running image ID; the new candidate record does not replace the previous working release record. Stop Python writers for a consistent pre-update backup. Then select the new image and recreate with `--no-build --pull never --wait`. If it fails readiness, keep maintenance enabled. Save logs and make another consistent backup of the latest user data before recovery. Keep the failed data directory for diagnosis.

If data/schema are unchanged and intact, restart the previous known-good **Python** image against the latest data. If data are damaged, restore the latest verified backup to a new directory, preserve the failed directory, set ownership, replace the bind source while the service is stopped, and recreate the previous good Python service. Verify all acknowledged records since transfer are retained. Restoring an earlier pre-cutover backup after Python has accepted writes would lose those writes; reconcile them first. Do not automatically reopen the old vulnerable PHP service as rollback.

A missing image with `--pull never`, a readiness failure, or a failed backup/verification is a stop condition: keep maintenance active and investigate rather than switching traffic.

## Logs and routine operations

Use `docker compose --env-file .env.release ps`, `logs --tail 100 backend`, and `restart backend`. Docker rotates stdout/stderr at 5 MiB with two files; external proxy retention is administered separately. A `/meta` healthcheck confirms content reads, not writable user storage. For 500 responses on POST, inspect permissions/ownership of `data/`, free space, SQLite lock holders, and logs; `ok` means commit completed. Never delete WAL/journal files to clear a lock. Take a new consistent backup before repairs.

For routine backups, stop writers, then run backup/verify with a fresh timestamped directory and restart the same image. On Linux, `data/` belongs to UID 10001 with mode 700: run these maintenance commands as that user or with `sudo python3 tools/release/user_data.py ...`, including verification/restoration of root-owned backup bundles. Do not loosen database permissions to make an operator command work. Retention and off-host copies follow the server's operational policy. Test restore periodically into a private disposable directory; never rehearse on live data.

## Local rehearsal evidence

See [release rehearsal](python-release-verification.md). The rehearsal uses synthetic old-schema records, transfers them to Python, commits new records, backs up the latest state, restores after corruption, validates the amd64 candidate, and recovers after a deliberately failed container update. No real user database or production service participates.

## Repeat the synthetic transfer exercise

The fixture generator creates exactly three rows in each database using the captured PHP schemas, including Unicode, literal plus signs, and optional NULL startup fields. It refuses an existing output directory. For example, after creating a fresh private parent directory:

```sh
mkdir -m 0700 backups/rehearsal
python3 tools/release/make_rehearsal_data.py --output backups/rehearsal/old
python3 tools/release/user_data.py backup --source backups/rehearsal/old --output backups/rehearsal/bundle --writers-stopped
python3 tools/release/user_data.py verify --source backups/rehearsal/bundle
python3 tools/release/user_data.py restore --source backups/rehearsal/bundle --output backups/rehearsal/data
```

Mount only this restored test directory into the candidate, use a different Compose project and loopback port, and set Linux ownership to 10001 before starting. Follow the existing disposable-bind instructions in the backend guide. Run quick/full with `USER_DATA_DIR` pointing at this directory. Stop the test service, back up its latest state, restore to a new directory, recreate the test container, and confirm the extra synthetic records remain. These are synthetic writes; never use the exercise's HTTP runner on production data.

Run the data-tool regression checks without Docker or network:

```sh
python3 -m unittest discover -s tools/release -p test_user_data.py -v
```
