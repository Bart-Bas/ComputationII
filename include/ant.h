#ifndef _ANT_H_
#define _ANT_H_

#include <stdlib.h>
#include <stdio.h>

// Enums
typedef enum CellType
{
    CELL_DIRT,
    CELL_WATER,
    CELL_FOOD,
    CELL_HILL,
    CELL_UNSEEN
} CellType;

typedef enum AntType
{
    ANT_NONE,
    ANT_LIVE,
    ANT_DEAD

} AntType;

// Data structures
typedef struct Cell
{
    CellType type;
    unsigned int owner;
} Cell;

typedef struct Ant
{
    AntType type;
    unsigned int owner;
} Ant;

typedef struct Map
{
    Cell *cells;
    Ant *ants;
    unsigned int rows;
    unsigned int cols;
} Map;

typedef struct GameSettings
{
    unsigned int loadtime;
    unsigned int turntime;
    unsigned int rows;
    unsigned int cols;
    unsigned int turns;
    unsigned int viewradius2;
    unsigned int attackradius2;
    unsigned int spawnradius2;
    size_t player_seed;
} GameSettings;

// Singly Circular Linked List of ants
typedef struct AntItem
{
    unsigned int row;
    unsigned int col;
} AntItem;

typedef struct AntNode
{
    struct AntNode *nextNode;
    struct AntItem *antItem;

} AntNode;

typedef struct AntList
{
    int counter;
    AntNode *headNode;
} AntList;

// Function prototypes
GameSettings *initialize_game();
void cleanup_game();
int read_turn();
Map *initialize_map(int rows, int cols);
void clear_map(Map *map);
Cell *index_cell(Map *map, int row, int col);
Ant *index_ant(Map *map, int row, int col);
int wrap_number(int input, int max);
void cleanup_map();
void print_map(Map *map, FILE *outputFile);
AntList *initialize_ant_list();
void cleanup_ant_list(AntList *antList);
void insert_ant(AntList *antList, int row, int col);
void remove_ant(AntList *antList, AntNode *delNode);
AntNode *find_ant(AntList *antList, int row, int col);
int move_ant(Map *map, AntItem *antItem, char direction);
int check_move(Map *map, int newRow, int newCol);

#endif // _ANT_H_