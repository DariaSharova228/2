#ifndef MY_THREAD
#define MY_THREAD

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
int f1(const char *name,int* len,int* min, double* value_min, double* first, double* last, int* first_lonely, int* last_lonely);
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
        double last = 0;
        double first = 0;
        double value = 0;
        int local_min_length = 0;
        int len = 0;
        int flag1 = 0;
        int first_lon = 0;
        int last_lon = 0;
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
