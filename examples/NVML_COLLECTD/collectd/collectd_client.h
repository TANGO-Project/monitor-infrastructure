#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>


// Path to collectd-unixsock
#define SOCK_PATH "/var/run/collectd-unixsock"


// FUNCTIONS //////////////////////////////////////////////////////////////////
/*
 * Function: concat two char*
 */
char* concat(const char*, const char*);

/*
 * Function: create UNIX socket
 * Doc: http://man7.org/linux/man-pages/man2/socket.2.html
 */
int createUNIXSocket();

/*
 * Function: close socket
 *
 */
void closeUNIXSocket(int);

/*
 * Function: Connect to Collectd UNIXSOCKET plugin
 */
int connectToCollectd(int, struct sockaddr_un);

/*
 * Function: send command to unixsock
 */
void sendCommand(int, char*);

/*
 * Function: it only prints the command that should be send to Collectd
 */
void sendFakeCommand(int, char*);
