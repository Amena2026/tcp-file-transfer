CC = gcc
CFLAGS = -Wall

all: client/TCPEchoClient server/TCPEchoServer

client/TCPEchoClient: client/TCPEchoClient.c server/HandleTCPClient.c server/DieWithError.c
	$(CC) $(CFLAGS) -o client/TCPEchoClient client/TCPEchoClient.c server/HandleTCPClient.c server/DieWithError.c

server/TCPEchoServer: server/TCPEchoServer.c server/HandleTCPClient.c server/DieWithError.c
	$(CC) $(CFLAGS) -o server/TCPEchoServer server/TCPEchoServer.c server/HandleTCPClient.c server/DieWithError.c

clean:
	rm -f client/TCPEchoClient server/TCPEchoServer
