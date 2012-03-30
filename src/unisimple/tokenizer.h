#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <sqlite3/sqlite3.h>

#ifdef __cplusplus
extern "C" {
#endif
void install_unisimple_tokenizer(sqlite3 *db);
#ifdef __cplusplus
}
#endif

#endif
