# Production comparison — 2026-11-07

Migration step 5 is complete. A small safe GET sample matches the saved local PHP reference except for one explained home-page link and deployment headers. This is supplementary evidence, not a replacement for trusted local references or the full local Python tests.

## Scope and evidence

Ran 17 sequential GET requests to plain `http://meebible.org` between 08:47:53 and 08:48:12 UTC (10:47:53–10:48:12 Cyprus time). Requests were separated by a one-second pause; concurrency was one. Redirects were recorded without following them. No retries, POST, malformed input, feedback-reading routes, user databases, or full production downloads were used.

The sample includes home (identity/gzip), privacy, catalog (identity/gzip), KJV/NWT translation XML, English/Russian/Arabic/NWT chapters, gzip chapter content, a passage, a known verse-normalization redirect, CSS, favicon, and a 64-byte SIS range.

Source baseline: `96f5ac94076bcbb427f7bb45856eb9b1079df58e`. The trusted local reference manifest is unchanged; its SHA-256 is recorded in the production report. Capture and comparison use only Python's standard library.

Git retains the compact [`report.json`](../tests/backend/production/2026-11-07/report.json) with original response headers, sizes, body hashes, and per-header differences, plus [`home.diff`](../tests/backend/production/2026-11-07/home.diff). Full wire entities and decoded bodies are kept locally in `tests/backend/.local/production-2026-11-07/`, excluded from Git. They contain only public content and are not used to regenerate local fixtures.

## Results and classification

| Check | Result | Classification and implication |
| --- | --- | --- |
| HTTP statuses | All 17 match the reference, including 301 normalization and 206 range | No status-contract difference in this sample |
| Decoded bodies | 15 match byte for byte | Catalog, sampled translations/chapters, passage, redirect body, privacy, and static/range content agree |
| Home, identity and gzip | Each differs by one byte | Production uses `https://code.jquery.com/jquery.min.js`; the local PHP reference uses `http://...`. This is a deployed page-version difference, not a Bible data or compression difference. The complete diff is saved in `home.diff` |
| Gzip | All three requested gzip responses are compressed and decode correctly | Production and reference agree on the sampled compression behavior; compressed bytes need not be identical |
| CORS | Dynamic responses include `Access-Control-Allow-Origin: *`; sampled static responses do not | Deployment configuration differs from the isolated reference. The owner previously reported a CORS-header difference. HTTP alone cannot identify which production layer adds it. Native Qt clients do not require browser CORS support |
| Server header | `nginx/1.31.0` instead of reference `nginx/1.25.4` | Runtime/infrastructure version difference; not a client contract requirement |
| Date | Capture dates differ | Expected changing values |
| Static Last-Modified and ETag | Production timestamps are in March/April 2024; reference snapshot timestamps are from October 2026 | Deployment file metadata differs, while response bytes agree. Each observed ETag encodes the matching Last-Modified timestamp and full asset size; the SIS range retains `bytes 0-63/281860` |
| Other headers | Match in this sample, apart from home identity Content-Length increasing by one | Content-Type/charset, Content-Encoding, Location, Content-Range, Cache-Control, and Vary show no additional difference. Content-Length was checked against each received wire entity |

There is no unexplained sampled status/content mismatch. The exact CORS insertion layer is unverified; this does not indicate a chapter/catalog data difference. No conclusion about all production content, public write behavior, server internals, or real-phone display follows from 17 requests.

## Migration decisions

Keep the trusted PHP fixtures and the Python HTTP suite unchanged. Do not copy public responses into them automatically. The single home-page script URL difference is recorded; this comparison does not modify the Python templates. During release, retain or deliberately change the existing outer proxy's CORS policy rather than attributing it to application behavior. The external proxy version is now observed from HTTP; final host configuration still needs a release review.

Next is step 10: prepare and rehearse backup, consistent user-data transfer, restore/recovery, and release commands. Production switching is a separate step. No production settings or data were changed by this comparison.

## Repeating the small comparison

Only run this explicit production command when a fresh public comparison is intended. Use a new output directory:

```sh
python3 tests/backend/compare_production.py --output tests/backend/.local/production-NEW-DATE
```

The target and 17 safe GET paths are fixed in the script. It refuses an existing output directory, checks the selected local fixture hashes before network access, does not follow redirects, and stops on a 5xx without retries. It is separate from `run.py`, whose loopback-only restriction is unchanged.

Offline tool checks (no public requests):

```sh
python3 -m unittest discover -s tests/backend -p test_production_compare.py -v
```

All three checks passed: fixed GET-only capture without following even an external redirect; refusal to overwrite an existing capture before network access; stop on the first 5xx without retry. Ruff lint and formatting passed for both new files.

## Follow-up: HTTPS jQuery URL

On 2026-11-07, the owner confirmed that production has an uncommitted HTTPS jQuery URL fix: browsers block the HTTP script when the page is opened over HTTPS. Updated `backend/templates/index.html` to use `https://code.jquery.com/jquery.min.js` as well. This keeps the dependency loadable on both HTTP and HTTPS pages. The historical PHP reference and captured production evidence remain unchanged. A targeted rendered-home check and the 99 unit/API cases passed after the one-line template change.
