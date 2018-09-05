CC=gcc
CFLAGS=-I.

mutex : mutex.c
	$(CC) -o mutex mutex.c -lpthread $(CFLAGS)

fact : fact.c
	$(CC) -o fact fact.c -lpthread
	
dl : deadlock.c
	$(CC) -o dl deadlock.c -lpthread

clean :
	rm mutex fact dl

all : mutex fact dl
    
reset : clean all
    