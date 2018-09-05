#include <stdbool.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <getopt.h>


int main(int argc, char **argv) {
  
    size_t buf_size = -1;
    size_t port = -1;
    char ip[255] = {'\0'};
      
    while (true) {
    
        const char* short_options = "i:p:b:";

        static struct option options[] = {
                          {"ip", required_argument, 0, 'i'},
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
                        memcpy(ip, optarg, strlen(optarg));
                        break;
                    case 1:
                        port = atoi(optarg);
                        if (port <= 0) {
                            printf("port is a positive number\n");
                            return 1;
                        }
                        break;
                    case 2:
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
            case 'i':
                 memcpy(ip, optarg, strlen(optarg));
                 break;
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
    if (!strlen(ip)){
        const char* default_ip = "127.0.0.1";
        memcpy(ip, "127.0.0.1", strlen(default_ip));
    }
    if (buf_size == -1 || port == -1) {
        fprintf(  stderr, 
              "Using: %s --ip \"ip\" --port \"port\" --bufsize \"size\"\n",
              argv[0]
            );
        return 1;
    }

    const size_t kSize = sizeof(struct sockaddr_in);

    int sockfd, n;
    char sendline[buf_size], recvline[buf_size + 1];
    struct sockaddr_in servaddr;
    struct sockaddr_in cliaddr;

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);

    if (inet_pton(AF_INET, ip, &servaddr.sin_addr) < 0) {
        perror("inet_pton problem");
        exit(1);
    }

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket problem");
        exit(1);
    }

    write(1, "Enter string\n", 13);

    while ((n = read(0, sendline, buf_size)) > 0) {

        if (sendto(sockfd, sendline, n, 0, (struct sockaddr *)&servaddr, kSize) == -1) {
            perror("sendto problem");
            exit(1);
        }

        if (recvfrom(sockfd, recvline, buf_size, 0, NULL, NULL) == -1) {
            perror("recvfrom problem");
            exit(1);
        }

        printf("REPLY FROM SERVER= %s\n", recvline);
    }

    close(sockfd);
}
