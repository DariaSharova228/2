#ifndef MY_THREAD
#define MY_THREAD

#include <stdio.h>
#include <csignal>
#include <pthread.h>
#include <cstdlib>
#include <sys/time.h>
#include <sys/resource.h>

#define CHUNK_SIZE 100000

class global_results {
public:
    long long unsigned int max_six = 0;
    long long unsigned int last = 0;
    long long unsigned int prev_last = 0;
    int n = 0;
    int current_count = 0;
    int prev_count = 0;
    int index_last = -1;
};

class results {
public:
    int start = 0;
    int ch_size = CHUNK_SIZE;
    int count_six_prime2 = 0;
    long long unsigned int first_prime = 0;
    long long unsigned int last_prime = 0;
    long long unsigned int prev_last_prime = 0;
    long long unsigned int second_prime = 0;
    long long unsigned int current_max = 0;
    double time_cpu = 0;
    global_results *glob_res;
};

class Args {
public:
    results *res = nullptr;
    int n = 0;
    int k = 0;
    int p = 0;
    pthread_t tid = -1;
};

double get_cpu_time();
double get_time_tot();
int find_6y_numbers(Args *arg);

int global_res_fun(Args *arg);

void *thread_func(void *ptr);


#endif //MY_THREAD