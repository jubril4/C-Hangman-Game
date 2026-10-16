/*
 * Hangman in C
 * Jubril Hall-Robinson
 * Rebuilt from a Programming in C course project (December 2022).
 *
 * Usage: ./hangman [--seed N] [--words FILE] [--scores FILE]
 */
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "game.h"
#include "scores.h"
#include "words.h"

static const char *GALLOWS[MAX_WRONG + 1] = {
    "  +---+\n  |   |\n      |\n      |\n      |\n      |\n=========\n",
    "  +---+\n  |   |\n  O   |\n      |\n      |\n      |\n=========\n",
    "  +---+\n  |   |\n  O   |\n  |   |\n      |\n      |\n=========\n",
    "  +---+\n  |   |\n  O   |\n /|   |\n      |\n      |\n=========\n",
    "  +---+\n  |   |\n  O   |\n /|\\  |\n      |\n      |\n=========\n",
    "  +---+\n  |   |\n  O   |\n /|\\  |\n /    |\n      |\n=========\n",
    "  +---+\n  |   |\n  O   |\n /|\\  |\n / \\  |\n      |\n=========\n",
};

/* Read one line of input safely. Returns 0 at end of input. */
static int read_line(const char *prompt, char *buf, size_t size)
{
    printf("%s", prompt);
    fflush(stdout);
    if (!fgets(buf, (int)size, stdin))
        return 0;
    buf[strcspn(buf, "\r\n")] = '\0';
    return 1;
}

static void show_board(const Game *g)
{
    printf("\n%s", GALLOWS[g->wrong]);
    printf("Word: ");
    for (size_t i = 0; g->revealed[i]; i++)
        printf("%c ", g->revealed[i]);
    printf("\nGuessed: ");
    for (int i = 0; i < 26; i++)
        if (g->guessed[i])
            printf("%c ", 'a' + i);
    printf("\nLives left: %d\n", MAX_WRONG - g->wrong);
}

/* Play one round. Returns 0 if input ended early. */
static int play_round(WordList *wl, Player *player)
{
    const char *secret = words_next(wl);
    if (!secret) {
        printf("\nYou have played every word in the list. Restart to play again.\n");
        return 1;
    }

    Game g;
    game_init(&g, secret);
    char buf[64];

    while (!game_won(&g) && !game_lost(&g)) {
        show_board(&g);
        if (!read_line("Guess a letter: ", buf, sizeof buf))
            return 0;
        if (strlen(buf) != 1) {
            printf("Please enter exactly one letter.\n");
            continue;
        }
        switch (game_guess(&g, buf[0])) {
        case GUESS_CORRECT: printf("Yes, '%c' is in the word.\n", tolower((unsigned char)buf[0])); break;
        case GUESS_WRONG:   printf("No '%c' in the word.\n", tolower((unsigned char)buf[0])); break;
        case GUESS_REPEAT:  printf("You already guessed '%c'. No life lost.\n", tolower((unsigned char)buf[0])); break;
        case GUESS_INVALID: printf("That is not a letter.\n"); break;
        }
    }

    show_board(&g);
    if (game_won(&g)) {
        int pts = game_score(&g);
        player->score += pts;
        player->wins++;
        printf("\nYou won! The word was \"%s\". +%d points (total %d).\n", g.word, pts, player->score);
    } else {
        player->losses++;
        printf("\nOut of lives. The word was \"%s\".\n", g.word);
    }
    return 1;
}

static void show_scoreboard(Scoreboard *sb)
{
    scores_sort(sb);
    printf("\n%-4s %-20s %6s %5s %7s\n", "Rank", "Player", "Score", "Wins", "Losses");
    for (int i = 0; i < sb->count; i++) {
        const Player *p = &sb->players[i];
        printf("%-4d %-20s %6d %5d %7d\n", i + 1, p->name, p->score, p->wins, p->losses);
    }
    if (sb->count == 0)
        printf("No games played yet.\n");
}

int main(int argc, char *argv[])
{
    const char *words_path = "data/words.txt";
    const char *scores_path = "statistics.txt";
    unsigned seed = (unsigned)time(NULL);

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--seed") == 0 && i + 1 < argc)
            seed = (unsigned)strtoul(argv[++i], NULL, 10);
        else if (strcmp(argv[i], "--words") == 0 && i + 1 < argc)
            words_path = argv[++i];
        else if (strcmp(argv[i], "--scores") == 0 && i + 1 < argc)
            scores_path = argv[++i];
        else {
            fprintf(stderr, "Usage: %s [--seed N] [--words FILE] [--scores FILE]\n", argv[0]);
            return EXIT_FAILURE;
        }
    }
    srand(seed);

    static WordList wl;
    if (words_load(&wl, words_path) <= 0) {
        fprintf(stderr, "Could not load any words from %s\n", words_path);
        return EXIT_FAILURE;
    }
    static Scoreboard sb;
    scores_load(&sb, scores_path);

    char name[MAX_NAME_LEN] = "";
    char buf[64];
    printf("Welcome to Hangman! %d words loaded.\n", wl.count);
    while (name[0] == '\0') {
        if (!read_line("Enter your name: ", buf, sizeof buf))
            return EXIT_SUCCESS;
        buf[strcspn(buf, "|")] = '\0'; /* '|' is the file separator */
        buf[MAX_NAME_LEN - 1] = '\0';  /* names longer than 31 characters are cut */
        memcpy(name, buf, strlen(buf) + 1);
    }
    Player *player = scores_find_or_add(&sb, name);
    if (!player) {
        fprintf(stderr, "Scoreboard is full.\n");
        return EXIT_FAILURE;
    }
    /* Remember the name: sorting the board later can move the player's row */
    snprintf(name, sizeof name, "%s", player->name);

    for (;;) {
        printf("\n---------- Menu ----------\n1. Play a round\n2. Show scoreboard\n3. Save and quit\n");
        if (!read_line("Choice: ", buf, sizeof buf))
            break;
        if (strcmp(buf, "1") == 0) {
            player = scores_find_or_add(&sb, name);
            if (!play_round(&wl, player))
                break;
        } else if (strcmp(buf, "2") == 0) {
            show_scoreboard(&sb);
        } else if (strcmp(buf, "3") == 0) {
            break;
        } else {
            printf("Please choose 1, 2, or 3.\n");
        }
    }

    if (scores_save(&sb, scores_path) != 0)
        fprintf(stderr, "Warning: could not save scores to %s\n", scores_path);
    printf("\nScores saved. Goodbye, %s!\n", name);
    return EXIT_SUCCESS;
}
