#include "ssh.h"

static void chan_close(ssh_session session, ssh_channel channel, void *userdata) {
    int fd = *(int*)userdata;
    (void)session;
    (void)channel;

    close(fd);
}

static int copy_chan_to_fd(ssh_session session,
                                           ssh_channel channel,
                                           void *data,
                                           uint32_t len,
                                           int is_stderr,
                                           void *userdata) {
    int fd = *(int*)userdata;
    int sz;
    (void)session;
    (void)channel;
    (void)is_stderr;

    char buf2[2048];
    memcpy(buf2, data, len);
    printf("%#02x\n", buf2[len-1]);
    if(buf2[len-1] == 67) {
      printf("Hello\n");
    }
    // sz = write(fd, data, len);
    return sz;
}

static int copy_fd_to_chan(socket_t fd, int revents, void *userdata) {
    ssh_channel chan = (ssh_channel)userdata;
    char buf[2048];
    int sz = 0;

    if(!chan) {
        close(fd);
        return -1;
    }

    if(revents & POLLIN) {
        sz = read(fd, buf, 2048);
        if(sz > 0) {
            ssh_channel_write(chan, buf, sz);
        }
    }

    if(revents & POLLHUP) {
        ssh_channel_close(chan);
        sz = -1;
    }

    return sz;
}

void startSSH() {
  ssh_session session;
  ssh_bind sshbind;
  ssh_message message;
  ssh_channel chan = 0;
  char buf[2048];
  int auth = 0;
  int sftp = 0;
  int i;
  int r;

  sshbind = ssh_bind_new();
  session = ssh_new();

  int port = 4242;
  ssh_bind_options_set(sshbind, SSH_BIND_OPTIONS_BINDPORT, &port);
  ssh_bind_options_set(sshbind, SSH_BIND_OPTIONS_HOSTKEY, "ssh-rsa");
  ssh_bind_options_set(sshbind, SSH_BIND_OPTIONS_RSAKEY, KEYS_FILE);

  if (ssh_bind_listen(sshbind) < 0) {
    printf("Error listening to socket: %s\n", ssh_get_error(sshbind));
    return 1;
  }

  r = ssh_bind_accept(sshbind, session);
  if (r == SSH_ERROR) {
    printf("error accepting a connection : %s\n", ssh_get_error(sshbind));
    return 1;
  }

  if (ssh_handle_key_exchange(session)) {
    printf("ssh_handle_key_exchange: %s\n", ssh_get_error(session));
    return 1;
  }

  do {
    message = ssh_message_get(session);
    if (!message)
      break;

    switch (ssh_message_type(message)) {
    case SSH_REQUEST_AUTH:
      switch (ssh_message_subtype(message)) {
      case SSH_AUTH_METHOD_NONE:
        printf("Logging in as %s\n", ssh_message_auth_user(message));
        auth = 1;
        ssh_message_auth_reply_success(message, 0);
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
  } while (!auth);

  if (!auth) {
    printf("auth error: %s\n", ssh_get_error(session));
    ssh_disconnect(session);
    return 1;
  }

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
    return 1;
  }

  do {
    message = ssh_message_get(session);
    if (message && ssh_message_type(message) == SSH_REQUEST_CHANNEL && ssh_message_subtype(message) == SSH_CHANNEL_REQUEST_PTY) {
      ssh_message_channel_request_reply_success(message);
      break;
    }

    ssh_message_reply_default(message);
    
    
    ssh_message_free(message);
  } while (message && !sftp);

  printf("it works!\n");




  struct ssh_channel_callbacks_struct cb = {
	.channel_data_function = copy_chan_to_fd,
    .channel_eof_function = chan_close,
    .channel_close_function = chan_close,
	.userdata = NULL 
};

  ssh_session sess = ssh_channel_get_session(chan);
    socket_t fd;
    struct termios *term = NULL;
    struct winsize *win = NULL;
    pid_t childpid;
    ssh_event event;
    short events;

    cb.userdata = &fd;
    ssh_callbacks_init(&cb);
    ssh_set_channel_callbacks(chan, &cb);

    events = POLLIN | POLLPRI | POLLERR | POLLHUP | POLLNVAL;

    event = ssh_event_new();
    if(event == NULL) {
        printf("Couldn't get a event\n");
        return -1;
    }

    if(ssh_event_add_fd(event, fd, events, copy_fd_to_chan, chan) != SSH_OK) {
        printf("Couldn't add an fd to the event\n");
        return -1;
    }

    if(ssh_event_add_session(event, sess) != SSH_OK) {
        printf("Couldn't add the session to the event\n");
        return -1;
    }

    do {
        ssh_event_dopoll(event, 1000);
    } while(!ssh_channel_is_closed(chan));

    ssh_event_remove_fd(event, fd);
    ssh_event_remove_session(event, sess);
    ssh_event_free(event);





  do {
    i = ssh_channel_read(chan, buf, 2048, 0);
    if (i > 0) {
      // ssh_channel_write(chan, state, BOARD_BUFFER_SIZE);
      // if (write(1, buf, i) < 0) {
      //   printf("error writing to buffer\n");
      //   return 1;
      // }
    }
  } while (i > 0);

  ssh_disconnect(session);
  ssh_bind_free(sshbind);
  ssh_finalize();
}