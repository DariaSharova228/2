#ifndef MY_THREAD
#define MY_THREAD

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void *thread_func(void *ptr);

class Results {
    public:
        int status = -100; //not opened
        int count = 0; //when status >= 0
};

class Args {
    public:
        Results *res = nullptr; //length = p
        int k = 0; //k thread
        int p = 0; //number of threads
        char *name = nullptr; //filename
        pthread_t tid = -1;
};

#endif //MY_THREAD
