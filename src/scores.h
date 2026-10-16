#ifndef SCORES_H
#define SCORES_H

#define MAX_PLAYERS  100
#define MAX_NAME_LEN 32

typedef struct {
    char name[MAX_NAME_LEN];
    int  score;
    int  wins;
    int  losses;
} Player;

typedef struct {
    Player players[MAX_PLAYERS];
    int    count;
} Scoreboard;

/* Load "name|score|wins|losses" lines. A missing file is fine: it means a new, empty scoreboard. */
void scores_load(Scoreboard *sb, const char *path);

/* Returns 0 on success, -1 if the file cannot be written. */
int scores_save(const Scoreboard *sb, const char *path);

/* Find a player by name (case insensitive), adding them if new. Returns NULL if the board is full. */
Player *scores_find_or_add(Scoreboard *sb, const char *name);

/* Sort highest score first. */
void scores_sort(Scoreboard *sb);

#endif
