#include "scores.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

void scores_load(Scoreboard *sb, const char *path)
{
    memset(sb, 0, sizeof *sb);
    FILE *fp = fopen(path, "r");
    if (!fp)
        return; /* first run: no scores yet */

    char line[128];
    while (sb->count < MAX_PLAYERS && fgets(line, sizeof line, fp)) {
        Player p = {0};
        /* %31[^|] reads at most 31 characters, so a long name cannot overflow */
        if (sscanf(line, "%31[^|]|%d|%d|%d", p.name, &p.score, &p.wins, &p.losses) == 4)
            sb->players[sb->count++] = p;
    }
    fclose(fp);
}

int scores_save(const Scoreboard *sb, const char *path)
{
    FILE *fp = fopen(path, "w");
    if (!fp)
        return -1;
    for (int i = 0; i < sb->count; i++) {
        const Player *p = &sb->players[i];
        fprintf(fp, "%s|%d|%d|%d\n", p->name, p->score, p->wins, p->losses);
    }
    return fclose(fp) == 0 ? 0 : -1;
}

Player *scores_find_or_add(Scoreboard *sb, const char *name)
{
    for (int i = 0; i < sb->count; i++)
        if (strcasecmp(sb->players[i].name, name) == 0)
            return &sb->players[i];

    if (sb->count == MAX_PLAYERS)
        return NULL;
    Player *p = &sb->players[sb->count++];
    memset(p, 0, sizeof *p);
    snprintf(p->name, sizeof p->name, "%s", name);
    return p;
}

static int by_score_desc(const void *a, const void *b)
{
    const Player *pa = a, *pb = b;
    if (pb->score != pa->score)
        return pb->score - pa->score;
    return strcasecmp(pa->name, pb->name);
}

void scores_sort(Scoreboard *sb)
{
    qsort(sb->players, (size_t)sb->count, sizeof sb->players[0], by_score_desc);
}
