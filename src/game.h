#ifndef GAME_H
#define GAME_H

#include <stdbool.h>

#define MAX_WORD_LEN 32
#define MAX_WRONG    6

/* Result of a single guess */
typedef enum {
    GUESS_CORRECT,
    GUESS_WRONG,
    GUESS_REPEAT,   /* letter was already guessed; costs nothing */
    GUESS_INVALID   /* not a letter */
} GuessResult;

typedef struct {
    char word[MAX_WORD_LEN];      /* secret word, lowercase */
    char revealed[MAX_WORD_LEN];  /* '_' for hidden letters */
    bool guessed[26];             /* every letter tried so far */
    int  wrong;                   /* wrong guesses used */
} Game;

/* Start a round. Returns false if the word is empty, too long, or not all letters. */
bool game_init(Game *g, const char *word);

/* Apply one letter guess (case insensitive). */
GuessResult game_guess(Game *g, char letter);

bool game_won(const Game *g);
bool game_lost(const Game *g);

/* Points for a finished round: 10 per letter in the word plus 5 per life left. 0 if lost. */
int game_score(const Game *g);

#endif
