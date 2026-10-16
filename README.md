# Hangman in C

![tests](https://github.com/jubril4/c-hangman-game/actions/workflows/tests.yml/badge.svg)

A console Hangman game in C11 with an ASCII gallows, a persistent scoreboard, and unit tests that run under AddressSanitizer on every push.

This started as my final project for Programming in C (December 2022). I came back to it, found six bugs in the original, and rebuilt it as separate, tested modules. The original submission is kept in `original/` for comparison.

**Skills shown:** C11, structs, file I/O, string handling, defensive input parsing, modular design with header files, Makefiles, unit testing, memory error checking with AddressSanitizer and UBSan, and continuous integration with GitHub Actions.

## Sample game

```
  +---+
  |   |
  O   |
      |
      |
      |
=========
Word: t h r o n e
Guessed: a e h n o r t
Lives left: 5

You won! The word was "throne". +85 points (total 85).

Rank Player                Score  Wins  Losses
1    Jubril                   85     1       0
```

## How to play

```bash
make            # build ./hangman (warnings are treated as errors)
./hangman       # play
make test       # run the unit tests
make sanitize   # run the tests under AddressSanitizer and UBSan
```

Optional flags: `--seed N` for a repeatable word order, `--words FILE` for a custom word list, and `--scores FILE` for a different scoreboard file.

**Rules:** guess one letter at a time. You have 6 lives. Repeating a guess costs nothing. A win scores 10 points per letter plus 5 points per life left. Scores are saved to `statistics.txt` and players are matched by name, so returning players keep their totals.

## Bugs found in the original and how they were fixed

| Bug in the 2022 version | Fix |
|---|---|
| Crashed on the first run because `statistics.txt` did not exist yet | A missing scoreboard file now starts an empty scoreboard |
| Leading spaces and line endings in `words.txt` were counted as part of each word, leaving blanks no guess could ever fill | Words are trimmed when loaded, and lines with spaces or digits are skipped |
| Guessing a correct letter again added points again, so a player could win by repeating one letter | Every guess is tracked; repeats are ignored and cost nothing |
| No limit on wrong guesses, so the player could never lose | 6 lives with an ASCII gallows that fills in on each miss |
| After every word was used, choosing the next word looped forever | Picks the k-th unused word directly and tells the player when the list is finished |
| Every score was credited to a player literally named "player" | Asks for a name and saves score, wins, and losses per player |

It also removes fixed size reads that could overflow (`scanf("%c")`, unchecked `strcpy`, and `strtok` on a missing field) and reads every input line with bounds checked `fgets`.

## Project structure

```
src/
  game.c / game.h       game rules: guesses, win and loss, scoring (no input or output)
  words.c / words.h     loads and trims the word list, picks unused words
  scores.c / scores.h   loads, saves, finds, and sorts players
  main.c                menu, prompts, and the ASCII gallows
tests/test_hangman.c    55 checks across the three modules
data/words.txt          50 words
original/               the 2022 course submission and its word list
.github/workflows/      builds and runs the tests, then reruns them under sanitizers
```

Keeping the game rules free of `printf` and `scanf` is what makes them testable: the tests call `game_guess()` directly and check the result, with no keyboard needed.

## Author

Jubril Hall-Robinson, B.S. Computer Engineering, Capitol Technology University
[jubrilhall-robinsonportfolio.dev](https://jubrilhall-robinsonportfolio.dev) | [LinkedIn](https://www.linkedin.com/in/jubrilhallrobinson)
