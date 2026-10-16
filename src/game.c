#include "game.h"

#include <ctype.h>
#include <string.h>

bool game_init(Game *g, const char *word)
{
    size_t len = strlen(word);
    if (len == 0 || len >= MAX_WORD_LEN)
        return false;

    memset(g, 0, sizeof *g);
    for (size_t i = 0; i < len; i++) {
        unsigned char c = (unsigned char)word[i];
        if (!isalpha(c))
            return false;
        g->word[i] = (char)tolower(c);
        g->revealed[i] = '_';
    }
    return true;
}

GuessResult game_guess(Game *g, char letter)
{
    unsigned char c = (unsigned char)letter;
    if (!isalpha(c))
        return GUESS_INVALID;

    c = (unsigned char)tolower(c);
    if (g->guessed[c - 'a'])
        return GUESS_REPEAT;
    g->guessed[c - 'a'] = true;

    bool hit = false;
    for (size_t i = 0; g->word[i] != '\0'; i++) {
        if (g->word[i] == (char)c) {
            g->revealed[i] = (char)c;
            hit = true;
        }
    }
    if (!hit)
        g->wrong++;
    return hit ? GUESS_CORRECT : GUESS_WRONG;
}

bool game_won(const Game *g)
{
    return g->word[0] != '\0' && strchr(g->revealed, '_') == NULL;
}

bool game_lost(const Game *g)
{
    return g->wrong >= MAX_WRONG;
}

int game_score(const Game *g)
{
    if (!game_won(g))
        return 0;
    return 10 * (int)strlen(g->word) + 5 * (MAX_WRONG - g->wrong);
}
