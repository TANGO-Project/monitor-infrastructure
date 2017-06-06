#include "collectd_client.h"


/*
 * Function: concat two char*
 */
char* concat(const char *s1, const char *s2) {
    const size_t len1 = strlen(s1);
    const size_t len2 = strlen(s2);
    char *result = malloc(len1+len2+1); //+1 for the zero-terminator. TODO check for errors in malloc here
    memcpy(result, s1, len1);
    memcpy(result+len1, s2, len2+1);	//+1 to copy the null-terminator
    return result;
}

/*
 * Function: create UNIX socket
 * Doc: http://man7.org/linux/man-pages/man2/socket.2.html
 */
int createUNIXSocket() {
	int res;

	printf("[-] Create unix socket ...\n");
	if ((res = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
		printf("> Failed to create new socket\n");
		perror("> ERROR: socket");
	}
	else {
		printf("> Socket created: %d \n", res);
	}

	return res;
}

/*
 * Function: Connect to Collectd UNIXSOCKET plugin
 */
int connectToCollectd(int s, struct sockaddr_un remote) {
	int res, len;

	// 2- Connect to collectd unix socket
	printf("[-] Trying to connect [%s]...\n", SOCK_PATH);
	remote.sun_family = AF_UNIX;
	strcpy(remote.sun_path, SOCK_PATH);
	len = strlen(remote.sun_path) + sizeof(remote.sun_family);
	if ((res = connect(s, (struct sockaddr *)&remote, len)) == -1) {
		printf("> Failed to connect to Collectd\n");
		perror("> ERROR: connect");
	}
	else {
		printf("> Connected to %s.\n", SOCK_PATH);
	}

	return res;
}

/*
 * Function: close socket
 */
void closeUNIXSocket(int s) {
	// 4- Connect to collectd unix socket
	printf("[-] Close socket ...\n");
	close(s);
}

/*
 * Function: send command to unixsock
 */
void sendCommand(int s, char* str) {
	char server_reply[2000];

	printf("[-] Send command to Collectd...");
	// Send command
	if (send(s, str, strlen(str), 0) == -1) {
		printf("> ERROR > send.\n");
		perror("send");
		exit(1);
	}
	else {
		printf("> %s", str);
	}

	// Receive a reply from the server
	if (recv(s , server_reply , 2000 , 0) < 0) {
		printf("> ERROR > recv.\n");
		perror("recv");
		exit(1);
	}
	else {
		printf("< %s", server_reply);
	}
}

/*
 * Function: it only prints the command that should be send to Collectd
 */
void sendFakeCommand(int s, char* str) {
	//printf("[-] Send command to Collectd...");
	printf("> %s\n", str);
}
