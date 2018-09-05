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
#include <arpa/inet.h>


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

    int sockfd, n;
    char mesg[buf_size], ipadr[16];
    struct sockaddr_in servaddr;
    struct sockaddr_in cliaddr;

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket problem");
        exit(1);
    }

    memset(&servaddr, 0, kSize);
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);

    if (bind(sockfd, (struct sockaddr *)&servaddr, kSize) < 0) {
        perror("bind problem");
        exit(1);
    }
    printf("SERVER starts...\n");

    while (1) {

        unsigned int len = kSize;

        if ((n = recvfrom(sockfd, mesg, buf_size, 0, (struct sockaddr *)&cliaddr, &len)) < 0) {
            perror("recvfrom");
            exit(1);
        }
        mesg[n] = 0;

        printf( "REQUEST %s      FROM %s : %d\n", 
                mesg,
                inet_ntop(AF_INET, (void *)&cliaddr.sin_addr.s_addr, ipadr, 16),
                ntohs(cliaddr.sin_port)
                );

        if (sendto(sockfd, mesg, n, 0, (struct sockaddr *)&cliaddr, len) < 0) {
            perror("sendto");
            exit(1);
        }
    }
}