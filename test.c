#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "utilities.h"

int test_read(char *buffer);
int test_write(char *buffer);
int test_connect(char *addr, int port);
//broadcast, logout and die do not require tests

int main() {
	char buffer[MAXMSGLENGTH];
	if(test_read(buffer) == 0) {
		printf("Test successful\n");
	}
	if(test_write(buffer) == 0) {
		printf("Test successful\n");
	}
	if(test_connect("127.0.0.1", 7000) == 0) {
		printf("Test successful\n");
	}
}

int test_read(char *buffer) {
	if(read_message(STDIN_FILENO, buffer) == 0) {
		return 0;
	}
	printf("Test unsuccessful\n");
	return -1;
}

int test_write(char *buffer) {
	if(send_message(STDOUT_FILENO, buffer) == 0) {
		return 0;
	}
	printf("Test unsuccessful\n");
	return -1;
}

int test_connect(char *addr, int port) {
	int sockfd;
	if((sockfd = connect_to_server(addr, port)) == 0) {
		close(sockfd);
		return 0;
	}
	return -1;
}
