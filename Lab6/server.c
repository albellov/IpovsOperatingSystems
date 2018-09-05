#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <getopt.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "pthread.h"


typedef unsigned long long ull;


struct FactorialArgs {
    ull begin;
    ull end;
    ull mod;
};


ull MultModulo(ull a, ull b, ull mod) {
    ull result = 0;
    a = a % mod;
    while (b > 0) {
        if (b % 2 == 1)
            result = (result + a) % mod;
        a = (a * 2) % mod;
        b /= 2;
    }

    return result % mod;
}


ull Factorial(const struct FactorialArgs *args) {
    ull ans = args->begin;
    for (ull i = args->begin + 1; i <= args->end; i++)
        ans = MultModulo(ans, i, args->mod);

    return ans;
}


void *ThreadFactorial(void *args) {
    struct FactorialArgs *fargs = (struct FactorialArgs *)args;
    return (void *)(ull *)Factorial(fargs);
}


int main(int argc, char **argv) {

    int tnum = -1;
    int port = -1;

    while (true) {
        int current_optind = optind ? optind : 1;

        const char* short_options = "p:t:";

        static struct option options[] = {{"port", required_argument, 0, 'p'},
                                          {"tnum", required_argument, 0, 't'},
                                          {0, 0, 0, 0}};

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
                    tnum = atoi(optarg);
                    if (tnum <= 0) {
                        printf("tnum is a positive number\n");
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
                    
        case 't':
            tnum = atoll(optarg);

            if (tnum <= 0) {
                printf("tnum is a positive number\n");
                return 1;
            }
            break;    

        case '?':
            printf("Unknown argument\n");
            break;
        default:
            fprintf(stderr, "getopt returned character code 0%o?\n", c);
        }
    }

  if (port == -1 || tnum == -1) {
    printf("%s %s %s %s %s\n",argv[0], argv[1], argv[2],argv[3],argv[4] );
    fprintf(stderr, "Using: %s --port 20001 --tnum 4\n", argv[0]);
    return 1;
  }

  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0) {
    fprintf(stderr, "Can not create server socket!");
    return 1;
  }

  struct sockaddr_in server;
  server.sin_family = AF_INET;
  server.sin_port = htons((uint16_t)port);
  server.sin_addr.s_addr = htonl(INADDR_ANY);

  int opt_val = 1;
  setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt_val, sizeof(opt_val));

  int err = bind(server_fd, (struct sockaddr *)&server, sizeof(server));
  if (err < 0) {
    fprintf(stderr, "Can not bind to socket!");
    return 1;
  }

  err = listen(server_fd, 128);
  if (err < 0) {
    fprintf(stderr, "Could not listen on socket\n");
    return 1;
  }

  printf("\nServer listening at %d\n", port);

  while (true) {

    struct sockaddr_in client;
    socklen_t client_len = sizeof(client);
    int client_fd = accept(server_fd, (struct sockaddr *)&client, &client_len);

    if (client_fd < 0) {
      fprintf(stderr, "Could not establish new connection\n");
      continue;
    }

    while (true) {

      unsigned int buffer_size = sizeof(ull) * 3;
      char from_client[buffer_size];
      int read = recv(client_fd, from_client, buffer_size, 0);

      if (!read)
        break;
      if (read < 0) {
        fprintf(stderr, "Client read failed\n");
        break;
      }
      if (read < buffer_size) {
        fprintf(stderr, "Client send wrong data format\n");
        break;
      }

      pthread_t threads[tnum];

      ull begin = 0;
      ull end = 0;
      ull mod = 0;
      memcpy(&begin, from_client, sizeof(ull));
      memcpy(&end, from_client + sizeof(ull), sizeof(ull));
      memcpy(&mod, from_client + 2 * sizeof(ull), sizeof(ull));

      fprintf(stdout, "Receive: %llu %llu %llu\n", begin, end, mod);

      struct FactorialArgs args[tnum];

      int section = end - begin + 1;

      if( tnum > section){
          tnum = section;
      }

      ull block = section / tnum;

      for (uint32_t i = 0; i < tnum; i++) {
        // TODO: parallel somehow
        args[i].begin = begin + block*i;
        args[i].mod = mod;

        if(i+1 == tnum){
            args[i].end = begin - 1 + block * (i+1) + section % tnum;
        }
        else{
            args[i].end = begin - 1 + block * (i+1);
        } 

        printf("%llu %llu\n",args[i].begin, args[i].end);

        if (pthread_create(&threads[i], NULL, ThreadFactorial,
                           (void *)&args[i])) {
          printf("Error: pthread_create failed!\n");
          return 1;
        }
      }

      ull total = 1;
      for (uint32_t i = 0; i < tnum; i++) {
          ull result = 0;
          pthread_join(threads[i], (void **)&result);
          total = MultModulo(total, result, mod);
      }

      printf("Total: %llu\n", total);

      char buffer[sizeof(total)];
      memcpy(buffer, &total, sizeof(total));
      err = send(client_fd, buffer, sizeof(total), 0);
      if (err < 0) {
          fprintf(stderr, "Can't send data to client\n");
          break;
      }
    }

    shutdown(client_fd, SHUT_RDWR);
    close(client_fd);
  }

  return 0;
}
