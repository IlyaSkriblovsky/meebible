#include "ilyatokenizer.h"

#include <cstdio>
#include <cstring>

#include <unicode/unistr.h>
#include <unicode/uchar.h>
#include <unicode/translit.h>
#include <unicode/normlzr.h>


/*
** Register a tokenizer implementation with FTS3 or FTS4.
*/
int registerTokenizer(
  sqlite3 *db,
  const char *zName,
  const sqlite3_tokenizer_module *p
){
  int rc;
  sqlite3_stmt *pStmt;
  const char *zSql = "SELECT fts3_tokenizer(?, ?)";

  rc = sqlite3_prepare_v2(db, zSql, -1, &pStmt, 0);
  if( rc!=SQLITE_OK ){
    return rc;
  }

  sqlite3_bind_text(pStmt, 1, zName, -1, SQLITE_STATIC);
  sqlite3_bind_blob(pStmt, 2, &p, sizeof(p), SQLITE_STATIC);
  sqlite3_step(pStmt);

  return sqlite3_finalize(pStmt);
}

/*
** Query FTS for the tokenizer implementation named zName.
*/
int queryTokenizer(
  sqlite3 *db,
  const char *zName,
  const sqlite3_tokenizer_module **pp
){
  int rc;
  sqlite3_stmt *pStmt;
  const char *zSql = "SELECT fts3_tokenizer(?)";

  *pp = 0;
  rc = sqlite3_prepare_v2(db, zSql, -1, &pStmt, 0);
  if( rc!=SQLITE_OK ){
    return rc;
  }

  sqlite3_bind_text(pStmt, 1, zName, -1, SQLITE_STATIC);
  if( SQLITE_ROW==sqlite3_step(pStmt) ){
    if( sqlite3_column_type(pStmt, 0)==SQLITE_BLOB ){
      memcpy(pp, sqlite3_column_blob(pStmt, 0), sizeof(*pp));
    }
  }

  return sqlite3_finalize(pStmt);
}


int ilya_tokenizer_create(int argc, const char * const *argv, sqlite3_tokenizer **tokenizer);
int ilya_tokenizer_destroy(sqlite3_tokenizer *tokenizer);
int ilya_tokenizer_open(sqlite3_tokenizer *tokenizer, const char *input, int nBytes, sqlite3_tokenizer_cursor **cursor);
int ilya_tokenizer_close(sqlite3_tokenizer_cursor *cursor);
int ilya_tokenizer_next(sqlite3_tokenizer_cursor *cursor, const char **token, int *nBytes, int *startOffset, int *endOffset, int *position);


const sqlite3_tokenizer_module *icuDesc = 0;
sqlite3_tokenizer_module desc = {
    0,
    ilya_tokenizer_create,
    ilya_tokenizer_destroy,
    ilya_tokenizer_open,
    ilya_tokenizer_close,
    ilya_tokenizer_next
};


enum State
{
    SPACE,
    WORD,
    TAG
};

struct ilya_tokenizer: public sqlite3_tokenizer {
    sqlite3_tokenizer *icu;
};
struct ilya_tokenizer_cursor: public sqlite3_tokenizer_cursor {
    sqlite3_tokenizer_cursor *icu_cursor;

    State state;
    UChar token[100];
    unsigned char token_utf8[100];

    const char* input;
    int len;
    int pos;

    ilya_tokenizer_cursor(const char* input, int len)
        : state(SPACE), input(input), len(len), pos(0)
    {
        if (this->len == -1)
            this->len = strlen(input);
    }

    ~ilya_tokenizer_cursor() { }
};



void install_ilya_tokenizer(sqlite3 *db)
{
    if (icuDesc == 0)
    {
        int res = queryTokenizer(db, "icu", &icuDesc);
        if (icuDesc == 0)
            fprintf(stderr, "Cannot find icu tokenizer\n");

        registerTokenizer(db, "ilya", &desc);
    }
}



int ilya_tokenizer_create(int argc, const char * const *argv, sqlite3_tokenizer **tokenizer)
{
    ilya_tokenizer *t = new ilya_tokenizer;
    *tokenizer = t;
    return SQLITE_OK;
}

int ilya_tokenizer_destroy(sqlite3_tokenizer *tokenizer)
{
    ilya_tokenizer *t = static_cast<ilya_tokenizer*>(tokenizer);
    delete t;
    return SQLITE_OK;
}

int ilya_tokenizer_open(sqlite3_tokenizer *tokenizer, const char *input, int nBytes, sqlite3_tokenizer_cursor **cursor)
{
    ilya_tokenizer_cursor *tc = new ilya_tokenizer_cursor(input, nBytes);

    *cursor = tc;
    return SQLITE_OK;
}

int ilya_tokenizer_close(sqlite3_tokenizer_cursor *cursor)
{
    ilya_tokenizer_cursor *tc = static_cast<ilya_tokenizer_cursor*>(cursor);

    delete tc;
    return SQLITE_OK;
}

int ilya_tokenizer_next(sqlite3_tokenizer_cursor *cursor, const char **token, int *nBytes, int *startOffset, int *endOffset, int *position)
{
    // printf("=== next\n");

    ilya_tokenizer_cursor *tc = static_cast<ilya_tokenizer_cursor*>(cursor);


    int token_pos = 0;

    bool done = false;
    bool next = true;
    int word_start = -1, word_end = -1;

    UChar32 c;
    while (! done && tc->pos < tc->len)
    {
        int c_pos = tc->pos;

        if (next)
            U8_NEXT(tc->input, tc->pos, tc->len, c);

        int type = u_charType(c);
        int bit = U_MASK(type);

        // printf("state=%d c=0x%x (%c)\n", tc->state, c, c);

        switch (tc->state)
        {
            case SPACE:
                if ((bit & U_GC_Z_MASK) || (bit & U_GC_C_MASK) || (bit & U_GC_P_MASK))
                {
                    next = true;
                }
                else
                {
                    next = false;
                    if (c == '<')
                    {
                        tc->state = TAG;
                    }
                    else
                    {
                        tc->state = WORD;
                        word_start = c_pos;
                    }
                }
                break;

            case WORD:
                if (c == '<')
                {
                    tc->state = TAG;
                    done = true;
                }
                else
                {
                    if ((bit & U_GC_Z_MASK) == 0 && (bit & U_GC_C_MASK) == 0 && (bit & U_GC_P_MASK) == 0)
                    {
                        UBool err;
                        // U8_APPEND(tc->token, token_pos, sizeof(tc->token), c, err);
                        tc->token[token_pos++] = c;
                    }
                    else
                    {
                        tc->state = SPACE;
                        word_end = c_pos;
                        done = true;
                    }
                }

                next = true;
                break;

            case TAG:
                if (c == '>')
                    tc->state = SPACE;
                next = true;
                break;
        }

    }

    if (word_end == -1)
        word_end = tc->len;

    Normalizer norm(tc->token, token_pos, UNORM_NFKD);
    c = norm.first();
    int p = 0;
    while (c != Normalizer::DONE)
    {
        int type = u_charType(c);
        int bit = U_MASK(type);

        UBool err;

        if ((bit & U_GC_M_MASK) == 0)
        {
            c = u_tolower(c);
            U8_APPEND(tc->token_utf8, p, sizeof(tc->token_utf8), c, err);
        }

        c = norm.next();
    }


    if (token_pos > 0)
    {
        *token = (char*)tc->token_utf8;
        *nBytes = p;
        *startOffset = word_start;
        *endOffset = word_end;
        *position = 0;

        printf("\t=== ");
        for (int i = 0; i < *nBytes; i++)
            printf("%c", (*token)[i]);
        printf(" (%d)", *nBytes);
        printf("\n");
        printf("\tstart = %d\n", *startOffset);
        printf("\tend   = %d\n", *endOffset);

        return SQLITE_OK;
    }
    else
        return SQLITE_DONE;
}
