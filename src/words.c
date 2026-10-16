#include "words.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Strip leading and trailing whitespace, including Windows \r\n line endings */
static char *trim(char *s)
{
    while (isspace((unsigned char)*s))
        s++;
    char *end = s + strlen(s);
    while (end > s && isspace((unsigned char)end[-1]))
        *--end = '\0';
    return s;
}

static bool all_letters(const char *s)
{
    for (; *s; s++)
        if (!isalpha((unsigned char)*s))
            return false;
    return true;
}

int words_load(WordList *wl, const char *path)
{
    FILE *fp = fopen(path, "r");
    if (!fp)
        return -1;

    memset(wl, 0, sizeof *wl);
    char line[128];
    while (wl->count < MAX_WORDS && fgets(line, sizeof line, fp)) {
        char *w = trim(line);
        size_t len = strlen(w);
        if (len == 0 || len >= MAX_WORD_LEN || !all_letters(w))
            continue;
        memcpy(wl->words[wl->count], w, len + 1);
        wl->count++;
    }
    fclose(fp);
    wl->remaining = wl->count;
    return wl->count;
}

const char *words_next(WordList *wl)
{
    if (wl->remaining == 0)
        return NULL;

    /* Choose the k-th unused word, so the loop always finishes */
    int k = rand() % wl->remaining;
    for (int i = 0; i < wl->count; i++) {
        if (wl->used[i])
            continue;
        if (k-- == 0) {
            wl->used[i] = true;
            wl->remaining--;
            return wl->words[i];
        }
    }
    return NULL;
}
