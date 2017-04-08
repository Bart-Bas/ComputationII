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
int read_turn(Map *map, AntList *ownLiveAnts)
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
                // Game is over (set turn to -1)
                turn = -1;
                break;
            }
            else if(strcmp(key_token, "w") == 0)
            {
                // Water
                int row = strtoul(strtok(NULL, " \n"), NULL, 10);
                int col = strtoul(strtok(NULL, " \n"), NULL, 10);
                Cell *cell = index_cell(map, row, col);
                cell->type = CELL_WATER;
            }
            else if(strcmp(key_token, "f") == 0)
            {
                // Food
                int row = strtoul(strtok(NULL, " \n"), NULL, 10);
                int col = strtoul(strtok(NULL, " \n"), NULL, 10);
                Cell *cell = index_cell(map, row, col);
                cell->type = CELL_FOOD;
            }
            else if(strcmp(key_token, "h") == 0)
            {
                // Ant hill
                int row = strtoul(strtok(NULL, " \n"), NULL, 10);
                int col = strtoul(strtok(NULL, " \n"), NULL, 10);
                int owner = strtoul(strtok(NULL, " \n"), NULL, 10);
                Cell *cell = index_cell(map, row, col);
                cell->type = CELL_HILL;
                cell->owner = owner;
            }
            else if(strcmp(key_token, "a") == 0)
            {
                // Live ant
                int row = strtoul(strtok(NULL, " \n"), NULL, 10);
                int col = strtoul(strtok(NULL, " \n"), NULL, 10);
                int owner = strtoul(strtok(NULL, " \n"), NULL, 10);
                Ant *ant = index_ant(map, row, col);
                ant->type = ANT_LIVE;
                ant->owner = owner;

                // Add own live ants from the list
                // Add only ants that may be new (spawn at an own hill)
                // Ant didn't exist yet (an existing ant could walk over the hill)
                if(ant->owner == 0)
                    if(index_cell(map, row, col)->type == CELL_HILL)
                        if(find_ant(ownLiveAnts, row, col) == NULL)
                            insert_ant(ownLiveAnts, row, col);
            }
            else if(strcmp(key_token, "d") == 0)
            {
                // Dead ant
                int row = strtoul(strtok(NULL, " \n"), NULL, 10);
                int col = strtoul(strtok(NULL, " \n"), NULL, 10);
                int owner = strtoul(strtok(NULL, " \n"), NULL, 10);
                Ant *ant = index_ant(map, row, col);
                ant->type = ANT_DEAD;
                ant->owner = owner;

                // Delete own dead ants from the list
                if(ant->owner == 0)
                    remove_ant(ownLiveAnts, find_ant(ownLiveAnts, row, col));
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
    // By using calloc everyting initilizes to 0 which means type CELL_DIRT and owner 0
    map->cells = (Cell *) calloc(rows * cols, sizeof(Cell));

    // Allocate ant memory
    // By using calloc everyting initilizes to 0 which means type ANT_NONE and owner 0
    map->ants = (Ant *) calloc(rows * cols, sizeof(Ant));

    return map;
}

// Function that clears all non static data on the map
void clear_map(Map *map)
{
    // Check every cell
    for(int y = 0; y < map->rows; y++)
    {
        for(int x = 0; x < map->cols; x++)
        {
            Cell *currentCell = index_cell(map, y, x);
            Ant *currentAnt = index_ant(map, y, x);

            // Food may be gone
            if(currentCell->type == CELL_FOOD)
                currentCell->type = CELL_DIRT;

            // Hill may be razed
            if(currentCell->type == CELL_HILL)
                currentCell->type = CELL_DIRT;

            // Ant may have moved or died
            // Also the stored dead ants are only from the last turn
            currentAnt->type = ANT_NONE;
            currentAnt->owner = 0;
        }
    }

    return;
}

// Function for accessing the right cell on the map
Cell *index_cell(Map *map, int row, int col)
{
    // The world map is wrapped
    row = wrap_number(row, map->rows);
    col = wrap_number(col, map->cols);

    return &map->cells[row * map->cols + col];
}

// Function for accessing the right ant on the map
Ant *index_ant(Map *map, int row, int col)
{
    // The world map is wrapped
    row = wrap_number(row, map->rows);
    col = wrap_number(col, map->cols);

    return &map->ants[row * map->cols + col];
}

// Function that wraps a row or col number
int wrap_number(int input, int max)
{
    if(input >= max)
        return input - max;
    if(input < 0)
        return input + max;

    return input;
}

// Function for freeing all allocated map, cell and ant memory
void cleanup_map(Map *map)
{
    // Free all allocated cell memory
    free(map->cells);

    // Free all allocated ant memory
    free(map->ants);

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
            Cell *currentCell = index_cell(map, y, x);
            Ant *currentAnt = index_ant(map, y, x);

            if(currentCell->type == CELL_UNSEEN && currentAnt->type == ANT_NONE)
            {
                // Unseen territory
                fprintf(outputFile, "?");
            }
            else if(currentCell->type == CELL_DIRT && currentAnt->type == ANT_NONE)
            {
                // Land
                fprintf(outputFile, ".");
            }
            else if(currentCell->type == CELL_WATER)
            {
                // Water
                fprintf(outputFile, "%%");
            }
            else if(currentCell->type == CELL_FOOD)
            {
                // Food
                fprintf(outputFile, "*");
            }
            else if(currentCell->type == CELL_HILL && currentAnt->type == ANT_NONE)
            {
                // Hill
                fprintf(outputFile, "%u", currentAnt->owner);
            }
            else if(currentAnt->type == ANT_DEAD)
            {
                // Dead ant
                fprintf(outputFile, "!");
            }
            else if(currentAnt->type == ANT_LIVE)
            {
                if(currentCell->type == CELL_HILL)
                {
                    // Live ant on own hill
                    fprintf(outputFile, "%c", (currentAnt->owner + 65));    // 65-74 is A-J in ASCII
                }
                else
                {
                    // Live ant
                    fprintf(outputFile, "%c", (currentAnt->owner + 97));    // 97-106 is a-j in ASCII
                }
            }
        }
        fprintf(outputFile, "\n");
    }

    fflush(outputFile);

    return;
}

// Function for allocating and initializing a list of ants
AntList *initialize_ant_list()
{
    AntList *antList = (AntList *) calloc(1, sizeof(AntList));
    antList->counter = 0;
    return antList;
}

// Function for freeing all allocated memory for the list
void cleanup_ant_list(AntList *antList)
{
    // Only if there are nodes in the list
    if(antList->headNode != NULL)
    {
        // Set the first node to delete
        AntNode *delNode = antList->headNode->nextNode;
        antList->headNode->nextNode = NULL;

        // Delete all nodes
        while(delNode != NULL)
        {
            // Remember the next node
            AntNode *nextNode = delNode->nextNode;

            // Free the item of the node
            free(delNode->antItem);
            // Free the node itself
            free(delNode);

            // Set the next node
            delNode = nextNode;
        }

        // Set the head pointer to NULL to indicate empty list
        antList->headNode = NULL;

        // Reset the list counter
        antList->counter = 0;
    }

    // Free the list itself
    free(antList);

    return;
}

// Function for inserting an ant to the list
void insert_ant(AntList *antList, int row, int col)
{
    // Make a new node with item
    AntNode *newNode = (AntNode *) malloc(sizeof(AntNode));
    AntItem *newItem = (AntItem *) malloc(sizeof(AntItem));
    newNode->antItem = newItem;
    newItem->row = row;
    newItem->col = col;

    // Insert it in the list
    if(antList->headNode == NULL)
    {
        // It was the first node
        newNode->nextNode = newNode;
        antList->headNode = newNode;
    }
    else
    {
        // It was not the first node
        // Insert it between de head node and the node after the head node to keep the circle complete
        newNode->nextNode = antList->headNode->nextNode;
        antList->headNode->nextNode = newNode;
    }

    // Update the list node counter
    antList->counter++;

    return;
}

// Function for removing an ant from the list
void remove_ant(AntList *antList, AntNode *delNode)
{
    // Only if the node exists
    if(delNode != NULL)
    {
        // Delete the node by copying the data from the next node to the current node and removing the next node
        // Must be done because you don't have pointers to the prev node
        // Be carefull, external pointers become invalid in this way!
        AntNode *tempNode = delNode->nextNode;
        free(delNode->antItem);
        delNode->antItem = delNode->nextNode->antItem;
        delNode->nextNode = delNode->nextNode->nextNode;
        free(tempNode);

        // Change the head node if del node was the last node
        if(tempNode == antList->headNode)
            antList->headNode = NULL;

        // Update the list node counter
        antList->counter--;
    }

    return;
}

// Function for searching an ant with particular coordinates in the list
AntNode *find_ant(AntList *antList, int row, int col)
{
    // Only if there are nodes in the list
    if(antList->headNode != NULL)
    {
        AntNode *currentNode = antList->headNode;

        // Loop through the list
        do
        {
            // Check the coordinates
            if(currentNode->antItem->row == row && currentNode->antItem->col == col)
                return currentNode;

        // Go to the next node
        currentNode = currentNode->nextNode;

        } while(currentNode != antList->headNode);
    }

    // Not found, return 0
    return NULL;
}

// Function that gives the server a move command and updates the ant item in the list and the map with the ants new position
// If the move is not valid it returns 0, makes no changes and doesn't send a move command
int move_ant(Map *map, AntItem *antItem, char direction)
{
    int row = antItem->row;
    int col = antItem->col;

    if(direction == 'N')
    {
        int newRow = wrap_number(antItem->row - 1, map->rows);

        if(check_move(map, newRow, col))
        {
            // Update the position of the ant in the list
            antItem->row = newRow;
            // Update the map
            index_ant(map, row, col)->type = ANT_NONE;
            index_ant(map, newRow, col)->type = ANT_LIVE;
        }
        else
            // Change nothing and return 0 because of invalid move
            return 0;
    }
    else if(direction == 'E')
    {
        int newCol = wrap_number(antItem->col + 1, map->cols);

        if(check_move(map, row, newCol))
        {
            // Update the position of the ant in the list
            antItem->col = newCol;
            // Update the map
            index_ant(map, row, col)->type = ANT_NONE;
            index_ant(map, row, newCol)->type = ANT_LIVE;
        }
        else
            // Change nothing and return 0 because of invalid move
            return 0;
    }
    else if(direction == 'S')
    {
        int newRow = wrap_number(antItem->row + 1, map->rows);

        if(check_move(map, newRow, col))
        {
            // Update the position of the ant in the list
            antItem->row = newRow;
            // Update the map
            index_ant(map, row, col)->type = ANT_NONE;
            index_ant(map, newRow, col)->type = ANT_LIVE;
        }
        else
            // Change nothing and return 0 because of invalid move
            return 0;
    }
    else if(direction == 'W')
    {
        int newCol = wrap_number(antItem->col - 1, map->cols);

        if(check_move(map, row, newCol))
        {
            // Update the position of the ant in the list
            antItem->col = newCol;
            // Update the map
            index_ant(map, row, col)->type = ANT_NONE;
            index_ant(map, row, newCol)->type = ANT_LIVE;
        }
        else
            // Change nothing and return 0 because of invalid move
            return 0;
    }

    // Send move command to the server
    printf("o %u %u %c\n", row, col, direction);
    fflush(stdout);

    return 1;
}

// Function to check if a move is valid, it makes sure there is no other ant and only dirt or hill at the new position
// Returns 1 if the move is valid
int check_move(Map *map, int newRow, int newCol)
{
    if((index_cell(map, newRow, newCol)->type == CELL_DIRT || index_cell(map, newRow, newCol)->type == CELL_HILL) && index_ant(map, newRow, newCol)->type != ANT_LIVE)
        return 1;

    return 0;
}