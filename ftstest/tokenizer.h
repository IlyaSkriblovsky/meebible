#ifndef TOKENIZER_H
#define TOKENIZER_H

#include "fts3_tokenizer.h"

#ifdef __cplusplus
extern "C" {
#endif
void install_unisimple_tokenizer(sqlite3 *db);
#ifdef __cplusplus
}
#endif

#endif
