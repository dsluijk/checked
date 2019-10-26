#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "board.h"

/**
 * Main function.
 * This kicks of all processes for the game.
 */
int main() {
  Board board = createBoard();
  printBoard(board);

  exit(0);
}