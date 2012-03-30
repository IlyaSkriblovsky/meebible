DEFINES += SQLITE_ENABLE_FTS3 SQLITE_ENABLE_FTS3_PARENTHESIS

HEADERS +=                      \
    src/sqlite3/sqlite3.h       \
    src/sqlite3/sqlite3ext.h    \
    src/sqlite3/fts3_tokenizer.h
SOURCES += src/sqlite3/sqlite3.c
