#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <stdbool.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>

#include <pthread.h>
#include <getopt.h>

#include "utils.h"

#define ERROR_CREATE_THREAD -11
#define ERROR_JOIN_THREAD   -12
#define SUCCESS        0


struct SumArgs {
	
	int *array;
	int begin;
	int end;
};


void* ThreadSum(void *args) {
	
	struct SumArgs *sum_args = (struct SumArgs *)args;
	size_t sum = 0;
	
	// TODO: your code here 
	for(int i = sum_args->begin; i < sum_args->end; ++i)
	{
		sum += (sum_args->array)[i];
	}
	
	return (void *)sum;
}


int main(int argc, char **argv) {
	/*
	*  TODO:
	*  threads_num by command line arguments
	*  array_size by command line arguments
	*  seed by command line arguments
	*/

	uint32_t threads_num = 0;
	uint32_t array_size = 0;
	uint32_t seed = 0;
	
	bool is_print = false;
	
	while (true) {
    //int current_optind = optind ? optind : 1;

    const char* short_options = "s:t:a:p";
    
    static struct option options[] = {{"seed", required_argument, 0, 's'},
                                      {"threads_num", required_argument, 0, 't'},
                                      {"array_size", required_argument, 0, 'a'},
									  {"print_array", no_argument, 0, 'p'},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, short_options, options, &option_index);

    if (c == -1) break;
		switch (c) {
		  case 0:
			switch (option_index) {
			  	case 0:
				{
					seed = atoi(optarg);
					// your code here
					// error handling
					if (seed <= 0) {
						printf("seed is a positive number\n");
						return 1;
					}
					break;
				}
				case 1:
				{
					threads_num = atoi(optarg);
					// your code here
					// error handling
					if (threads_num <= 0) {
						printf("Threads num is a positive number\n");
						return 1;
					}
					break;
				}
				case 2:
				{
					array_size = atoi(optarg);
					// your code here
					// error handling
					if(array_size <= 0)
					{
						printf("Array size is a positive number\n");
						return 1;
					}

					break;
				}
				case 3:
				{
					is_print = true;
					break;
				}

				defalut:
					printf("Index %d is out of options\n", option_index);
			}
			break;

			case 's':
				seed = atoi(optarg);
				// your code here
				// error handling
				if (seed <= 0) {
					printf("seed is a positive number\n");
					return 1;
				}
				break;

			case 'a':
				array_size = atoi(optarg);
					// your code here
					// error handling
					if (array_size <= 0) {
						printf("Array size is a positive number\n");
						return 1;
					}
					break;

			case 't':

				threads_num = atoi(optarg);
				// your code here
				// error handling
				if (threads_num <= 0) {
					printf("Threads num is a positive number\n");
					return 1;
				}
				break;

			case 'p':
				is_print = true;

				break;

			case '?':
				break;

			default:

			printf("getopt returned character code 0%o?\n", c);
		}
	}

	if (optind < argc) {
		printf("Has at least one no option argument\n");
		return 1;
	}
	
	if (seed == -1 || array_size == -1 || threads_num == -1 ) {
		printf("Usage: %s --seed \"num\" --threads_num \"num\" --array_size \"num\" \n",
				argv[0]);
		return 1;
	}

	pthread_t *threads = malloc( sizeof(pthread_t) * threads_num );
  
	int* array = malloc(sizeof(int) * array_size);  
	GenerateArray(array, array_size, seed);
  
	if(is_print){
		
		printf("\nArray: ");
		
		for(int i = 0; i < array_size; ++i)
		{
			printf("%d ", array[i]);
		}
		printf("\n");
	}
 
	/*
	* TODO:
	* your code here
	* Generate array here
	*/

	struct SumArgs* args = malloc(sizeof(struct SumArgs) * threads_num);
	
	int block = array_size / threads_num;
	
	for(int i = 0; i < threads_num; ++i)
	{
		args[i].array = array;
		args[i].begin = block*i;
		
		if(i == threads_num - 1)
		{
			args[i].end = block * (i+1) + array_size % threads_num;
		}
		else
		{
			args[i].end = block * (i+1);
		}
	}
  
	struct timeval start_time;
	gettimeofday(&start_time, NULL);
	
	int status;
	
	for (uint32_t i = 0; i < threads_num; ++i) {
		status = pthread_create(&threads[i], NULL, ThreadSum, (void *)&(args[i]));
		if (status != SUCCESS ) {
			printf("Error: pthread_create failed!\n");
			return 1;
		}
	}

	int total_sum = 0;
	
	printf("\n");
	
	for (uint32_t i = 0; i < threads_num; ++i) {
		
		int sum = 0;
		status = pthread_join(threads[i], (void **)&sum);
		
		if (status != SUCCESS) {
            printf("main error: can't join thread, status = %d\n", status);
            exit(ERROR_JOIN_THREAD);
        }
		printf("Sum_%d: %d\n", i+1, sum);
		total_sum += sum;
	}
  
	struct timeval finish_time;
	gettimeofday(&finish_time, NULL);


	double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
	elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;
  
  
	free(array);
	free(args);
	free(threads);
  
  
	printf("\nTotal Sum: %d\n", total_sum);
	printf("\nElapsed time: %fms\n\n", elapsed_time);
  
	return 0;
}
