#include "../include/ant.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

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
        // Process the input
        // Skip empty lines
        if(strcmp(line, "\n") != 0)
        {
            // Separate the key token
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
int read_turn(Map *map, AntList *ownLiveAnts, int *antIds)
{
    int turn;

    // Initialize to 0 so that getline automatically allocates enough memory
    char *line = NULL;
    size_t lineLength = 0;

    // Read line by line
    while(getline(&line, &lineLength, stdin) != -1)
    {
        // Process the input
        // Skip empty lines
        if(strcmp(line, "\n") != 0)
        {
            // Separate the key token
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
                Cell *cell = index_cell(map, row, col);

                if(ant->owner == 0 && cell->type == CELL_HILL)
                {
                    // Add own live ants from the list
                    // Add only ants that may be new (spawn at an own hill)
                    // Ant didn't exist yet (an existing ant could walk over the hill)
                    if(cell->owner == 0)
                    {
                        if(find_ant(ownLiveAnts, row, col) == NULL)
                        {
                            insert_ant(ownLiveAnts, row, col, *antIds);
                            (*antIds)++;
                        }
                    }
                    // Change CELL_HILL into CELL_DIRT if an own ant walks over an enemy hill
                    else
                    {
                        cell->type = CELL_DIRT;
                    }
                }
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
    // By using calloc everything initializes to 0 which means type CELL_DIRT and owner 0
    map->cells = (Cell *) calloc(rows * cols, sizeof(Cell));

    // Allocate ant memory
    // By using calloc everything initializes to 0 which means type ANT_NONE and owner 0
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
                fprintf(outputFile, "%u", currentCell->owner);
            }
            else if(currentAnt->type == ANT_DEAD)
            {
                // Dead ant
                fprintf(outputFile, "!");
            }
            else if(currentAnt->type == ANT_LIVE)
            {
                if(currentCell->type == CELL_HILL && currentCell->owner == currentAnt->owner)
                {
                    // Live ant on own hill
                    fprintf(outputFile, "%c", (currentCell->owner + 65));    // 65-74 is A-J in ASCII
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

// Function that prints out a visual representation of the list of ants
void print_ant_list(AntList *antList, FILE *outputFile)
{
    // Only if there are ant nodes
    if(antList->headNode != NULL)
    {
        fprintf(outputFile, "Number of ants in list: %d\n", antList->counter);

        AntNode *currentNode = antList->headNode;

        // Loop through the list
        do
        {
            AntItem *antItem = currentNode->antItem;

            if(currentNode == antList->headNode)
                fprintf(outputFile, "HeadNode: %p\n", currentNode);
            else
                fprintf(outputFile, "Node: %p\n", currentNode);

            fprintf(outputFile, "  ->nextnode: %p\n", currentNode->nextNode);
            fprintf(outputFile, "  ->antitem: %p\n", currentNode->antItem);
            fprintf(outputFile, "      ->id: %d\n", currentNode->antItem->id);
            fprintf(outputFile, "      ->row: %d\n", currentNode->antItem->row);
            fprintf(outputFile, "      ->col: %d\n", currentNode->antItem->col);
            fprintf(outputFile, "      ->ld: %c\n", currentNode->antItem->lastDirection);
            fprintf(outputFile, "      ->lt: %d\n", currentNode->antItem->lastTurnMoved);
            fprintf(outputFile, "      ->hg: %d\n", currentNode->antItem->hasGoal);
            fprintf(outputFile, "      ->rg: %d\n", currentNode->antItem->rowGoal);
            fprintf(outputFile, "      ->cg: %d\n", currentNode->antItem->colGoal);

            // Go to the next node
            currentNode = currentNode->nextNode;
        } while(currentNode != antList->headNode);
    }
}

// Function for inserting an ant to the list
void insert_ant(AntList *antList, int row, int col, int id)
{
    // Make a new node with item
    AntNode *newNode = (AntNode *) malloc(sizeof(AntNode));
    AntItem *newItem = (AntItem *) malloc(sizeof(AntItem));
    newNode->antItem = newItem;
    newItem->id = id;
    newItem->row = row;
    newItem->col = col;
    newItem->lastDirection = 0;
    newItem->hasGoal = 0;
    newItem->rowGoal = row;
    newItem->colGoal = col;

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
        // Insert it between the head node and the node after the head node to keep the circle complete
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
        // This is the last node of the list
        if(antList->headNode == antList->headNode->nextNode)
        {
            free(antList->headNode->antItem);
            free(antList->headNode);
            antList->headNode = NULL;
        }
        else
        {
            // Delete the node by copying the data from the next node to the current node and removing the next node
            // Must be done because you don't have pointers to the previous node
            // Be careful, external pointers become invalid in this way!

            AntNode *tempNode = delNode->nextNode;
            free(delNode->antItem);
            delNode->antItem = delNode->nextNode->antItem;
            delNode->nextNode = delNode->nextNode->nextNode;

            // Change the head node if necessary
            if(antList->headNode == tempNode)
                antList->headNode = tempNode->nextNode;

            free(tempNode);
        }

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
int move_ant(Map *map, AntItem *antItem, char direction, int turn)
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
    else
    {
        // Change nothing and return 0 because of invalid move
        return 0;
    }

    // Update the other ant properties
    antItem->lastTurnMoved = turn;
    antItem->lastDirection = direction;

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

// Function to give an ant x chances to take a random valid direction
void random_move_ant(Map *map, AntItem *antItem, int turn, int chances)
{
    int randNumber = rand() % 4;

    for(int i = 0; i < chances; i++)
    {
        if(randNumber == 0)
        {
            if(move_ant(map, antItem, 'N', turn));
                break;
        }
        else if(randNumber == 1)
        {
            if(move_ant(map, antItem, 'E', turn));
                break;
        }
        else if(randNumber == 2)
        {
            if(move_ant(map, antItem, 'S', turn));
                break;
        }
        else if(randNumber == 3)
        {
            if(move_ant(map, antItem, 'W', turn));
                break;
        }

        randNumber = rand() % 4;
    }

    return;
}

// Function that checks if CELL_FOOD or CELL_HILL is nearby
// Returns 1 if there was a goal
int check_for_goal(Map *map, AntItem *antItem, int goalRadius)
{
    int randNumber = rand() % 4;
    int rowMin = antItem->row - goalRadius;
    int rowMax = antItem->row + goalRadius;
    int colMin = antItem->col - goalRadius;
    int colMax = antItem->col + goalRadius;

    // Different orders to search the cells so that not every ant finds the same goal
    if(randNumber == 0)
    {
        for(int y = rowMin; y < rowMax; y++)
        {
            for(int x = colMin; x < colMax; x++)
            {
                Cell *goalCell = index_cell(map, wrap_number(y, map->rows), wrap_number(x, map->cols));

                // Check if cell is a valid goal
                if((goalCell->type == CELL_HILL && goalCell->owner != 0) || goalCell->type == CELL_FOOD)
                {
                    antItem->rowGoal = wrap_number(y, map->rows);
                    antItem->colGoal = wrap_number(x, map->cols);
                    if(check_for_goal_reachable(map, antItem, goalRadius))
                        return 1;
                }
            }
        }
    }
    else if(randNumber == 1)
    {
        for(int y = rowMax; y > rowMin; y--)
        {
            for(int x = colMin; x < colMax; x++)
            {
                Cell *goalCell = index_cell(map, wrap_number(y, map->rows), wrap_number(x, map->cols));

                // Check if cell is a valid goal
                if((goalCell->type == CELL_HILL && goalCell->owner != 0) || goalCell->type == CELL_FOOD)
                {
                    antItem->rowGoal = wrap_number(y, map->rows);
                    antItem->colGoal = wrap_number(x, map->cols);
                    if(check_for_goal_reachable(map, antItem, goalRadius))
                        return 1;
                }
            }
        }
    }
    else if(randNumber == 2)
    {
        for(int y = rowMin; y < rowMax; y++)
        {
            for(int x = colMax; x > colMin; x--)
            {
                Cell *goalCell = index_cell(map, wrap_number(y, map->rows), wrap_number(x, map->cols));

                // Check if cell is a valid goal
                if((goalCell->type == CELL_HILL && goalCell->owner != 0) || goalCell->type == CELL_FOOD)
                {
                    antItem->rowGoal = wrap_number(y, map->rows);
                    antItem->colGoal = wrap_number(x, map->cols);
                    if(check_for_goal_reachable(map, antItem, goalRadius))
                        return 1;
                }
            }
        }
    }
    else if(randNumber == 3)
    {
        for(int y = rowMax; y > rowMin; y--)
        {
            for(int x = colMax; x > colMin; x--)
            {
                Cell *goalCell = index_cell(map, wrap_number(y, map->rows), wrap_number(x, map->cols));

                // Check if cell is a valid goal
                if((goalCell->type == CELL_HILL && goalCell->owner != 0) || goalCell->type == CELL_FOOD)
                {
                    antItem->rowGoal = wrap_number(y, map->rows);
                    antItem->colGoal = wrap_number(x, map->cols);
                    if(check_for_goal_reachable(map, antItem, goalRadius))
                        return 1;
                }
            }
        }
    }

    return 0;
}

// Function that checks if a goal is reachable (no CELL_WATER in the way)
// Returns 1 if the goal is reachable
int check_for_goal_reachable(Map *map, AntItem *antItem, int goalRadius)
{
    char direction1;
    char direction2;
    direction_for_goal(map, antItem, &direction1, &direction2);

    if(direction1 == 'N' || direction2 == 'N')
        for(int y = 0; y < goalRadius; y++)
            for(int x = (antItem->col - 1); x < (antItem->col + 1); x++)
                if(index_cell(map, wrap_number(antItem->row - y, map->rows), wrap_number(x, map->cols))->type == CELL_WATER)
                    return 0;

    if(direction1 == 'E' || direction2 == 'E')
        for(int y = 0; y < goalRadius; y++)
            for(int x = (antItem->row - 1); x < (antItem->row + 1); x++)
                if(index_cell(map, wrap_number(x, map->rows), wrap_number(antItem->col + y, map->rows))->type == CELL_WATER)
                    return 0;

    if(direction1 == 'S' || direction2 == 'S')
        for(int y = 0; y < goalRadius; y++)
            for(int x = (antItem->col - 1); x < (antItem->col + 1); x++)
                if(index_cell(map, wrap_number(antItem->row + y, map->rows), wrap_number(x, map->cols))->type == CELL_WATER)
                    return 0;

    if(direction1 == 'W' || direction2 == 'W')
        for(int y = 0; y < goalRadius; y++)
            for(int x = (antItem->row - 1); x < (antItem->row + 1); x++)
                if(index_cell(map, wrap_number(x, map->rows), wrap_number(antItem->col - y, map->rows))->type == CELL_WATER)
                    return 0;

    return 1;
}

// Function that determines the right direction to the goal
void direction_for_goal(Map *map, AntItem *antItem, char *direction1, char *direction2)
{
    int rowDiffUnwrap = abs(antItem->row - antItem->rowGoal);
    int rowDiffWrap = abs(map->rows - rowDiffUnwrap);
    int colDiffUnwrap = abs(antItem->col - antItem->colGoal);
    int colDiffWrap = abs(map->cols - colDiffUnwrap);
    char horizontalDirection;
    char verticalDirection;

    // Calculate vertical direction
    if(antItem->row > antItem->rowGoal)
    {
        // Goal in N direction
        if(rowDiffUnwrap > rowDiffWrap)
            // It is shorter to pick the wrapped route
            verticalDirection = 'S';
        else
            verticalDirection = 'N';
    }
    else
    {
        // Goal in S direction
        if(rowDiffUnwrap > rowDiffWrap)
            // It is shorter to pick the wrapped route
            verticalDirection = 'N';
        else
            verticalDirection = 'S';
    }

    // Calculate horizontal direction
    if(antItem->col > antItem->colGoal)
    {
        // Goal in W direction
        if(colDiffUnwrap > colDiffWrap)
            // It is shorter to pick the wrapped route
            horizontalDirection = 'E';
        else
            horizontalDirection = 'W';
    }
    else
    {
        // Goal in E direction
        if(colDiffUnwrap > colDiffWrap)
            // It is shorter to pick the wrapped route
            horizontalDirection = 'W';
        else
            horizontalDirection = 'E';
    }

    // Give vertical movement the biggest priority
    if(fmin(rowDiffUnwrap, rowDiffWrap) > fmin(colDiffUnwrap, colDiffWrap))
    {
        *direction1 = verticalDirection;
        *direction2 = horizontalDirection;
    }
    // Give horizontal movement the biggest priority
    else
    {
        *direction1 = horizontalDirection;
        *direction2 = verticalDirection;
    }

    return;
}