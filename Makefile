CC = gcc

OBJCLIENT = client.o
OBJSERVER = server.o

PROGRAMS = client server

COPT = -Wall

all: ${PROGRAMS}

clean:
	rm -f ${PROGRAMS}

client: 
	${CC} ${COPT} -o client client.c Net.c NetShared.c Checksum.c SlidingWindow.c -lpthread
server: 
	${CC} ${COPT} -o server server.c Net.c NetShared.c Checksum.c SlidingWindow.c -lpthread
