#ifndef PIECE
#define PIECE

#include <stdbool.h>
#include <stdlib.h>

typedef struct Piece Piece;
struct Piece {
  bool king;
  int player;
};

Piece *createPiece(int player);

#endif