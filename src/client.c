#include "client.h"

void handlePlacement(Client *client) {
  int xdiff = client->x - client->selectedX;
  int ydiff = client->y - client->selectedY;
  int direction = client->player == 0 ? 1 : -1;
  bool hasKing = client->board->pieces[client->selectedY][client->selectedX]->king;

  if(client->board->pieces[client->y][client->x] != NULL) {
    return;
  }

  if(abs(xdiff) == 1 && (ydiff == direction || (hasKing && abs(ydiff) == 1))) {
    client->board->pieces[client->y][client->x] = client->board->pieces[client->selectedY][client->selectedX];
    client->board->pieces[client->selectedY][client->selectedX] = NULL;
    client->board->activeSide = client->player == 1 ? 0 : 1;
  } else if(abs(xdiff) == 2 && (ydiff == direction * 2 || (hasKing && abs(ydiff) == 2))) {
    if(client->board->pieces[client->y - (ydiff / 2)][client->x - (xdiff / 2)] == NULL) {
      return;
    }

    client->board->pieces[client->y][client->x] = client->board->pieces[client->selectedY][client->selectedX];
    client->board->pieces[client->selectedY][client->selectedX] = NULL;
    client->board->activeSide = client->player == 1 ? 0 : 1;

    if(client->board->activeSide == 0) {
      client->board->player1left--;
    } else {
      client->board->player2left--;
    }
  }
}

void clientInput(Client* client, int key) {
  if(!client->board->started) {
    return;
  }

  switch(key) {
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
      if(client->board->activeSide != client->player) {
        break;
      }
    
      if(client->selected) {
        handlePlacement(client);
        client->selected = false;
      } else if(client->board->pieces[client->y][client->x] != NULL && client->board->pieces[client->y][client->x]->player == client->player) {
        client->selectedX = client->x;
        client->selectedY = client->y;
        client->selected = true;
      }

      boardRender(client->board);
      break;
    // Top arrow
    case 65:
      if(client->y > 0) {
        client->y--;
      }

      boardRender(client->board);
      break;
    // Right arrow
    case 66:
      if(client->y < 9) {
        client->y++;
      }

      boardRender(client->board);
      break;
    // Right arrow
    case 67:
      if(client->x < 9) {
        client->x++;
      }

      boardRender(client->board);
      break;
    // Right arrow
    case 68:
      if(client->x > 0) {
        client->x--;
      }

      boardRender(client->board);
      break;
    default:
      printf("Unknown key %#4x from %s\n", key, client->name);
      break;
  }
}