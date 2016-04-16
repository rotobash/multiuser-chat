#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "utilities.h"

void die(char *message) {
	perror(message);
	exit(1);
}

int send_message(int sockfd, char *message) {
	int bytes_sent; 
	int msglen = strlen(message);
	while (msglen > 0) {
		bytes_sent = write(sockfd, message + bytes_sent, msglen);
		if(bytes_sent < 0) {
			continue;
		}
		msglen -= bytes_sent;
	}
	return 0;
} 

/*
* Returns 0 if all clients were sent a message and -1 if a client had their connection closed.
*/
int broadcast_to_clients(char* message, Client *clients, int clientlen) {
	int i;
	for(i = 0; i < clientlen; i++) {
		if(clients[i].sock > 0) {
			send_message(clients[i].sock, message);
		}
	}
	return 0;
}

int log_out(int sock) {
	if (close(sock) < 1) {
		return -1;
	}
	return 0;
}

/* Read a message from a given file descriptor into the char array pointed to by buffer.
*  Returns 0 if a whole line was read and -1 otherwise.
*/
int read_message(int sockfd, char *buffer) {
	int bytes_read = 0;
	
	while((bytes_read = read(sockfd, buffer + bytes_read, MAXMSGLENGTH - bytes_read)) > 0) {
		if(strchr(buffer, '\n') || strchr(buffer, '\r')) {
			return 0;
		}
	}
	return -1;
}

/* Connect to the server at a given address and port.
*  Returns the socket file descriptor if the connection was successful and -1 otherwise.
*/
int connect_to_server(char *address, int port) {
	struct sockaddr_in server;
	int sockfd;
	
	printf("Connecting to %s on port %d...\n", address, port);

	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr = inet_addr(address);
	
	if((sockfd=socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		fprintf(stderr, "Failed to open socket.\n");
		return -1;
	}
	if(connect(sockfd, (struct sockaddr *)&server, SIZE) < 0) {
		fprintf(stderr, "Could not connect to address %s at port %d.\n", address, port);
		return -1;
	}
	return sockfd;
}
