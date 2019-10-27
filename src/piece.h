#ifndef PIECE
#define PIECE

#include <stdbool.h>
#include <stdlib.h>

typedef struct Piece Piece;
// Piece of the board.
struct Piece {
  // Indicates if the piece is a king.
  bool king;
  // Indicates to which player it belongs to.
  int player;
};

Piece *createPiece(int player);

#endif