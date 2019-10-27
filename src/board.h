#ifndef BOARD
#define BOARD

#include <stdio.h>

#include "client.h"
#include "piece.h"
#include "ssh.h"

#define BOARD_SIZE 10
#define PLAYER_LAYERS 3
// Maximum buffer size for the board output.
#define BOARD_BUFFER_SIZE ((BOARD_SIZE * 3 + 1) * (BOARD_SIZE * 5 + 1)) + 1

typedef struct Client Client;
typedef struct Board Board;
/**
 * Registers the state lof the board.
 */
struct Board {
  // Stores the state of each peice of the board.
  Piece *pieces[BOARD_SIZE][BOARD_SIZE];
  // Indicates if the game has started yet.
  bool started;
  // Indicates which player is currently setting.
  int activeSide;
  // Indicates how many pieces player 1 has left.
  // Tracked for performance reasons.
  int player1left;
  // Indicates how many pieces player 2 has left.
  // Tracked for performance reasons.
  int player2left;
  // The client for player 1.
  Client *player1;
  // The client for player 2.
  Client *player2;
};

Board *createBoard();
char *makeBoard(Client *client);
void startGame(Board *board);
void boardRender(Board *board);

#endif