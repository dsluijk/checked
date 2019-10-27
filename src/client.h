#ifndef CLIENT
#define CLIENT

#include "board.h"
#include "ssh.h"

typedef struct Board Board;
typedef struct Client Client;
// Defined a client in the game.
// It also stores information on the SSH connection.
struct Client {
  // Name of the client, as defined by SSH.
  char *name;
  // Board the client is participating in.
  Board *board;
  // SSH channel of the client.
  ssh_channel *chan;
  // SSH session of the client.
  ssh_session *sess;
  // Player ID in the game.
  int player;
  // X axis of the cursor.
  int x;
  // Y axis of the cursor.
  int y;
  // Indicates if a piece has been selected.
  bool selected;
  // X axis of the selected piece.
  int selectedX;
  // Y axis of the selected piece.
  int selectedY;
};

void clientInput(Client *client, int key);
Client* newClient();

#endif