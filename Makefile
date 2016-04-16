CC = gcc
CFLAGS = -Wall -Werror -g
SRC =  utilities.c
OBJ =  utilities.o

all: chat_server chat_client test_utils

chat_server: server.o ${OBJ}
	$(CC) $(CFLAGS) -o chat_server server.o utilities.o

chat_client: client.o ${OBJ}
	$(CC) $(CFLAGS) -o chat_client client.o ${OBJ}
	
test_utils: test.o ${OBJ}
	$(CC) $(CFLAGS) -o test_utils test.o ${OBJ}
	
test.o: test.c ${OBJ}
	$(CC) $(CFLAGS) -c test.c

server.o: server.c ${OBJ}
	$(CC) $(CFLAGS) -c server.c
	
client.o: client.c ${OBJ}
	$(CC) $(CFLAGS) -c client.c 

utilities.o: utilities.c utilities.h
	$(CC) $(CFLAGS) -c utilities.c

clean: 
	rm chat_client chat_server *.o
cleano: 
	rm *.o
