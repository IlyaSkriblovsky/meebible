#include "tokenizer.h"

#include <cstdio>
#include <cstring>
#include <cstdlib>

#include <sqlite3/fts3_tokenizer.h>

#include <unisimple/unisimple.h>
#include <unisimple/utf8.h>


namespace us
{


class TokenizerCursor: public sqlite3_tokenizer_cursor
{
    public:
        TokenizerCursor(const char *input, int length)
            :_input(input), _input_length(length), _input_pos(0),
             _token(0), _token_buf_size(0), _token_pos(0),
             _nToken(0)
        {
            if (_input_length == -1)
                _input_length = strlen(_input);
        }

        ~TokenizerCursor()
        {
            free(_token);
        }



        // sqlite interface functions

        static int sq_close(sqlite3_tokenizer_cursor *cursor)
        {
            delete static_cast<TokenizerCursor*>(cursor);
            return SQLITE_OK;
        }

        static int sq_next(sqlite3_tokenizer_cursor *cursor, const char **token, int *nBytes, int *startOffset, int *endOffset, int *position)
        {
            TokenizerCursor *c = static_cast<TokenizerCursor*>(cursor);
            if (c->nextToken(token, nBytes, startOffset, endOffset, position))
            {
                // printf("\t=== [%d:%d] ", *startOffset, *endOffset);
                // for (int i = 0; i < *nBytes; i++)
                //     printf("%c", (*token)[i]);
                // printf("\n");
                return SQLITE_OK;
            }
            else
            {
                return SQLITE_DONE;
            }
        }



    private:
        const char *_input;
        int _input_length;
        int _input_pos;
        int _cur_input_offset;


        char *_token;
        int _token_buf_size;
        int _token_pos;


        int _nToken;


        char32 nextChar()
        {
            _cur_input_offset = _input_pos;
            if (_input_pos >= _input_length)
                return 0;
            else
            {
                char32 c;
                U8_NEXT_UNSAFE(_input, _input_pos, c);
                return c;
            }
        }

        inline bool isSpacingCharClass(const CharClass cc)
        {
            return (cc & (UCHAR_CLASS_Z | UCHAR_CLASS_P | UCHAR_CLASS_S | UCHAR_CLASS_C)) != 0;
        }

        void addToToken(char32 c)
        {
            static char32 simplified[20];
            int count = us::simplifyChar(c, simplified);

            for (int i = 0; i < count; i++)
            {
                if (_token_buf_size - _token_pos < 4)
                {
                    if (_token_buf_size == 0)
                        _token_buf_size = 20;
                    else
                        _token_buf_size *= 2;
                    _token = (char*)realloc(_token, _token_buf_size);
                }

                U8_APPEND_UNSAFE(_token, _token_pos, simplified[i]);
            }
        }


        bool nextToken(const char **token, int *length, int *startOffset, int *endOffset, int *position)
        {
            char32 c = nextChar();
            CharClass cc = us::charClass(c);
            bool in_tag = false;
            while (c != 0)
            {
                if (c == '<')
                    in_tag = true;
                else if (c == '>')
                    in_tag = false;
                else
                    if (! in_tag && ! isSpacingCharClass(cc))
                        break;

                c = nextChar();
                cc = us::charClass(c);
            }

            if (c == 0) return false;


            *startOffset = _cur_input_offset;
            _token_pos = 0;

            while (c != 0 && c != '<' && ! isSpacingCharClass(cc))
            {
                addToToken(c);

                c = nextChar();
                cc = us::charClass(c);
            }

            *token = _token;
            *length = _token_pos;
            *endOffset = _cur_input_offset;
            *position = _nToken++;

            _input_pos = _cur_input_offset;

            return true;
        }


};

class Tokenizer: public sqlite3_tokenizer
{
    public:
        // sqlite interface functions

        static int sq_create(int argc, const char * const *argv, sqlite3_tokenizer **tokenizer)
        {
            *tokenizer = new Tokenizer(argc, argv);
            return SQLITE_OK;
        }

        static int sq_destroy(sqlite3_tokenizer *tokenizer)
        {
            delete static_cast<Tokenizer*>(tokenizer);
            return SQLITE_OK;
        }

        static int sq_open(sqlite3_tokenizer *tokenizer, const char *input, int nBytes, sqlite3_tokenizer_cursor **cursor)
        {
            *cursor = new TokenizerCursor(input, nBytes);
            return SQLITE_OK;
        }


    private:
        Tokenizer(int argc, const char * const argv[])
        {
            Q_UNUSED(argc)
            Q_UNUSED(argv)
        }
};


sqlite3_tokenizer_module desc = {
    0,
    Tokenizer::sq_create,
    Tokenizer::sq_destroy,
    Tokenizer::sq_open,
    TokenizerCursor::sq_close,
    TokenizerCursor::sq_next
};


}





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


void install_unisimple_tokenizer(sqlite3 *db)
{
    registerTokenizer(db, "unisimple", &us::desc);
}
