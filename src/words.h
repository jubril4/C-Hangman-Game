#ifndef WORDS_H
#define WORDS_H

#include <stdbool.h>
#include "game.h"

#define MAX_WORDS 200

typedef struct {
    char words[MAX_WORDS][MAX_WORD_LEN];
    bool used[MAX_WORDS];
    int  count;
    int  remaining;
} WordList;

/* Load one word per line. Trims spaces and line endings, skips blank or invalid lines.
   Returns the number of words loaded, or -1 if the file cannot be opened. */
int words_load(WordList *wl, const char *path);

/* Pick a random unused word and mark it used. Returns NULL when every word has been played. */
const char *words_next(WordList *wl);

#endif
