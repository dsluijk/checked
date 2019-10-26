#include <stdio.h>
#include "board.h"

/**
 * Creates a new playing board.
 * The size is defined by BOARD_SIZE in board.h, the amount of layers is defined by PLAYER_LAYERS.
 * A piece is represented by the Piece struct, if there is no piece at the coordinate it will be NULL.
 * White (player 1) is on top.
 * @return The new board.
 */
Board createBoard() {
  struct Board board;

  for(int i = 0; i < BOARD_SIZE; i++) {
    int player;
    if(i < PLAYER_LAYERS) {
      player = 1;
    } else if(i >= BOARD_SIZE - PLAYER_LAYERS) {
      player = 2;
    } else {
      player = 0;
    }

    for(int j = 0; j < BOARD_SIZE; j++) {
      if((i + 1) % 2 == j % 2 && player != 0) {
        board.pieces[i][j] = createPiece(player - 1);
      } else {
        board.pieces[i][j] = NULL;
      }
    }
  }

  return board;
};

/**
 * Print a horizontal divider line.
 * This is a helper function for printBoard().
 * @param board The current state of the board to print.
 */
void _printDivider(Board board) {
  for(int j = 0; j < BOARD_SIZE * 3; j++) {
    if(j % 3 == 0) {
      printf("+");
    } else {
      printf("-");
    }
  }

  printf("+\n");
}

/**
 * Print a piece from a coordinate on the screen.
 * This is a helper function for printBoard().
 * @param board The current state of the board to print.
 * @param i The I coordinate of the piece to print.
 *  This correspondends to the y axis.
 * @param j The J coordinate of the piece to print.
 *  This correspondends to the x axis.
 */
void _printPiece(Board board, int i, int j) {
  Piece* pieceRef = board.pieces[i][j];
  if (pieceRef == NULL) {
    printf(" ");
    return;
  }

  Piece piece = (*pieceRef);
  if (piece.king == false) {
    if(piece.player == 0) {
      printf("\u25A0"); // ■
    } else {
      printf("\u25C6"); // ◆
    }
  } else {
    if(piece.player == 0) {
      printf("\u25A3"); // ▣
    } else {
      printf("\u25C8"); // ◈
    }
  }
}

/**
 * Print the current board to STDOUT.
 * @param board The current state of the board to print.
 */
void printBoard(Board board) {
  for(int i = 0; i < BOARD_SIZE; i++) {
    _printDivider(board);

    for(int j = 0; j < BOARD_SIZE * 3; j++) {
      if(j % 3 == 0) {
        printf("|");
      } else {
        _printPiece(board, i, (j - j % 3) / 3);
      }
    }
  
    printf("|\n");
  }

  _printDivider(board);
};