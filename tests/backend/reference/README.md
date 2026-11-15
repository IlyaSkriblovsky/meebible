# Local PHP reference evidence

These files come from the isolated PHP 5.6.40 server and the fixed SQLite snapshot at revision `ebfbcd85cd01cf8c62f0fb59cf7533f5befc85c3`. They contain only synthetic form submissions. No production responses or real user databases were used.

`manifest.json` records the environment, 95 request/response cases, all 94 translation XML responses, form write results, static asset hashes, and hashes of the saved files. `bodies/` contains decoded response bodies and compressed entities for gzip samples. Original status and headers are retained, including Date and diagnostic responses. The manifest also records the temporary CPU increase used during local verification.

`chapters.json.gz` indexes all 109,505 advertised chapter paths, byte counts, and SHA-256 values from the fixed SQLite text. The complete chapter bodies remain in the original content snapshot. Full mode verifies every indexed body over HTTP, including all `nwt` languages.

Some PHP cases return 200 with warnings or write bad values. The Python profile has explicit rules for those cases in `../cases.py`. The old response is evidence of a bug, not a requirement to preserve it. The literal-plus rule is a deliberate Python fix agreed with the owner.

Capture new evidence to a new directory with `../capture.py` and review it explicitly. Test failures never replace these files. See the [test guide](../../../docs/backend-tests.md) and [verification results](../verification.md).
