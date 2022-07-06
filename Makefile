CC=gcc
CFLAGS=-O3 -Wall
LIBS=-pthread

all: server client
server: server.o string.o
	$(CC) $(CFLAGS) -fsanitize=address $(LIBS) -o server server.o string.o
client: client.o string.o
	$(CC) $(CFLAGS) -fsanitize=address $(LIBS) -o client client.o string.o
server.o: server.c
	$(CC) $(CFLAGS) -c server.c
string.o: string.c
	$(CC) $(CFLAGS) -c string.c
client.o: client.c
	$(CC) $(CFLAGS) -c client.c

.PHONY: clean
clean:
	rm -f *.o *.out
