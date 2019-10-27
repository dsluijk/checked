#include "board.h"

/**
 * Creates a new playing board.
 * The size is defined by BOARD_SIZE in board.h, the amount of layers is defined
 * by PLAYER_LAYERS. A piece is represented by the Piece struct, if there is no
 * piece at the coordinate it will be NULL. White (player 1) is on top.
 * @return The new board.
 */
Board *createBoard() {
  Board *board = malloc(sizeof(Board) + BOARD_SIZE * BOARD_SIZE);
  board->started = false;
  board->player1left = (BOARD_SIZE / 2) * PLAYER_LAYERS;
  board->player2left = (BOARD_SIZE / 2) * PLAYER_LAYERS;

  for (int i = 0; i < BOARD_SIZE; i++) {
    int player = 0;
    if (i < PLAYER_LAYERS) {
      player = 1;
    } else if (i >= BOARD_SIZE - PLAYER_LAYERS) {
      player = 2;
    }

    for (int j = 0; j < BOARD_SIZE; j++) {
      if ((i + 1) % 2 == j % 2 && player != 0) {
        board->pieces[i][j] = createPiece(player - 1);
      } else {
        board->pieces[i][j] = NULL;
      }
    }
  }

  return board;
};

/**
 * Write a horizontal divider to the cursor.
 * This is a helper function for writeBoard().
 * @param cursor The cursor where to write on the buffer.
 * @return a modified cursor, to be used as new starting possition.
 */
char *_writeDivider(char *cursor) {
  for (int j = 0; j < BOARD_SIZE * 3; j++) {
    if (j % 3 == 0) {
      *cursor = '+';
      cursor++;
    } else {
      *cursor = '-';
      cursor++;
    }
  }

  *cursor = '+';
  cursor++;
  *cursor = '\n';
  cursor++;
  *cursor = '\r';
  cursor++;

  return cursor;
}

/**
 * Write a piece to the char sequence.
 * This is a helper function for makeBoard().
 * @param cursor The pointer to the current write possition of the buffer.
 * @param board The current state of the board to print.
 * @param i The I coordinate of the piece to print.
 *  This correspondends to the y axis.
 * @param j The J coordinate of the piece to print.
 *  This correspondends to the x axis.
 * @return a modified cursor, to be used as new starting possition.
 */
char *_writePiece(char *cursor, Board *board, int i, int j, Client *client) {
  Piece *piece = board->pieces[i][j];

  if(i == client->y && j == client->x) {
    if(client->selected) {
      *cursor = '\xE2';
      cursor += 1;
      *cursor = '\x96';
      cursor += 1;
      *cursor = '\xB2';
      cursor += 1;
    } else {
      *cursor = '\xE2';
      cursor += 1;
      *cursor = '\x96';
      cursor += 1;
      *cursor = '\xB3';
      cursor += 1;
    }
    return cursor;
  }

  if (piece == NULL) {
    *cursor = ' ';
    cursor += 1;
    return cursor;
  }

  if (piece->king == false) {
    if (piece->player == 0) {
      // ■
      // It's a hack, but I don't know of any way else to do this.
      *cursor = '\xE2';
      cursor += 1;
      *cursor = '\x96';
      cursor += 1;
      *cursor = '\xA0';
      cursor += 1;
    } else {
      // ◆
      *cursor = '\xE2';
      cursor += 1;
      *cursor = '\x97';
      cursor += 1;
      *cursor = '\x86';
      cursor += 1;
    }
  } else {
    if (piece->player == 0) {
      // ▣
      *cursor = '\xE2';
      cursor += 1;
      *cursor = '\x96';
      cursor += 1;
      *cursor = '\xa3';
      cursor += 1;
    } else {
      // ◈
      *cursor = '\xE2';
      cursor += 1;
      *cursor = '\x97';
      cursor += 1;
      *cursor = '\x88';
      cursor += 1;
    }
  }

  return cursor;
}

/**
 * Put the current state of the current board in a buffer.
 * @param board The current state of the board to print.
 * @return A pointer to the start of a null-terminated char sequence.
 */
char *makeBoard(Client *client) {
  Board *board = client->board;
  char *buffer = malloc(BOARD_BUFFER_SIZE);
  char *start = buffer;

  for (int i = 0; i < BOARD_SIZE; i++) {
    buffer = _writeDivider(buffer);

    for (int j = 0; j < BOARD_SIZE * 3; j++) {
      if (j % 3 == 0) {
        *buffer = '|';
        buffer += 1;
      } else {
        buffer = _writePiece(buffer, board, i, (j - (j % 3)) / 3, client);
      }
    }

    *buffer = '|';
    buffer++;
    *buffer = '\n';
    buffer++;
    *buffer = '\r';
    buffer++;
  }

  _writeDivider(buffer);
  return start;
};

void startGame(Board* board) {
  board->started = true;
  board->activeSide = 0;
  boardRender(board);
  printf("starting!\n");
}

void boardRender(Board* board) {
  char *board1 = makeBoard(board->player1);
  char *board2 = makeBoard(board->player2);

  ssh_channel_write(*board->player1->chan, board1, strlen(board1));
  ssh_channel_write(*board->player2->chan, board2, strlen(board2));
}