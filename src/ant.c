#include "../include/ant.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Function that reads the initialization data from the server
GameSettings *initialize_game()
{
    GameSettings *gameSettings = (GameSettings *) malloc(sizeof(GameSettings));

    // Initialize to 0 so that getline automatically allocates enough memory
    char *line = NULL;
    size_t lineLength = 0;

    // Read line by line
    while(getline(&line, &lineLength, stdin) != -1)
    {
        // Proces the input
        // Skip empty lines
        if(strcmp(line, "\n") != 0)
        {
            // Seperate the key token
            char *key_token = strtok(line, " \n");

            // Compare the key token and initialize the right setting
            if(strcmp(key_token, "loadtime") == 0)
            {
                gameSettings->loadtime = strtoul(strtok(NULL, " \n"), NULL, 10);
            }
            else if(strcmp(key_token, "turntime") == 0)
            {
                gameSettings->turntime = strtoul(strtok(NULL, " \n"), NULL, 10);
            }
            else if(strcmp(key_token, "rows") == 0)
            {
                gameSettings->rows = strtoul(strtok(NULL, " \n"), NULL, 10);
            }
            else if(strcmp(key_token, "cols") == 0)
            {
                gameSettings->cols = strtoul(strtok(NULL, " \n"), NULL, 10);
            }
            else if(strcmp(key_token, "turns") == 0)
            {
                gameSettings->turns = strtoul(strtok(NULL, " \n"), NULL, 10);
            }
            else if(strcmp(key_token, "viewradius2") == 0)
            {
                gameSettings->viewradius2 = strtoul(strtok(NULL, " \n"), NULL, 10);
            }
            else if(strcmp(key_token, "attackradius2") == 0)
            {
                gameSettings->attackradius2 = strtoul(strtok(NULL, " \n"), NULL, 10);
            }
            else if(strcmp(key_token, "spawnradius2") == 0)
            {
                gameSettings->spawnradius2 = strtoul(strtok(NULL, " \n"), NULL, 10);
            }
            else if(strcmp(key_token, "player_seed") == 0)
            {
                gameSettings->player_seed = strtoull(strtok(NULL, " \n"), NULL, 10);
            }
            else if(strcmp(key_token, "ready") == 0)
            {
                // All settings retrieved, break out of the while loop
                break;
            }
        }
    }

    // Free the memory that the getline function allocated
    free(line);

    return gameSettings;
}

// Function for freeing the allocated game settings memory
void cleanup_game(GameSettings *gameSettings)
{
    free(gameSettings);
    return;
}

// Function that reads the data at the beginning of each turn
int read_turn(Map *map)
{
    int turn;

    // Initialize to 0 so that getline automatically allocates enough memory
    char *line = NULL;
    size_t lineLength = 0;

    // Read line by line
    while(getline(&line, &lineLength, stdin) != -1)
    {
        // Proces the input
        // Skip empty lines
        if(strcmp(line, "\n") != 0)
        {
            // Seperate the key token
            char *key_token = strtok(line, " \n");

            // Compare the key token and initialize the right setting
            if(strcmp(key_token, "turn") == 0)
            {
                turn = strtoul(strtok(NULL, " \n"), NULL, 10);
            }
            else if(strcmp(key_token, "end") == 0)
            {
                // Game is over (turn -1)
                turn = -1;
            }
            else if(strcmp(key_token, "w") == 0)
            {
                // Water
                int row = strtoul(strtok(NULL, " \n"), NULL, 10);
                int col = strtoul(strtok(NULL, " \n"), NULL, 10);
                Cell *cell = index_map(map, row, col);
                cell->type = CELL_WATER;
            }
            else if(strcmp(key_token, "f") == 0)
            {
                // Food
                int row = strtoul(strtok(NULL, " \n"), NULL, 10);
                int col = strtoul(strtok(NULL, " \n"), NULL, 10);
                Cell *cell = index_map(map, row, col);
                cell->type = CELL_FOOD;
            }
            else if(strcmp(key_token, "h") == 0)
            {
                // Ant hill
                int row = strtoul(strtok(NULL, " \n"), NULL, 10);
                int col = strtoul(strtok(NULL, " \n"), NULL, 10);
                int owner = strtoul(strtok(NULL, " \n"), NULL, 10);
                Cell *cell = index_map(map, row, col);
                cell->type = CELL_HILL;
                cell->owner = owner;
            }
            else if(strcmp(key_token, "a") == 0)
            {
                // Live ant
                int row = strtoul(strtok(NULL, " \n"), NULL, 10);
                int col = strtoul(strtok(NULL, " \n"), NULL, 10);
                int owner = strtoul(strtok(NULL, " \n"), NULL, 10);
                Cell *cell = index_map(map, row, col);
                cell->type = CELL_ANT;
                cell->owner = owner;
            }
            else if(strcmp(key_token, "d") == 0)
            {
                // Dead ant
                // int row = strtoul(strtok(NULL, " \n"), NULL, 10);
                // int col = strtoul(strtok(NULL, " \n"), NULL, 10);
                // int owner = strtoul(strtok(NULL, " \n"), NULL, 10);
            }
            else if(strcmp(key_token, "go") == 0)
            {
                // All data retrieved, break out of the while loop
                break;
            }
        }
    }

    return turn;
}

// Function for allocating and initializing a map structure
Map *initialize_map(int rows, int cols)
{
    // Allocate map memory and initialize rows and cols
    Map *map = (Map *) malloc(sizeof(Map));
    map->rows = rows;
    map->cols = cols;

    // Allocate cell memory
    // By using calloc everyting initilizes to 0 which means type dirt and owner 0
    map->cells = (Cell *) calloc(rows * cols, sizeof(Cell));

    return map;
}

// Function that clears all possible moving or disappearing cells of the map
void clear_map(Map *map)
{
    // Check every cell
    for(int y = 0; y < map->rows; y++)
    {
        for(int x = 0; x < map->cols; x++)
        {
            Cell *currentCell = index_map(map, y, x);

            switch(currentCell->type)
            {
                case CELL_FOOD:
                    currentCell->type = CELL_DIRT;
                    break;

                case CELL_ANT:
                    currentCell->type = CELL_DIRT;
                    currentCell->owner = 0;
                    break;
            }
        }
    }

    return;
}

// Function for accessing the right cell in the map
Cell *index_map(Map *map, int row, int col)
{
    return &map->cells[row * map->cols + col];
}

// Function for freeing all allocated map and cell memory
void cleanup_map(Map *map)
{
    // Free all allocated cell memory
    free(map->cells);

    // Free the allocated map memory
    free(map);

    return;
}

// Function for printing a basic representation of the map status
void print_map(Map *map, FILE *outputFile)
{
    fprintf(outputFile, "rows %u\n", map->rows);
    fprintf(outputFile, "cols %u\n", map->cols);

    for(int y = 0; y < map->rows; y++)
    {
        fprintf(outputFile, "m ");
        for(int x = 0; x < map->cols; x++)
        {
            Cell *currentCell = index_map(map, y, x);

            switch(currentCell->type)
            {
                case CELL_DIRT:
                    fprintf(outputFile, ".");
                    break;

                case CELL_WATER:
                    fprintf(outputFile, "%%");
                    break;

                case CELL_FOOD:
                    fprintf(outputFile, "*");
                    break;

                case CELL_HILL:
                    fprintf(outputFile, "%u", currentCell->owner);
                    break;

                case CELL_ANT:
                    fprintf(outputFile, "%c", (currentCell->owner + 97));    // 97-106 is a-j in ASCII
                    break;

                case CELL_ANT_ON_OWN_HILL:
                    fprintf(outputFile, "%c", (currentCell->owner + 65));    // 65-74 is A-J in ASCII
                    break;
            }
        }
        fprintf(outputFile, "\n");
    }

    fflush(outputFile);

    return;
}