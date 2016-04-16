#define MAXMSGLENGTH 512
#define SIZE sizeof(struct sockaddr_in)

typedef struct {
	int sock;
	char buf[MAXMSGLENGTH];
	int beenread;
} Client;

void die(char *msg);

int send_message(int sockfd, char *message);
int read_message(int sockfd, char *buffer);
int broadcast_to_clients(char *message, Client *clients, int clientlen);

int log_out(int sock);

int connect_to_server(char *address, int port);
