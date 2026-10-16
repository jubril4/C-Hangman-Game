//Jubril Hall-Robinson
//Date: Dec. 12, 2022
//Dr.Likoudis 
//Hangman Project Part 3


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_WORDS 50
#define MAX_CHAR_LENGTH 30
#define MAX_NAMES 50

// structure to store words
struct Words
{
    char word[MAX_CHAR_LENGTH];
    int used;
};

// structure to store player's scores
struct Player
{
    char name[MAX_CHAR_LENGTH];
    int score;
};

// Function to read words from file
void readWords(struct Words words[], int *n)
{
    FILE *fp = NULL;
    char list[MAX_CHAR_LENGTH];
    int i = 0;

    fp = fopen("words.txt", "r");
    if (!fp)
    {
        perror("\n The File cannot be opened!");
        exit(EXIT_FAILURE);
    }

    // Read words from file
    while (fgets(list, sizeof(list), fp) != NULL)
    {
        // Copy word to words array
        strcpy(words[i].word, list);
        words[i].used = 0;
        i++;
    }
    *n = i;
    fclose(fp);
}

// Function to read players from file
void readPlayers(struct Player players[], int *n)
{
    FILE *fp;
    char list[MAX_CHAR_LENGTH];
    char *token;
    int i = 0;

    fp = fopen("statistics.txt", "r");
    if (!fp)
    {
        printf("\nThe file cannot be opened ");
        exit(EXIT_FAILURE);
    }

    // Read players from file
    while (fgets(list, sizeof(list), fp) != NULL)
    {
        token = strtok(list, "|");
        strcpy(players[i].name, token);
        token = strtok(NULL, "|");
        players[i].score = atoi(token);
        i++;
    }
    *n = i;
    fclose(fp);
}

// Function to print players and scores
void printPlayers(struct Player players[], int n)
{
    int i;

    printf("Name\tScore\n");
    for (i = 0; i < n; i++)
    {
        printf("%s\t%d\n", players[i].name, players[i].score);
    }
}

// Function to play the game
void playGame(struct Words words[], int n, struct Player players[], int p)
{
    int i, j, k;
    int score = 0;
    int wordList;
    int wordLength;
    int found = 0;
    char word[MAX_CHAR_LENGTH];
    char guess;
    char guessed[MAX_CHAR_LENGTH];

    // Find a word that has not been used
    do
    {
        wordList = rand() % n;
    } while (words[wordList].used == 1);

    strcpy(word, words[wordList].word);
    wordLength = strlen(word) - 1;
    words[wordList].used = 1;

    // Initialize guessed array
    for (i = 0; i < wordLength; i++)
    {
        guessed[i] = '_';
    }

    while (score < wordLength)
    {
        // Print word
        printf("Word: ");
        for (i = 0; i < wordLength; i++)
        {
            printf("%c ", guessed[i]);
        }
        printf("\n");

        // Get input
        printf("Guess a letter or word: ");
        scanf(" %c", &guess);

        // Check if letter is in word
        for (j = 0; j < wordLength; j++)
        {
            if (guess == word[j])
            {
                guessed[j] = guess;
                score++;
                found = 1;
            }
        }

        // Check if letter is already guessed
        for (k = 0; k < wordLength; k++)
        {
            if (guess == guessed[k])
            {
                found = 1;
            }
        }

        if (found == 0)
        {
            printf("Letter not found!\n");
        }
        found = 0;
    }

    // Calculate score
    score = score * 10;

    // Check if player is in stats
    for (i = 0; i < p; i++)
    {
        if (strcmp(players[i].name, "player") == 0)
        {
            players[i].score += score;
        }
    }

    printf("Congratulations You won! Your score is %d\n", score);
}

// Function to write players to file
void writePlayers(struct Player players[], int n)
{
    FILE *fp;
    int i;

    fp = fopen("statistics.txt", "w");
    if (fp == NULL)
    {
        printf("\nThe file cannot be opened!");
        exit(EXIT_FAILURE);
    }

    // Write players to file
    for (i = 0; i < n; i++)
    {
        fprintf(fp, "%s|%d\n", players[i].name, players[i].score);
    }
    fclose(fp);
}

// Main function
int main()
{
    int choice;
    int nWords = 0;
    int nPlayers = 0;
    struct Words words[MAX_WORDS];
    struct Player players[MAX_NAMES];

    readWords(words, &nWords);
    readPlayers(players, &nPlayers);

    // Menu
    while (1)
    {
        printf("\n----------- Menu -----------");
        printf("\n1. Play Game\n2. Display statistics\n3. Quit\n\nInput: ");
        scanf("%d", &choice);
        switch (choice)
        {
        case 1:
            playGame(words, nWords, players, nPlayers);
            break;
        case 2:
            printPlayers(players, nPlayers);
            break;
        case 3:
            writePlayers(players, nPlayers);
            printf("\nGood Bye!!\n\n");
            exit(0);
            break;
        default:
            printf("\nInvalid Option Chosen!!\nTry again...!\n");
            break;
        }
    }

    return 0;
}