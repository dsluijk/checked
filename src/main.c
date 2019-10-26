#include "main.h"

/**
 * Main function.
 * This kicks of all processes for the game.
 */
int main() {
  Board board = createBoard();
  char *state = makeBoard(board);
  printf("%s", state);

  startSSH();

  exit(0);
}