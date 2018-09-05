CC=gcc
CFLAGS=-I.

server: server.c libshared.so
	$(CC) -o server server.c -lpthread -L. -lshared -Wl,-rpath,.

client: client.c libshared.so
	$(CC) -o client client.c -lpthread -L. -lshared -Wl,-rpath,.

exec_s: exec_servers.c
	$(CC) -o exec_s exec_servers.c -lpthread

clean:
	rm server client libshared.so

all: server client
    
reset: clean all

libshared.so: slib.o
	$(CC) -shared -o libshared.so slib.o
	rm slib.o

slib.o: shared_lib.c
	$(CC) -c -fPIC shared_lib.c -o slib.o