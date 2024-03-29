#include "ssh.h"

Board *waiting;

/**
 * Closing SSH callback.
 * Cleans up any connections.
 * @param session The session to close.
 * @param channel The channel to close.
 * @param userdata The client to close, in form of the userdata.
 */
void sshClose(ssh_session session, ssh_channel channel, void *userdata) {
  Client *client = (Client *)userdata;

  if (client == waiting->player1) {
    free(waiting);
    waiting = NULL;
  } else {
    if(client->player == 0) {
      ssh_event_remove_session(*client->board->player2->event, ssh_channel_get_session(*client->board->player2->chan));
    } else {
      ssh_event_remove_session(*client->board->player1->event, ssh_channel_get_session(*client->board->player1->chan));
    }
  }
}

/**
 * Data avaiable callback.
 * Fired when the client send data over SSH.
 * @param sess Session from which the data comes from.
 * @param chan Channel which send the data.
 * @param data Pointer to the data from the client.
 * @param len Length of the data
 * @param id_stderr If the data is going to stderr.
 * @param userdata Userdata, this is the client.
 * @return the amount of read bytes.
 */
static int sshRead(ssh_session sess, ssh_channel chan, void *data, uint32_t len,
                   int is_stderr, void *userdata) {
  char *p = data;
  int key = *(p + len - 1);
  clientInput((Client *)userdata, key);

  return 1;
}

/**
 * Handle incomming connections.
 * @param session The new session.
 */
void handleConnection(ssh_session session) {
  ssh_message message;
  ssh_channel chan = 0;
  char buf[2048];
  char *user;

  if (ssh_handle_key_exchange(session)) {
    printf("ssh_handle_key_exchange: %s\n", ssh_get_error(session));
    return;
  }

  bool done = false;
  do {
    message = ssh_message_get(session);

    if (!message) {
      printf("auth error: %s\n", ssh_get_error(session));
      ssh_disconnect(session);
      return;
    }

    switch (ssh_message_type(message)) {
    case SSH_REQUEST_AUTH:
      switch (ssh_message_subtype(message)) {
      case SSH_AUTH_METHOD_NONE:
        user = malloc(strlen(ssh_message_auth_user(message)) + 1);
        memcpy(user, ssh_message_auth_user(message),
               strlen(ssh_message_auth_user(message)));
        ssh_message_auth_reply_success(message, 0);
        done = true;
      default:
        ssh_message_auth_set_methods(message, SSH_AUTH_METHOD_NONE);
        ssh_message_reply_default(message);
        break;
      }
      break;
    default:
      ssh_message_reply_default(message);
    }

    ssh_message_free(message);
  } while (!done);

  do {
    message = ssh_message_get(session);
    if (message) {
      switch (ssh_message_type(message)) {
      case SSH_REQUEST_CHANNEL_OPEN:
        if (ssh_message_subtype(message) == SSH_CHANNEL_SESSION) {
          chan = ssh_message_channel_request_open_reply_accept(message);
          break;
        }
      default:
        ssh_message_reply_default(message);
      }
      ssh_message_free(message);
    }
  } while (message && !chan);

  if (!chan) {
    printf("error : %s\n", ssh_get_error(session));
    ssh_finalize();
    return;
  }

  struct ssh_channel_callbacks_struct cb = {
      .channel_data_function = sshRead,
      .channel_eof_function = sshClose,
      .channel_close_function = sshClose,
      .userdata = NULL,
  };

  ssh_session sess = ssh_channel_get_session(chan);
  ssh_callbacks_init(&cb);
  ssh_set_channel_callbacks(chan, &cb);

  short events = POLLIN | POLLPRI | POLLERR | POLLHUP | POLLNVAL;

  ssh_event event = ssh_event_new();
  if (event == NULL) {
    printf("Couldn't get a event\n");
    return;
  }

  if (ssh_event_add_session(event, sess) != SSH_OK) {
    printf("Couldn't add the session to the event\n");
    return;
  }

  Client *client = newClient(user, &chan, &event);

  if (waiting == NULL) {
    waiting = createBoard();
    client->board = waiting;
    client->player = 0;
    waiting->player1 = client;
  } else {
    client->board = waiting;
    client->player = 1;
    waiting->player2 = client;
    startGame(waiting);
    waiting = NULL;
  }

  cb.userdata = client;
  makeBoard(client);

  do {
    ssh_event_dopoll(event, 1000);
  } while (!ssh_channel_is_closed(chan));

  ssh_event_remove_session(event, sess);
  ssh_event_free(event);

  ssh_disconnect(session);
}

/**
 * Start the SSH server
 */
void startSSH() {
  ssh_bind sshbind = ssh_bind_new();
  int port = SSHD_PORT;

  ssh_bind_options_set(sshbind, SSH_BIND_OPTIONS_BINDPORT, &port);
  ssh_bind_options_set(sshbind, SSH_BIND_OPTIONS_HOSTKEY, "ssh-rsa");
  ssh_bind_options_set(sshbind, SSH_BIND_OPTIONS_RSAKEY, KEYS_FILE);

  if (ssh_bind_listen(sshbind) < 0) {
    printf("Error listening to socket: %s\n", ssh_get_error(sshbind));
    return;
  }

  while (1) {
    pthread_t thread;
    ssh_session session = ssh_new();
    int r = ssh_bind_accept(sshbind, session);

    if (r == SSH_ERROR) {
      printf("error accepting a connection : %s\n", ssh_get_error(sshbind));
      return;
    }

    pthread_create(&thread, NULL, handleConnection, session);
  }
}