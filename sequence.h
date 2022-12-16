#ifndef MY_THREAD
#define MY_THREAD

#include <stdio.h>
#include <pthread.h>
#include <cstdlib>
#include <stddef.h>
#include <time.h>
#include <math.h>
/*class Results {
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

};*/
class Results {
    public:
        double sum = 0;
        int count = 0;
        int err = 0;
        double *arr1 = nullptr;
        double *arr2 = nullptr;

        Results() = default;
        ~Results() = default;
};

class Args {
    public:
        Results *res = nullptr;
        int k = 0;  //№
        int p = 0;  //amount
        int n1 = 0;
        int n2 = 0;
        int count = 0;
        double *arr = nullptr;
        double time = 0;
        pthread_t tid = -1;
        
        Args() = default;
        ~Args() = default;

};
void reduce_sum(int p, double* a = nullptr, int n = 0);
void *thread_func(void *ptr);
void check_arr(double *arr, double *sum, int *count, int n1, int n2, int p, int k);
void change_arr(double *arr, double *arr1, double *arr2, double mean, int n1, int n2, int p, int k, Results *res);
void print_arr(double *a, int n1, int n2);
int read_arr(double *a, int n, char *name);


#endif //MY_THREAD