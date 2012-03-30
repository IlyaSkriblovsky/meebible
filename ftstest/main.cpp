#include <cstring>
#include <cstdio>

#include "sqlite3.h"
#include "tokenizer.h"


int callback(void *user_data, int nCols, char *text[], char *name[])
{
    for (int i = 0; i < nCols; i++)
        printf("%s=%s\t", name[i], text[i]);
    printf("\n");
    return 0;
}

int main()
{
    sqlite3 *db;
    sqlite3_open("test.sqlite", &db);

    if (db == 0)
    {
        fprintf(stderr, "Cannot open db\n");
        return 1;
    }

    install_unisimple_tokenizer(db);

    // sqlite3_exec(db, "CREATE VIRTUAL TABLE text USING fts4(tokenize=unisimple);", 0, 0, 0);
    // sqlite3_exec(db, "INSERT INTO text (content) VALUES ('<div>Öö Jêsūs ßß ss Ёлки зелё<div>ные')", 0, 0, 0);
    // printf("\nall:\n");
    // sqlite3_exec(db, "SELECT * FROM text", callback, 0, 0);
    // printf("\nmatch:\n");
    // sqlite3_exec(db, "SELECT * FROM text WHERE content MATCH 'jesus'", callback, 0, 0);

    const char *transCodes[] = { /*"ara",*/ "bbg", "blv", "esv", "frs", "kjv", "niv", "pal", "rus", "rvr", "svf" };

    sqlite3_exec(db, "CREATE VIRTUAL TABLE text USING fts4 (transCode, langCode, bookCode, chapterNo, content, tokenize=unisimple)", 0, 0, 0);

    for (unsigned int i = 0; i < sizeof(transCodes) / sizeof(transCodes[0]); i++)
    {
        char *err;
        printf("%s\n", transCodes[i]);
        char buf[1024];

        snprintf(buf, 1024, "ATTACH '%s.sqlite' AS '%s'", transCodes[i], transCodes[i]);
        sqlite3_exec(db, buf, 0, 0, &err);
        if (err != 0) fprintf(stderr, "attach err: %s\n", err);

        snprintf(buf, 1024, "INSERT INTO text SELECT '%s', langCode, bookCode, chapterNo, html FROM %s.html", transCodes[i], transCodes[i]);
        //snprintf(buf, 1024, "INSERT INTO text SELECT '%s', langCode, bookCode, chapterNo, html FROM %s.html where bookCode='ge' and chapterNo=40", transCodes[i], transCodes[i]);
        sqlite3_exec(db, buf, 0, 0, &err);
        if (err != 0) fprintf(stderr, "insert err: %s\n", err);

        snprintf(buf, 1024, "DETACH '%s'", transCodes[i]);
        sqlite3_exec(db, buf, 0, 0, &err);
        if (err != 0) fprintf(stderr, "detach err: %s\n", err);
    }

    printf("done\n");

    // sqlite3_exec(db, "SELECT transCode, bookCode, chapterNo FROM text WHERE content MATCH 'UPPHoJD*' ORDER BY transCode,bookCode, chapterNo", callback, 0, 0);
    sqlite3_exec(db, "SELECT count(*) FROM text WHERE content MATCH 'UPPHoJD*' ORDER BY transCode,bookCode, chapterNo", callback, 0, 0);

    sqlite3_close(db);
}
