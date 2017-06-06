#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
// for linux
#include <unistd.h>
// for Windows
// #include <windows.h>
#define SOCK_PATH "/var/run/collectd-unixsock"


// concat two char*
char* concat(const char *s1, const char *s2) {
    const size_t len1 = strlen(s1);
    const size_t len2 = strlen(s2);
    char *result = malloc(len1+len2+1); //+1 for the zero-terminator. TODO check for errors in malloc here
    memcpy(result, s1, len1);
    memcpy(result+len1, s2, len2+1);	//+1 to copy the null-terminator
    return result;
}


int main(void) {
    int i, s, t, len;
    struct sockaddr_un remote;
	char server_reply[2000];
	char* str;
	char strI[2000] = "PUTVAL leeloo/talytal2/watts N:"; //:7770000991\n";

	// 1- Create unix socket
	printf("------------------------------------------------------------\n");
	printf("[1] Create unix socket ...\n");
    if ((s = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

	// 2- Connect to collectd unix socket
    printf("[2] Trying to connect [%s]...\n", SOCK_PATH);
    remote.sun_family = AF_UNIX;
    strcpy(remote.sun_path, SOCK_PATH);
    len = strlen(remote.sun_path) + sizeof(remote.sun_family);
    if (connect(s, (struct sockaddr *)&remote, len) == -1) {
		printf("> ERROR > connect.\n");
        perror("connect");
        exit(1);
    }
    printf("> Connected to %s.\n", SOCK_PATH);

	// 3- PUTVAL ('random' value; watts from 'my_types.db')
	printf("[3] Send data to Collectd: PUTVAL\n");
	for (i = 01; i < 6; i++) {
		//char str[2000] = concat(strI, "10001\n");
		char strint[15];
		sprintf(strint, "%d", i);
		printf("> [%s] \n", strint);
		
		str = concat(concat(strI, strint), "00000\n");

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
		
		sleep(1);
    }

	// 4- Connect to collectd unix socket
    printf("[4] Closing socket ...\n");
    close(s);
	printf("------------------------------------------------------------\n");

    return 0;
}