CC = gcc
LIBS = -lm -lpthread -lrt

all: client server dph
.PHONY: all

client: client.c
	gcc client.c -o client -lm -lpthread -lrt

server: server.c
	gcc server.c -o server -lm -lpthread -lrt

dph: dph.c
	gcc dph.c -o dph -lm -lpthread -lrt

.PHONY: clean
clean:
	rm client server dph
