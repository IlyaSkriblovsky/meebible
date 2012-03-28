#ifndef ILYATOKENIZER_H
#define ILYATOKENIZER_H

#include "fts3_tokenizer.h"

#ifdef __cplusplus
extern "C" {
#endif
void install_ilya_tokenizer(sqlite3 *db);
#ifdef __cplusplus
}
#endif

#endif
