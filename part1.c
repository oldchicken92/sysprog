#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include "slow_functions.h"


int main (){

    pthread_t function2;
    pthread_t function1[10]; 

    pthread_create(&function2, NULL, slow_function2, NULL);

    for (int count = 0; count < 10; count++){
        pthread_create(&function1[count], NULL, slow_function1, NULL);
    }

     for (int count = 0; count < 10; count++){
        pthread_join(function1[count], NULL);
    }


    pthread_join(function2, NULL);



    return 0;
}