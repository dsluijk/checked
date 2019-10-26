#ifndef BOARD
#define BOARD

#include "piece.h"

#define BOARD_SIZE 10
#define PLAYER_LAYERS 3

typedef struct Board Board;
struct Board {
  Piece* pieces[BOARD_SIZE][BOARD_SIZE];
};

Board createBoard();
void printBoard(Board board);

#endif