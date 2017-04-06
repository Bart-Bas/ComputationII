#include "../include/ant.h"
#include <stdio.h>

Map *map;
GameSettings *gameSettings;
int turn = 0;

int main()
{
    gameSettings = initialize_game();
    map = initialize_map(gameSettings->rows, gameSettings->cols);

    printf("go\n");
    fflush(stdout);

    fprintf(stderr, "Map initialized!\n");

    while(turn != -1)
    {
        clear_map(map);
        turn = read_turn(map);
        printf("go\n");
        fflush(stdout);
        if(turn == -1)
            fprintf(stderr, "Game ended!\n");
        else
            //fprintf(stderr, "Turn: %d\n", turn);
            print_map(map, stderr);
    }

    print_map(map, stderr);

    cleanup_game(gameSettings);
    cleanup_map(map);

    return 0;
}