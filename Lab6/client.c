#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <errno.h>
#include <getopt.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "shared_lib.h"


typedef unsigned long long ull;

struct Server {
    char ip[255];
    int port;
};


bool ConvertStringToUI64(const char *str, ull *val) {
    char *end = NULL;
    unsigned long long i = strtoull(str, &end, 10);
    if (errno == ERANGE) {
        fprintf(stderr, "Out of ull range: %s\n", str);
        return false;
    }

    if (errno != 0)
        return false;

    *val = i;
    return true;
}

struct Server* ReadServers(char* servers, int* servers_num){
   // TODO: for one server here, rewrite with servers from file
    *servers_num = 0;

    FILE* file;
    file = fopen(servers, "r");
    fscanf(file, "%d\n", servers_num);
    struct Server* to = malloc(sizeof(struct Server) * *servers_num);

    printf("\n Servers:\n");
    for(int i = 0; i < *servers_num; i++)
    {
        fscanf(file, "%s %d\n", to[i].ip, &to[i].port);
        printf("\t%s:%d\n", to[i].ip, to[i].port);
    }
    fclose(file);

    return to;
}

int main(int argc, char **argv) {
    ull k = -1;
    ull mod = -1;
    char servers[255] = {'\0'}; // TODO: explain why 255

    while (true) {
        int current_optind = optind ? optind : 1;

        const char* short_options = "k:m:s:";

        static struct option options[] = {{"k", required_argument, 0, 'k'},
                                          {"mod", required_argument, 0, 'm'},
                                          {"servers", required_argument, 0, 's'},
                                          {0, 0, 0, 0}};

        int option_index = 0;
        int c = getopt_long(argc, argv, short_options, options, &option_index);

        if (c == -1)
            break;

        switch (c) {
            case 0: {
                switch (option_index) {
                    case 0:
                        ConvertStringToUI64(optarg, &k);
                        // TODO: your code here
                        break;
                    case 1:
                        ConvertStringToUI64(optarg, &mod);
                        // TODO: your code here
                        break;
                    case 2:
                        // TODO: your code here
                        memcpy(servers, optarg, strlen(optarg));
                        break;
                    default:
                        printf("Index %d is out of options\n", option_index);
                    }
            } break;
            case 'k':
                ConvertStringToUI64(optarg, &k);
                break;                 
                    
            case 'm':
                ConvertStringToUI64(optarg, &mod);
                break;    
            case 's':
                memcpy(servers, optarg, strlen(optarg));
                break; 
            case '?':
                printf("Arguments error\n");
                break;
            default:
                fprintf(stderr, "getopt returned character code 0%o?\n", c);
        }
    }

    if (k == -1 || mod == -1 || !strlen(servers)) {
        fprintf(stderr, "Using: %s --k 1000 --mod 5 --servers /path/to/file\n",
                argv[0]);
        return 1;
    }

    int servers_num;
    // TODO: delete this and parallel work between servers
    struct Server* to = ReadServers(servers, &servers_num);


    const int block = k / servers_num;
    int* sck = malloc(sizeof(int) * servers_num);
    ull res = 1;

    // TODO: work continiously, rewrite to make parallel
    for (int i = 0; i < servers_num; i++) {

        struct hostent *hostname = gethostbyname(to[i].ip);
        if (hostname == NULL) {
            fprintf(stderr, "gethostbyname failed with %s\n", to[i].ip);
            exit(1);
        }

        struct sockaddr_in server;
        server.sin_family = AF_INET;
        server.sin_port = htons(to[i].port);
        server.sin_addr.s_addr = *((unsigned long *)hostname->h_addr);

        sck[i] = socket(AF_INET, SOCK_STREAM, 0);
        if (sck[i] < 0) {
            fprintf(stderr, "Socket creation failed!\n");
            exit(1);
        }

        if (connect(sck[i], (struct sockaddr *)&server, sizeof(server)) < 0) {
            fprintf(stderr, "Connection failed\n");
            exit(1);
        }

        // TODO: for one server
        // parallel between servers
        ull begin = block*i + 1;
        ull end;
        
        if (i+1 == servers_num)
        {
            end = block * (i+1) + k % servers_num;
        }
        else
        {
            end = block * (i+1);
        } 


        char task[sizeof(ull) * 3];
        memcpy(task, &begin, sizeof(ull));
        memcpy(task + sizeof(ull), &end, sizeof(ull));
        memcpy(task + 2 * sizeof(ull), &mod, sizeof(ull));

        if (send(sck[i], task, sizeof(task), 0) < 0) {
            fprintf(stderr, "Send failed\n");
            exit(1);
        }

    }

    for (int i = 0; i < servers_num; i++) {

        char response[sizeof(ull)];
        if (recv(sck[i], response, sizeof(response), 0) < 0) {
            fprintf(stderr, "Recieve failed\n");
            exit(1);
        }

        // TODO: from one server
        // unite results
        ull answer = 0;
        memcpy(&answer, response, sizeof(ull));
        res = MultModulo(res, answer, mod);

        close(sck[i]);
    }

    printf("\n Result: %llu\n\n", res);

    free(to);

    return 0;
}
