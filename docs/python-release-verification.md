# Release rehearsal — 2026-11-07

Migration step 10 is complete as local release preparation. The owner confirmed an x86_64 production server and old database paths `www/startup.sqlite` and `www/feedback.sqlite`. No production service or real user database participated in the rehearsal. Step 11 (the live switch) has not been performed.

## Candidate and environment

Application source: `3384603`, including the HTTPS jQuery fix. Release tools and Compose image selection are the accompanying working changes. The target candidate was built with `docker build --platform linux/amd64 -t meebible-python:release-rehearsal-3384603-amd64 .` and run under Docker Desktop emulation on the ARM host.

- Image: `meebible-python:release-rehearsal-3384603-amd64`.
- Recorded image ID: `sha256:e6fc807c7c509148acbdf5a8eca1c3bb5d1e44dc5b4c5293ab01f7d4b4c568ac`.
- Inspected platform: `linux/amd64`.
- Project/port: `meebible-release-rehearsal`, `127.0.0.1:10059`.
- Data and reports: ignored `tests/backend/.local/release-rehearsal-2026-11-07/`.
- Startup/recovery selected the image from a private env file and used `--no-build --pull never --wait`; failed-update recovery also used `--force-recreate`.

Only a small human-readable summary is kept in Git. Databases, full HTTP logs/reports, and failed data remain in the ignored rehearsal directory. A temporary source/content archive was verified against all 101 tracked content-database hashes, excluded user databases, and was then removed to avoid retaining a large duplicate. Production backups must follow the release guide and retention policy.

## Transfer and backup checks

`make_rehearsal_data.py` creates three synthetic records per database using the schemas saved from PHP, including Unicode, plus signs, and NULL optional startup values. Backed up this old-schema data with the new CLI, verified the bundle, and restored into a new private directory. Existing output directories are refused; no original source is overwritten.

An initial hand-written rehearsal fixture omitted SQL column types. The HTTP suite correctly rejected that fixture's schema. Replaced it with the generator based on the captured PHP schema and added pre-backup type validation plus a regression check. The aborted/failed fixture diagnostics are retained locally as `fixture-error-*`; they are not reported as successful candidate validation.

Six offline regression checks passed:

1. Committed WAL records survive backup and exact restoration without the original source; the backup is sealed without WAL sidecars.
2. The operator must assert writers are stopped; absent source databases are not created silently.
3. Existing backup/restore destinations remain unchanged.
4. Corrupt or incomplete bundles cannot create a restore destination.
5. Changed values/hashes and unexpected WAL sidecars are rejected.
6. Wrong legacy column types are rejected before creating a backup directory.

The tools return only schema names, row counts, and hashes; they do not print user records. Files are mode 600 and backup/staging directories 700. Linux production ownership must be set to UID/GID 10001 before mounting the restored directory; host permissions and live administration are checked at cutover.

## Target-architecture HTTP validation

On the corrected transferred data and the exact amd64 candidate:

| Run | Result |
| --- | --- |
| Quick | 93 passed, zero failed/skipped, 1.66 seconds |
| Full | 109,718 passed, zero failed/skipped, 323.66 seconds |
| Chapter coverage | All 109,505 advertised chapters, 20 workers |
| Implementation suite | 99 pytest cases passed on the host |
| Release-tool suite | Six offline unittest checks passed |
| Lint/format | Passed for release tools |

Quick/full included actual POST/storage observations and preserved all original three records in each database. This architecture run was justified by the new amd64 binary/runtime build; it does not replace earlier native ARM failure/proxy checks. Emulated timing is not a production capacity measurement. No broad test suite was sent to the public server. Existing default local quick/full report files were restored after retaining the rehearsal-specific copies.

## Failed update and recovery

After the tests, the databases contained 11 startup rows and 15 feedback rows, including all three original rows in each. Stopped the test writer and made/verified a latest-state backup.

A test-only Compose override replaced the command with `python -c 'raise SystemExit(42)'` and disabled restart. The release `up --wait` command returned failure, and container inspection confirmed `exited 42`. This simulated a failed startup without switching public traffic or building a second application implementation.

Preserved the failed directory, deliberately corrupted its feedback database, restored the latest verified bundle into a new directory at the original bind path, removed the bad-command override, and recreated the known-good amd64 service. It became healthy. Checks confirmed:

- Exactly 11 startup and 15 feedback rows remained; accepted post-transfer records were retained.
- Both restored database hashes matched the latest backup manifest.
- `/meta` returned 200 and the home page included the HTTPS jQuery URL.
- The test container/network were removed afterward; report/data evidence stayed local.

This tests both failed-start detection and recovery without rolling back acknowledged writes to a stale pre-transfer snapshot. It does not claim that the live external proxy was reloaded or that a real phone has been checked.

## Remaining live work

Before step 11, record the actual old PHP container identifier, live proxy config path and validation/reload method, old listening port/internal nginx container, selected free new loopback port, production image ID, and maintenance procedure. Commit the reviewed release tooling/configuration before building the final production artifact. Use the [release guide](python-release.md) to take a final stopped-writer backup from `www/`, restore to `data/`, recreate the selected Python image, and switch the existing external proxy. Verify live availability, real submissions, and a client only during that authorized cutover.
