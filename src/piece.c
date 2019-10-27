#include "piece.h"

#include <stdio.h>

/**
 * Create a new piece
 * @param player This is the player for which this is.
 *  It should be either a 1 or a 0.
 * @return A pointer to the piece.
 */
Piece *createPiece(int player) {
  Piece *piece = malloc(sizeof(Piece));
  piece->king = false;
  piece->player = player;

  return piece;
};