#ifndef CLIENT
#define CLIENT

#include "board.h"
#include "ssh.h"

typedef struct Board Board;
typedef struct Client Client;
struct Client {
  char *name;
  Board *board;
  ssh_channel *chan;
  ssh_session *sess;
  ssh_event *event;
  int player;
  int x;
  int y;
  int selectedX;
  int selectedY;
  bool selected;
};

void clientInput(Client *client, int key);

#endif