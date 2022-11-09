#ifndef MY_THREAD
#define MY_THREAD

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
int f1(const char* name, int* count, double* sum);
int f2(const char* name, double min, int* count);
void *thread_func(void *ptr);

void reduce_sum(int p, double* a = nullptr, int n = 0);

class Global_Results{
    public:
    int status = -100;
    int count = 0;
    int answer = 0;
};
class Results {
    public:
        int status = -100; //not opened
        int count = 0; //when status >= 0
        double sum = 0;
};

class Args {
    public:
        Results *res = nullptr; //length = p
        Global_Results *Res = nullptr;
        int k = 0; //k thread
        int p = 0; //number of threads
        char *name = nullptr; //filename
        pthread_t tid = -1;
};

#endif //MY_THREAD
