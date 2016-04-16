#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "utilities.h"

int main(int argc, char **argv) {
	char ch;
	char buffer[MAXMSGLENGTH];
	char message[MAXMSGLENGTH];
	char *addr = "127.0.0.1";
	int port = 7000;
	int ADDRESSFLAG, PORTFLAG, sockfd, maxfd, nready;
	
	fd_set readfds;
	
	while((ch = getopt(argc, argv, "a:p:")) != -1) {
		switch (ch) {
			case 'p':
				PORTFLAG = 1;
				port = atoi(optarg);
				break;
			case 'a':
				ADDRESSFLAG = 1;
				addr = optarg;
				break;
			default:
				die("Usage: chat_client [-a ADDRESS] [-p PORT] \n");
		}
	}
	
	//check our agruments
	if (!ADDRESSFLAG) {
		printf("No address specified, defaulting to localhost. \n");
	}
	if (!PORTFLAG) {
		printf("No port specified, defaulting to 7000.\n");
	}
	if((sockfd = connect_to_server(addr, port)) < 0) {
		die("");
	}
	
	printf("Connected!\n");
	
	//keep track of stdin and the socket
	FD_ZERO(&readfds);
	FD_SET(STDIN_FILENO, &readfds);
	FD_SET(sockfd, &readfds);
	maxfd = sockfd;
	
	while(1) {
		nready = select(maxfd + 1, &readfds, NULL, NULL, NULL);
		
		//todo why is stdin never ready?
		//if the user typed a message
		if(FD_ISSET(STDIN_FILENO, &readfds)) {
			printf("sending message...\n");
			if(read_message(STDIN_FILENO, message) < 0) {
				log_out(sockfd);
				die("STDIN closed, exiting\n");
			}
			if(send_message(sockfd, message) < 0) {
				log_out(sockfd);
				die("Server closed connection, exiting\n");
			}
			printf("message sent.\n");
			if(--nready <= 0) { continue; }
		}
		//if the server has sent us a message
		if(FD_ISSET(sockfd, &readfds)) {
			if(read_message(sockfd, buffer) == 0) {
				//don't print a message we sent
				//if(strcmp(message, buffer) != 0) { printf("%s\n", buffer); }
				printf("message received.\n");
				if(--nready <= 0) { continue; }
			}
			
		}
	}
	return 0;
}
	
