# External test verification

Checked on 2026-10-24. Result: **PASS for the local PHP profile**. The Python service is being written by the owner;
these results do not claim that it passes yet.

| Check                               | Result                                                                                                                                    |
|-------------------------------------|-------------------------------------------------------------------------------------------------------------------------------------------|
| Final quick suite                   | 104 passed, zero failed/skipped; 11.17 seconds                                                                                            |
| Complete chapter sweep              | 109,505 of 109,505 passed, with 20 concurrent requests                                                                                    |
| Translation XML in full mode        | All 94 translation/language pairs passed                                                                                                  |
| Static assets in full mode          | All 31 indexed paths passed, including both phone packages                                                                                |
| Full run including its quick checks | 109,727 passed, zero failed/skipped; 1420 seconds                                                                                         |
| Tool checks without a server        | Nine passed, including deliberate bad bytes/XML, public URLs, JSON errors, plus conversion, and a rejected request that still wrote a row |
| Final PHP setup/isolation check     | 20 passed; every source/content checksum unchanged                                                                                        |
| Cleanup                             | PHP CPU limit restored to one; containers and temporary databases removed                                                                 |

The full content run began against the first capture. The final capture adds a real split-verse passage and a form with
a non-address email value, both covered by the final quick run. All earlier decoded response bodies and the complete
chapter index stayed byte-identical. The full-run and final manifests therefore have different hashes; the full content
oracle is unchanged.

Final reference manifest SHA-256:

```text
50c2e2efe54e1503a1ff9929896e9caf5b4e553e992c36d4b2790dbaff22bd69
```

Complete chapter index SHA-256, identical for the full sweep and final quick run:

```text
6a73901fcb711ee9facf471bb4d486f4cc946de778e72e3cff3d66a562ea97b9
```

The full run started with the normal one-CPU PHP limit. During the sweep it was raised to two, the maximum available in
this Docker VM. The final quick and isolation checks used the restored one-CPU limit. All other container settings
stayed as described in the [PHP setup](../../docs/php-reference.md). Original resource notes are in the capture
manifest.

Local detailed reports and compressed response-header logs are in `.local/`, which is ignored by Git. The reports
contain only local/synthetic test values. The references identify the exact old source, images, PHP defaults, and
content checksums. The same suite must still pass on the owner's final Python server and deployment settings. No
production requests or real-phone display tests were made.

## 2026-11-05 — Python full-run connection reuse

The owner's original full report stopped after 16,000 chapter checks: 15,934 chapter responses had passed before
connection errors began. This was a transport failure, not a chapter hash mismatch. The first failed chapter returned
200 when requested separately.

The old test client opened and closed a TCP connection for every chapter. On this Mac the ephemeral port range is
49152–65535 (16,384 ports). A separate reproduction against a temporary Uvicorn server without reload failed after
16,250 successful chapter requests with `OSError: [Errno 49] Can't assign requested address`. The server stayed alive
and its error log was empty. The original `Connection refused` log alone does not establish whether that run also
encountered a server stop or reload; the port-exhaustion defect in the client was reproduced independently.

Full chapter requests now reuse one HTTP connection per worker thread, with at most 20 concurrent requests. Connections
are not shared between threads and are closed after the executor finishes or a request fails. There are no automatic
retries. The ordinary request helper and byte/hash assertions keep their existing behavior. The report now records the
worker count.

Validation after the fix:

- All 21 test-tool checks passed, including per-thread reuse, cleanup, failure propagation without retries, and
  rejection of public targets.
- The owner restarted the development server without `--reload`. The full suite then ran against `http://localhost:8000`
  with 20 workers and `--read-only`.
- All 109,505 chapter bodies matched their saved byte counts and SHA-256 values. The run passed 109,700 checks with zero
  failures in 27.73 seconds, including all translation XML and static assets.
- The report says `partial` because 18 POST/write-method checks were deliberately skipped. User databases were not
  changed. This was not validation of the final Docker/nginx environment.
- The original failing `.local/full-python.json` was preserved. The new report and compressed response evidence are
  `.local/full-client-diagnostic.json` and `.local/full-client-diagnostic-responses.jsonl.gz`.

Backend implementation and saved PHP references were not changed by this investigation. Further full runs should use a
stable server without development reload.

## Python container checks, 2026-11-05

The root Dockerfile, Compose project, and nginx configuration now run Python. Validation used Docker Engine 29.9.2 on Docker Desktop, native linux/arm64, project `meebible-python-check`, and loopback port 10057. Python 3.12.12, uv 0.9.10, and nginx 1.28.0 image indexes are pinned in the source files. The tested local application image ID was `sha256:4e9e9507828ce3e649a5071fa5c3a63f8a7dbd85e4c55fc32b3860ec7d4b7b9d`.

Commands followed the [Python guide](../../docs/python-backend.md#local-startup), using `-p meebible-python-check` and `MEEBIBLE_PORT=10057`. First, the default named volume received one synthetic startup row and one feedback row through nginx. Both rows survived restart and forced backend recreation. The application ran as UID 10001 and rejected writes under `/app` and `/app/backend/content`; no user databases or implementation tests were present under `/app`. Logs showed graceful application shutdown on SIGTERM.

For the external suite, a local override bound `.local/container-data/` at `/data` so the independent SQLite observer could inspect commits. No production or existing local user databases were mounted. Ran:

```sh
BASE_URL=http://127.0.0.1:10057 USER_DATA_DIR="$PWD/tests/backend/.local/container-data" python3 tests/backend/run.py quick
BASE_URL=http://127.0.0.1:10057 USER_DATA_DIR="$PWD/tests/backend/.local/container-data" python3 tests/backend/run.py full
```

- Quick: 93 passed, zero failures or skips, 0.55 seconds.
- Full: 109,718 passed, zero failures or skips, 44.61 seconds; all 109,505 chapters matched saved sizes/hashes, with 20 workers. Both runs enabled POST/storage checks and left optional checks disabled.
- Separate HTTP requests through nginx checked static ETag/304 and HEAD. The ordinary suite also checked gzip, byte ranges, canonical HTTP routes, errors, XML/catalog flow, and static asset bytes.
- All 93 implementation pytest cases passed; Ruff lint and format checks passed.

The first quick container run found Debian's different favicon MIME mapping; the application now explicitly returns the saved `image/x-icon` type. The first full container run stopped at 28,000 chapters with nginx `Cannot assign requested address` errors: the proxy opened one upstream TCP connection per request. A shared upstream with keepalive and dynamic DNS resolution fixed this; response assertions and concurrency were unchanged. The failed report is retained at `.local/container-full-before-keepalive.json`.

Final reports are `.local/container-quick-python.json` and `.local/container-full-python.json`, with console logs at `.local/container-quick.log` and `.local/container-full.log`. Previous top-level quick/full reports were preserved before testing and restored afterward. Source references and the isolated PHP project were unchanged.

These checks complete migration step 8 and part of step 9. Container failure injection for database locks/read-only storage/write failures, input/resource-limit review, production comparison, backup/restore rehearsal, and real-client checks remain outside this result. Only native arm64 was run; deployment architecture validation remains part of release preparation.

## Direct Python container checks, 2026-11-05

After the owner confirmed an existing external reverse proxy, removed nginx and the extra networks from root Compose. Tested only the backend, publishing `127.0.0.1:10057:8000`, with project `meebible-direct-check` and the same disposable `.local/container-data` bind override. The image was unchanged. Compose config lists exactly one service (`backend`), and its healthcheck passed.

The same quick/full commands above passed directly against Uvicorn: quick 93 checks; full 109,718 checks, all 109,505 chapters, 20 workers, 43.51 seconds. Neither run had failures or skips; POST/storage checks remained enabled. Results and console logs are `.local/direct-quick-python.json`, `.local/direct-full-python.json`, `.local/direct-quick.log`, and `.local/direct-full.log`. Previous top-level quick/full reports were restored. The server's external proxy was not part of this local test; ingress limits, forwarding, and real deployment behavior remain release checks.
