#ifndef SSH
#define SSH

#include <poll.h>
#include <pthread.h>
#include <pty.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libssh/callbacks.h>
#include <libssh/libssh.h>
#include <libssh/server.h>

#include "client.h"

#define LIBSSH_STATIC 1
#define SSHD_PORT 4242
#define KEYS_FILE "./keys/rsa"

void startSSH();
void sshClose(ssh_session session, ssh_channel channel, void *userdata);

#endif