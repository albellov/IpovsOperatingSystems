CC=gcc

tcp: tcpclient tcpserver

udp: udpclient udpserver

tcpclient : tcpclient.c
	$(CC) tcpclient.c -o tcpclient

tcpserver : tcpserver.c
	$(CC) tcpserver.c -o tcpserver

udpclient : udpclient.c
	$(CC) udpclient.c -o udpclient

udpserver : udpserver.c
	$(CC) udpserver.c -o udpserver

all : tcp udp
    
clean :
	rm tcpclient tcpserver udpclient udpserver