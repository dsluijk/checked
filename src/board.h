#ifndef BOARD
#define BOARD

#include <stdio.h>

#include "piece.h"
#include "ssh.h"

#define BOARD_SIZE 10
#define PLAYER_LAYERS 3
// Maximum buffer size for the board output.
#define BOARD_BUFFER_SIZE ((BOARD_SIZE * 3 + 1) * (BOARD_SIZE * 5 + 1)) + 1

typedef struct Client Client;
typedef struct Board Board;
struct Board {
  Piece *pieces[BOARD_SIZE][BOARD_SIZE];
  Client *player1;
  Client *player2;
};

Board *createBoard();
char *makeBoard(Board *board);
void startGame(Board *board);
void inputBoard(Client *client, int key);

#endif