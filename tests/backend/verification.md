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
