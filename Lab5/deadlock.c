#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define ERROR_CREATE_THREAD -11
#define ERROR_JOIN_THREAD   -12
#define SUCCESS             0

pthread_mutex_t mut_1, mut_2;

int test = 0;

void *proc_0()
{
    printf("\nThis is proc_0 using rs1");
    pthread_mutex_lock(&mut_1);
    
        sleep(1);
        
        printf("\np1 trying to get rs2..."); 
        pthread_mutex_lock(&mut_2);
            test++;
        printf("\nproc1 got rs2!!");    
        pthread_mutex_unlock(&mut_2);   
		
    pthread_mutex_unlock(&mut_1);  
     
return 0;
}

void *proc_1()
{
    printf("\nThis is proc_1 using rs2");
    pthread_mutex_lock(&mut_2);
    
        sleep(1);
        
        printf("\np2 trying to get rs1..."); 
        pthread_mutex_lock(&mut_1);
            test--;
        printf("\nproc2 got rs1!!");    
        pthread_mutex_unlock(&mut_1);  
		
    pthread_mutex_unlock(&mut_2);  
     
return 0;

}

int main(){
    
    pthread_t threads[2];
    int status;
    
    status = pthread_create(&threads[0], NULL, proc_0, NULL);
    
    if (status != SUCCESS) {
        printf("Main error: can't create threads[0], status = %d\n", status);
        return ERROR_CREATE_THREAD;
    }
    
    status = pthread_create(&threads[1], NULL, proc_1, NULL);
    if (status !=SUCCESS) {
        printf("Main error: can't create threads[1], status = %d\n", status);
        return ERROR_CREATE_THREAD;
    }
    status = pthread_join(threads[0], NULL);
    if (status != SUCCESS) {
        printf("Main error: can't join threads[0], status = %d\n", status);
        return ERROR_JOIN_THREAD;
    }
    status = pthread_join(threads[1], NULL);
    if ( status != SUCCESS) {
        printf("Main error: can't join threads[1], status = %d\n", status);
        return ERROR_JOIN_THREAD;
    }
    
    return 0;
}