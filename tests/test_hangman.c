/* Unit tests for the game, word list, and scoreboard modules. Run with: make test */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../src/game.h"
#include "../src/scores.h"
#include "../src/words.h"

static int failures = 0, checks = 0;

#define CHECK(cond)                                                        \
    do {                                                                   \
        checks++;                                                          \
        if (!(cond)) {                                                     \
            failures++;                                                    \
            printf("  FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond);       \
        }                                                                  \
    } while (0)

static void test_correct_and_wrong_guesses(void)
{
    Game g;
    CHECK(game_init(&g, "Snake"));
    CHECK(strcmp(g.word, "snake") == 0);
    CHECK(strcmp(g.revealed, "_____") == 0);
    CHECK(game_guess(&g, 'S') == GUESS_CORRECT);   /* case insensitive */
    CHECK(strcmp(g.revealed, "s____") == 0);
    CHECK(game_guess(&g, 'z') == GUESS_WRONG);
    CHECK(g.wrong == 1);
}

static void test_repeat_guess_is_free(void)
{
    /* The original game awarded points again for a repeated correct letter */
    Game g;
    game_init(&g, "pair");
    CHECK(game_guess(&g, 'p') == GUESS_CORRECT);
    CHECK(game_guess(&g, 'p') == GUESS_REPEAT);
    CHECK(game_guess(&g, 'q') == GUESS_WRONG);
    CHECK(game_guess(&g, 'q') == GUESS_REPEAT);
    CHECK(g.wrong == 1);
}

static void test_repeated_letters_reveal_together(void)
{
    Game g;
    game_init(&g, "midnight");
    game_guess(&g, 'i');
    CHECK(strcmp(g.revealed, "_i__i___") == 0);
}

static void test_invalid_input(void)
{
    Game g;
    game_init(&g, "era");
    CHECK(game_guess(&g, '3') == GUESS_INVALID);
    CHECK(game_guess(&g, ' ') == GUESS_INVALID);
    CHECK(g.wrong == 0);
    CHECK(!game_init(&g, ""));
    CHECK(!game_init(&g, "two words"));
    CHECK(!game_init(&g, "abcdefghijklmnopqrstuvwxyzabcdefgh")); /* too long */
}

static void test_win_and_score(void)
{
    Game g;
    game_init(&g, "gas");
    game_guess(&g, 'x');                     /* one wrong guess */
    game_guess(&g, 'g');
    game_guess(&g, 'a');
    CHECK(!game_won(&g));
    CHECK(game_score(&g) == 0);              /* no points until won */
    game_guess(&g, 's');
    CHECK(game_won(&g));
    CHECK(game_score(&g) == 10 * 3 + 5 * 5); /* 3 letters, 5 lives left */
}

static void test_loss_after_six_wrong(void)
{
    Game g;
    game_init(&g, "veil");
    const char *misses = "abcdfg";
    for (int i = 0; i < MAX_WRONG; i++) {
        CHECK(!game_lost(&g));
        game_guess(&g, misses[i]);
    }
    CHECK(game_lost(&g));
    CHECK(game_score(&g) == 0);
}

static void test_word_loading_trims_and_filters(void)
{
    const char *path = "test_words.tmp";
    FILE *fp = fopen(path, "w");
    fputs("   displace\r\n\nhello world\nsn4ke\n  roar  \nbrain", fp);
    fclose(fp);

    WordList wl;
    CHECK(words_load(&wl, path) == 3);       /* skips blank, spaced, and digit lines */
    CHECK(strcmp(wl.words[0], "displace") == 0);
    CHECK(strcmp(wl.words[1], "roar") == 0);
    CHECK(strcmp(wl.words[2], "brain") == 0);
    remove(path);

    CHECK(words_load(&wl, "does_not_exist.txt") == -1);
}

static void test_words_never_repeat_and_run_out(void)
{
    const char *path = "test_words.tmp";
    FILE *fp = fopen(path, "w");
    fputs("one\ntwo\nthree\nfour\n", fp);
    fclose(fp);

    WordList wl;
    words_load(&wl, path);
    srand(42);
    const char *seen[4];
    for (int i = 0; i < 4; i++) {
        seen[i] = words_next(&wl);
        CHECK(seen[i] != NULL);
        for (int j = 0; j < i; j++)
            CHECK(seen[i] != seen[j]);
    }
    CHECK(words_next(&wl) == NULL);          /* the original looped forever here */
    remove(path);
}

static void test_scoreboard_round_trip(void)
{
    const char *path = "test_scores.tmp";
    remove(path);

    Scoreboard sb;
    scores_load(&sb, path);                  /* missing file is fine on first run */
    CHECK(sb.count == 0);

    Player *a = scores_find_or_add(&sb, "Avery");
    a->score = 40; a->wins = 1;
    Player *b = scores_find_or_add(&sb, "Jordan");
    b->score = 95; b->wins = 2; b->losses = 1;
    CHECK(scores_find_or_add(&sb, "avery") == a); /* case insensitive lookup */
    CHECK(sb.count == 2);
    CHECK(scores_save(&sb, path) == 0);

    Scoreboard loaded;
    scores_load(&loaded, path);
    scores_sort(&loaded);
    CHECK(loaded.count == 2);
    CHECK(strcmp(loaded.players[0].name, "Jordan") == 0);
    CHECK(loaded.players[0].score == 95 && loaded.players[0].losses == 1);
    CHECK(strcmp(loaded.players[1].name, "Avery") == 0);
    remove(path);
}

int main(void)
{
    test_correct_and_wrong_guesses();
    test_repeat_guess_is_free();
    test_repeated_letters_reveal_together();
    test_invalid_input();
    test_win_and_score();
    test_loss_after_six_wrong();
    test_word_loading_trims_and_filters();
    test_words_never_repeat_and_run_out();
    test_scoreboard_round_trip();

    printf("%d checks, %d failed\n", checks, failures);
    return failures == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
