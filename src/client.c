#include "client.h"

/**
 * Handle placement of a piece.
 * @param client the client which placed a piece.
 */
void handlePlacement(Client *client) {
  Board *board = client->board;
  int xdiff = client->x - client->selectedX;
  int ydiff = client->y - client->selectedY;
  int direction = client->player == 0 ? 1 : -1;
  bool hasKing = board->pieces[client->selectedY][client->selectedX]->king;

  if (board->pieces[client->y][client->x] != NULL) {
    return;
  }

  if (abs(xdiff) == 1 && (ydiff == direction || (hasKing && abs(ydiff) == 1))) {
    board->pieces[client->y][client->x] = board->pieces[client->selectedY][client->selectedX];
    board->pieces[client->selectedY][client->selectedX] = NULL;
    board->activeSide = client->player == 1 ? 0 : 1;
  } else if (abs(xdiff) == 2 && (ydiff == direction * 2 || (hasKing && abs(ydiff) == 2))) {
    if (board->pieces[client->y - (ydiff / 2)][client->x - (xdiff / 2)] == NULL) {
      return;
    }

    board->pieces[client->y][client->x] = board->pieces[client->selectedY][client->selectedX];
    board->pieces[client->selectedY][client->selectedX] = NULL;
    board->activeSide = client->player == 1 ? 0 : 1;

    if (board->activeSide == 0) {
      board->player1left--;
    } else {
      board->player2left--;
    }
  }
}

/**
 * Handle client input.
 * @param client The client to handle for.
 * @param key The pressed key.
 */
void clientInput(Client *client, int key) {
  if (!client->board->started) {
    return;
  }

  switch (key) {
  // Ctrl+C
  case 3:
    printf("%s wants to exit!\n", client->name);
    // ssh_channel_close(*client->chan);
    // ssh_channel_free(*client->chan);
    // ssh_disconnect(*client->sess);
    // ssh_event_free

    // ssh_event_remove_session(*client->event, *client->sess);
    // ssh_event_free(*client->event);
    ssh_disconnect(*client->sess);
    // ssh_free(*client->sess);
    // sshClose(*client->sess, *client->chan, client);
    break;
  // Spacebar
  case 32:
    if (client->board->activeSide != client->player) {
      break;
    }

    if (client->selected) {
      handlePlacement(client);
      client->selected = false;
    } else if (client->board->pieces[client->y][client->x] != NULL && client->board->pieces[client->y][client->x]->player == client->player) {
      client->selectedX = client->x;
      client->selectedY = client->y;
      client->selected = true;
    }

    boardRender(client->board);
    break;
  // Top arrow
  case 65:
    if (client->y > 0) {
      client->y--;
    }

    boardRender(client->board);
    break;
  // Right arrow
  case 66:
    if (client->y < 9) {
      client->y++;
    }

    boardRender(client->board);
    break;
  // Right arrow
  case 67:
    if (client->x < 9) {
      client->x++;
    }

    boardRender(client->board);
    break;
  // Right arrow
  case 68:
    if (client->x > 0) {
      client->x--;
    }

    boardRender(client->board);
    break;
  default:
    printf("Unknown key %#4x from %s\n", key, client->name);
    break;
  }
}

/**
 * Create a new client.
 * @param user The username of the client.
 * @param chan The channel for the client.
 * @param sess The session for the client.
 * @return The new client.
 */
Client *newClient(char *user, ssh_channel *chan, ssh_session *sess) {
  Client *client = malloc(sizeof(Client));
  client->name = user;
  client->chan = chan;
  client->sess = sess;
  client->x = 5;
  client->y = 5;
  client->selectedX = -1;
  client->selectedY = -1;
  client->selected = false;
}