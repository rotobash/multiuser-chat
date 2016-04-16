#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "utilities.h"

void listen_for_connections(int sockfd, int maxclnts) {	
	int connected, nready, maxfd, maxi, i;
	
	struct sockaddr_in clntaddr;
	struct timeval tv;
	Client clients[maxclnts];
	fd_set clientreadset, allset;
	
	//wait for a second before continuing on to the next fd
	tv.tv_sec = 1;
	tv.tv_usec = 0;
	
	FD_ZERO(&allset);
	FD_SET(sockfd, &allset);
	
	if(listen(sockfd, 5) < 0) {
		die("Call to listen failed.\n");
	}
	
	
	for(i = 0; i < maxclnts; i++) {
		bzero(clients[i].buf, MAXMSGLENGTH);
		clients[i].sock = -1;
	}
	
	printf("Listening for clients.\n");
	maxfd = sockfd;
	maxi = -1;
	
	while(1) { 
		clientreadset = allset;
		nready = select(maxfd+1, &clientreadset, NULL, NULL, &tv);
		
		//accept a client
		if (FD_ISSET(sockfd, &clientreadset)) {
			printf("accepting a new client\n");
			connected = accept(sockfd, (struct sockaddr *) &clntaddr, (socklen_t *)SIZE);
			printf("accepted a new client\n");
			for (i = 0; i < maxclnts; i++) {
				//store the connection
				if (clients[i].sock < 0) {
					clients[i].sock = connected;
					break;
				}
			}
			if (i == maxclnts) {
				printf("too many clients");
			}
			
			if (i > maxi) { maxi = i; }
			
			FD_SET(connected, &allset);
			
			if (connected > maxfd) { maxfd = connected; }
			
			if (--nready <= 0) { continue; }
		}

		//check if any client has sent us a message
		for (i = 0; i <= maxi; i++) {
			if (clients[i].sock < 0) { continue; }
			
			if (FD_ISSET(clients[i].sock, &clientreadset)) {
				int result = read_message(clients[i].sock, clients[i].buf);
				
				if(result == -1)  {
					printf("Client disconnected");
					close(clients[i].sock);
					FD_CLR(clients[i].sock, &allset);
					clients[i].sock = -1;
				}
				printf("%s\n", clients[i].buf);
				clients[i].beenread = 1;
				
				if (--nready <= 0) { break; }
			}
		}
		for(i = 0; i <= maxi; i++) {
			if(clients[i].beenread) {
				broadcast_to_clients(clients[i].buf, clients, maxi);
				clients[i].beenread = 0;
			}	
		}
	}
}

		

int main(int argc, char **argv) {
	char ch;
	int sockfd;
	int port = 7000;
	int MAXCLIENTS = 10;
	
	int PORTFLAG = 0;
	int MAXCLIENTSFLAG = 0;
	
	struct sockaddr_in server;	

	while((ch = getopt(argc, argv, "p:m:")) != -1) {
		switch (ch) {
			case 'p':
				PORTFLAG = 1;
				port = atoi(optarg);
				break;
			case 'm':
				MAXCLIENTSFLAG = 1;
				MAXCLIENTS = atoi(optarg);
				break;
			default:
				die("Usage: chat_server [-p PORT] [-m MAXCLIENTS] \n");
		}
	}

	//check our arguments
	if (!PORTFLAG) {
		printf("No port specified, defaulting to 7000.\n");
	}
	if (!MAXCLIENTSFLAG) {
		printf("No maximum number of clients specified, defaulting to 10.\n");
	}	

	//init server struct
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr = INADDR_ANY;
	
	printf("Establishing socket..\n");
	
	//init socket
	if((sockfd=socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		die("Failed to open socket.\n");
	}
	if(bind(sockfd, (struct sockaddr *)&server, SIZE) < 0) {
		fprintf(stderr, "Could not bind to port %d.\n", port);
		die("\n");
	}
	printf("Established socket.\n");
	//main loop
	listen_for_connections(sockfd, MAXCLIENTS);
	return 0;
}
