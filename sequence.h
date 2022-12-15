#ifndef MY_THREAD
#define MY_THREAD

#include <stdio.h>
#include <pthread.h>
#include <cstdlib>
#include <stddef.h>
#include <time.h>
#include <math.h>
class Results {
    public:
        double prev1= 0;
        double prev2 = 0;
        double next1 = 0;
        double next2 = 0;
        int count = 0;
        double time = 0;
        int flagr1 = 0;
        int flagr2 = 0;
        int flagl1 = 0;
        int flagl2 = 0;
        //int flfirst = 0;
        //int fln1 = 0;
        //int fln2 = 0;

        Results() = default;
        ~Results() = default;
};

class Args {
    public:
        Results *res = nullptr;
        int count = 0;
        int k = 0;  //№
        int p = 0;  //amount
        int n = 0;
        double *arr = nullptr;
        double time = 0;
        pthread_t tid = -1;
        
        Args() = default;
        ~Args() = default;

};
void reduce_sum(int p, double* a = nullptr, int n = 0);
void *thread_func(void *ptr);
int count_change(double *arr, int n, int p, int k, Results *res);
void full_array(double *arr, int n, int p, int k, Results *res);
int read_arr(double *a, int n, char *name);
/*int f1(const char* name, int* count, double* sum);
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
};*/
#endif //MY_THREAD