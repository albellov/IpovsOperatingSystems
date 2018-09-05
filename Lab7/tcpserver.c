#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <getopt.h>
#include <stdbool.h>


int main(int argc, char *argv[]) {

    size_t buf_size = -1;
    size_t port = -1;
      
    while (true) {

        const char* short_options = "p:b:";

        static struct option options[] = {
                          {"port", required_argument, 0, 'p'},
                          {"bufsize", required_argument, 0, 'b'},
                          {0, 0, 0, 0}
                        };

        int option_index = 0;
        int c = getopt_long(argc, argv, short_options, options, &option_index);

        if (c == -1)
          break;

        switch (c) {
            case 0: {
                switch (option_index) {
                    case 0:
                        port = atoi(optarg);
                        if (port <= 0) {
                            printf("port is a positive number\n");
                            return 1;
                        }
                        break;
                    case 1:
                        buf_size = atoi(optarg);
                        if (buf_size <= 0) {
                            printf("buf_size is a positive number\n");
                            return 1;
                        }
                        break;
                    default:
                        printf("Index %d is out of options\n", option_index);
                }
            } break;
            case 'p':
                port = atoi(optarg);
                if (port <= 0) {
                    printf("port is a positive number\n");
                    return 1;
                }
                break;
            case 'b':
                buf_size = atoi(optarg);
                if (buf_size <= 0) {
                    printf("buf_size is a positive number\n");
                    return 1;
                }
                break;
            case '?':
                printf("Arguments error\n");
                break;
            default:
                fprintf(stderr, "getopt returned character code 0%o?\n", c);
        }
    }

    if (buf_size == -1 || port == -1 ) {
      fprintf(  stderr, 
            "Using: %s --port \"port\" --bufsize \"size\"\n",
            argv[0]
          );
      return 1;
    }

    const size_t kSize = sizeof(struct sockaddr_in);

    int lfd, cfd;
    int nread;
    char buf[buf_size];
    struct sockaddr_in servaddr;
    struct sockaddr_in cliaddr;

    if ((lfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }

    memset(&servaddr, 0, kSize);
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);

    if (bind(lfd, (struct sockaddr *)&servaddr, kSize) < 0) {
        perror("bind");
        exit(1);
    }

    if (listen(lfd, 5) < 0) {
        perror("listen");
        exit(1);
    }

    while (true) {

        unsigned int clilen = kSize;

        if ((cfd = accept(lfd, (struct sockaddr *)&cliaddr, &clilen)) < 0) {
            perror("accept");
            exit(1);
        }
        printf("connection established\n");

        while ((nread = read(cfd, buf, buf_size)) > 0) {
            write(1, &buf, nread);
        }

        if (nread == -1) {
            perror("read");
            exit(1);
        }
        close(cfd);
        
    }
}
