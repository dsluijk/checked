#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <poll.h>
#include <pty.h>

#include <libssh/libssh.h>
#include <libssh/server.h>
#include <libssh/callbacks.h>

#define LIBSSH_STATIC 1
#define SSHD_USER "libssh"
#define SSHD_PASSWORD "libssh"
#define SSHD_PORT 4242
#define KEYS_FILE "./keys/rsa"

void startSSH();