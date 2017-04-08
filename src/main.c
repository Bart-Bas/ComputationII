#include "../include/ant.h"
#include <stdio.h>
#include <stdlib.h>

#define _DEBUG_

// Global variables
Map *map;
GameSettings *gameSettings;
int turn = 1;
AntList *ownLiveAnts;

// Function prototypes
void send_commands();

// Main function
int main()
{
    // Start the game and read initial info from the server
    gameSettings = initialize_game();
    map = initialize_map(gameSettings->rows, gameSettings->cols);
    ownLiveAnts = initialize_ant_list();

    #ifdef _DEBUG_
    fprintf(stderr, "Game initialized!\n\n");
    #endif

    // Send go command to the server
    printf("go\n");
    fflush(stdout);

    // Actual main loop
    while(1)
    {
        // Delete all non static data on the map
        clear_map(map);

        // Read the turn information from the server
        // This updates the map and deletes dead ants from the list
        turn = read_turn(map, ownLiveAnts);

        // Break out the while loop if the game has ended
        if(turn == -1)
            break;

        #ifdef _DEBUG_
        fprintf(stderr, "Turn: %d\n", turn);
        fprintf(stderr, "Map and list updated!\n");
        print_map(map, stderr);
        #endif

        // Send move commands to the server
        send_commands();

        #ifdef _DEBUG_
        fprintf(stderr, "Move commands sent!\n\n");
        #endif

        // Send go command to the server
        printf("go\n");
        fflush(stdout);
    }

    // Cleanup after the game
    cleanup_game(gameSettings);
    cleanup_map(map);
    cleanup_ant_list(ownLiveAnts);

    #ifdef _DEBUG_
    fprintf(stderr, "Game cleaned up!\n");
    #endif

    return 0;
}

// Function for sending move commands to the server
// Actial AI is in here!
void send_commands()
{
    // Only if there are own live ants
    if(ownLiveAnts->headNode != NULL)
    {
        AntNode *currentNode = ownLiveAnts->headNode;

        // Loop through the list with all own live ants
        do
        {
            int row = currentNode->antItem->row;
            int col = currentNode->antItem->col;
            int randNumber = rand() % 4;

            // 5 Chances to take a random valid direction
            for(int i = 0; i < 5; i++)
            {
                if(randNumber == 0)
                {
                    if(move_ant(map, currentNode->antItem, 'N'));
                        break;
                }
                else if(randNumber == 1)
                {
                    if(move_ant(map, currentNode->antItem, 'E'));
                        break;
                }
                else if(randNumber == 2)
                {
                    if(move_ant(map, currentNode->antItem, 'S'));
                        break;
                }
                else if(randNumber == 3)
                {
                    if(move_ant(map, currentNode->antItem, 'W'));
                        break;
                }

                randNumber = rand() % 4;
            }

            // Go to the next node
            currentNode = currentNode->nextNode;
        } while(currentNode != ownLiveAnts->headNode);
    }

    return;
}