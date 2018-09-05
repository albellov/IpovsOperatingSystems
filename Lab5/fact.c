#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>

#include <getopt.h>
#include <pthread.h>

#define ERROR_CREATE_THREAD -11
#define ERROR_JOIN_THREAD   -12
#define SUCCESS             0


pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;

typedef unsigned long long ull;

struct args_calc
{
    ull     start;
    ull     end;
    ull     mod;
    ull*    res;
    
};

ull sync_fact(ull k, ull mod){
    
    for(ull i = k-1; i > 1; i--){
        k = (k*i%mod)%mod;
    }
    return k%mod;    
}

void* thread_factorial(void* args_f)
{
    struct args_calc* af = (struct args_calc*)args_f;
    ull start = af -> start;
    ull end =   af -> end;
    ull mod =   af -> mod;

    ull work = start;

    for(ull i = start + 1; i <= end; i ++)
    {
        work = (work * (i%mod))%mod;
    }
    pthread_mutex_lock(&mut);
    *(af -> res) = work;
    pthread_mutex_unlock(&mut);
};

void usage(char* argv){

    printf("Usage: %s -k \"num\" --mod \"num\" --pnum \"num\" \n", argv);
}


int main(int argc, char **argv)
{
    ull k = -1;
    ull mod = -1;
    int pnum = -1;
    
    while (true)
    {        
        // int current_optind = optind ? optind : 1;

        const char* short_options = "k:m:p:";

        static struct option options[] = {
            {"k", required_argument, 0, 'k'},
            {"mod", required_argument, 0, 'm'},
            {"pnum", required_argument, 0, 'p'},
            {0, 0, 0, 0}
        };

        int option_index = 0;
        int c = getopt_long(argc, argv, short_options, options, &option_index);

        if (c == -1) break;

            switch (c) {
                case 0:
                    switch (option_index) {
                        case 0:
                        {
                            k = atoi(optarg);

                            if (k <= 0) {
                                printf("k is a positive number\n");
                                return 1;
                            }
                            break;
                        }
                        case 1:
                        {
                            mod = atoll(optarg);

                            if (mod <= 0) {
                                printf("mod is a positive number\n");
                                return 1;
                            }
                            break;
                        }
                        case 2:
                        {
                            pnum = atoi(optarg); 

                            if(pnum <= 0)
                            {
                                printf("pnum is a positive number\n");
                                return 1;
                            }
                            
                            break;
                        }
                         
                        default:
                            printf("Index %d is out of options\n", option_index);
                    }
                    break;
              
                case 'k':
                    k = atoi(optarg);

                    if (k <= 0) {
                        printf("k is a positive number\n");
                        return 1;
                    }
                    break;         
                    
                case 'm':
                    mod = atoll(optarg);

                    if (mod <= 0) {
                        printf("mod is a positive number\n");
                        return 1;
                    }
                    break;
                        
                case 'p':
                            
                    pnum = atoi(optarg);

                    if(pnum <= 0)
                    {
                        printf("pnum is a positive number\n");
                        return 1;
                    }
                            
                    break;
                                  
                  
                case '?':
                    break;

                default:              
                    printf("getopt returned character code 0%o?\n", c);
            }
    }

    if (optind < argc)
    {
        printf("Has at least one no option argument\n");
        return 1;
    }

    if (k == -1 || mod == -1 || pnum == -1)
    {
        usage(argv[0]);
        return 1;
    }

    if (pnum > k/2){
        pnum /= 2;
    }
    
    pthread_t* threads = malloc(pnum*sizeof(pthread_t));    
    struct args_calc* ac = malloc(pnum * sizeof(struct args_calc));
        
    int status;
    ull final_res = 1;
    ull block = k / pnum;

    struct timeval start_time;
    gettimeofday(&start_time, NULL);
        
    for(int i = 0; i < pnum; i++)
    {
        ac[i].start = block*i + 1;
        ac[i].mod = mod;
        ac[i].res = &final_res;    

        if(i+1 == pnum)
        {
            ac[i].end = block * (i+1) + k % pnum;
        }
        else
        {
            ac[i].end = block * (i+1);
        }    
       
        status = pthread_create( &threads[i], NULL, thread_factorial, &ac[i]);

        if (status != SUCCESS ) {
            printf("Main error: can't create thread, status = %d\n", status);
            return ERROR_CREATE_THREAD;
        }
    }
    
    for(int i = 0; i < pnum; i++)
    {
       status = pthread_join( threads[i], NULL);

       if (status != SUCCESS) {
            printf("Main error: can't join thread, status = %d\n", status);
            return ERROR_JOIN_THREAD;
        }
    }
    
    
    struct timeval finish_time;
    gettimeofday(&finish_time, NULL);
    double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
    elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;
    

    printf("\nAsync Factorial %lld mod %lld = %llu\n", k, mod, final_res);
    printf("Sync Factorial %lld mod %lld = %llu\n", k, mod, sync_fact(k,mod));
    
    free(threads);
    free(ac);

    printf("\nElapsed time: %fms\n\n", elapsed_time);

    return 0;
}