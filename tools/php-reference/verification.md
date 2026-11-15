# PHP reference setup verification

Checked on 2026-10-24 at 13:17 UTC. Result: **PASS**.

This records migration step 3. It is a setup smoke check, not the full reference response corpus, full-content check, or production comparison. See the [operating guide](../../docs/php-reference.md) for commands.

## Environment

- Host: macOS arm64, Docker Desktop 4.93.0.
- Docker Engine: 29.8.1; Compose: 5.5.1; Python: 3.12.7.
- Both images: linux/amd64, running with emulation on this host.
- PHP: 5.6.40; SQLite: 3.8.10.2; libxml: 2.9.4; nginx: 1.25.4.
- Required extensions present: SQLite3, DOM, mbstring. No extensions were built or installed.

Pinned official image references:

```text
php@sha256:4f070f1b7b93cc5ab364839b79a5b26f38d5f89461f7bc0cd4bab4a3ad7d67d7
nginx@sha256:9ff236ed47fe39cf1f0acf349d0e5137f8b8a6fd0b46e5117a401010e56222e1
```

The amd64 identities reported by `docker image inspect --platform linux/amd64`:

```text
PHP:   sha256:7cfd6ccb875ff314b2c672b62aa3169fe79d54bf3422b7b0017955b3457ca1a7
nginx: sha256:cd64407576751d9b9ba4924f758d3d39fe76a6e142c32169625b60934c95f057
```

The application snapshot is from Git revision `ebfbcd85cd01cf8c62f0fb59cf7533f5befc85c3`. It contains 290 regular files, including 101 SQLite content files. The full source hash map is saved locally as `.local/snapshot.json`. Individual database hashes are committed in [content.sha256](content.sha256); that manifest's SHA-256 is `e53a81911dc471dd3d2f1c203bb481fc1875b70d60160218222776aaf4046d5d`.

Configuration and checker hashes used for this run:

```text
9715fd585472851b29556d5d4983ab10d7cfb75df464490b70387f7eaa8f49d8  compose.yml
d82e6bf9b2af460755c5b72f48bf7b540c81e34e93691c44c3037810d00dcb2e  fpm.conf
e695d9408206f677ae286b010500b92af083a8420d1b7688145a10a7b0480fa8  php.ini
717200d3370ce596d05c60763e13c8a3de713a2b2c1b81422c9c2fa36137061f  nginx.conf
8b2d3e9cb9c37d0c788a4f9dc27a1ebe3a93494f7eb8d0a2b42076b6295c4c83  reference.py
```

FPM uses `/www` and five workers. The image has no main php.ini. The only added PHP setting is `date.timezone=UTC`, read from `conf.d/zz-reference.ini`. Without this setting, a cold Smarty compile produced a timezone warning before headers, including a wrong 200 status for the first 404. The checked configuration passes from a cold cache. Displayed errors remain enabled. These are local settings; production settings remain unverified.

## Checks and results

| Check | Result |
| --- | --- |
| Metadata and translation XML | 200, parsed successfully |
| Metadata identity and gzip | Same 16,698 decoded bytes and SHA-256 |
| Four sample chapters | Byte-for-byte match with the pinned SQLite rows |
| Home, privacy, both donation pages, passage | 200, HTML without a warning prefix |
| CSS and favicon | 200 |
| Unknown application URL | 404 with HTML, including a cold Smarty cache |
| Missing chapter parameters | 400, `Not all parameters specified` |
| Passage normalization | 301 with the expected Location; no redirect followed |
| HEAD metadata | 200 with no body |
| Startup and feedback POST | 200, two-byte `ok`; one committed row each |
| Raw plus in feedback email | Stored as a space, confirming the PHP form decoding bug |
| Response length | Content-Length agrees with the received representation where present |
| Root filesystems and host binds | Read-only; expected non-root users, capabilities dropped, no new privileges |
| PHP network | Internal only; no published port or default IPv4 route |
| nginx port | Actual Docker binding is only `127.0.0.1:10056` |
| Code/content writes | Blocked; content directory cannot create a missing database |
| Temporary storage | Data and Smarty compile directories writable inside tmpfs |
| Snapshot integrity | All 290 file hashes unchanged after requests |

Each successful run makes 20 checked HTTP requests, plus safe metadata readiness probes. Sample chapter details:

| Translation/language, Genesis 1 | Bytes | Body SHA-256 |
| --- | --- | --- |
| `kjv/e` | 7125 | `4f6267f404e07b9e326619c7d76986755e0b017a66e58749e25448d99f477e8f` |
| `rus/u` | 8683 | `5bae7b43ae05901dea45d7d850f7c14f0cb4b8ad000ffa1eab96e97772e7bafc` |
| `ara/a` | 9933 | `93c01e4b3201dcad4999f3beee64261a1456b0a937511a8f74de2103943cf1bf` |
| `nwt/e` | 8010 | `a79e5aff219213acada33229c18335cb92247fa58c3989baf69088e155644372` |

Tested the documented `prepare`, `pull`, `up -d`, `check`, `logs`, `ps -a`, and `down` commands. A complete `down` → `up -d` reset removed both synthetic user databases and the Smarty cache. Counts changed from one row per table to zero, then back to one after another successful check. All 20 response statuses and decoded body hashes matched across the cold starts. Changing Date headers and compressed wire bytes were not required to match.

Raw local details are in ignored `.local/verification.json`, `.local/fpm-info.txt`, and `.local/lifecycle.json`. These contain only synthetic user values. The containers were removed with `down` after verification; the fixed snapshot and images remain available for step 4. No production requests were made.
