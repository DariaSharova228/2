#include "sequence.h"
double get_time_tot()
{
    struct timeval buf;
    gettimeofday(&buf, 0);
    return buf.tv_sec + buf.tv_usec / 1.e6;
}
int main(int argc, char *argv[]) {
    int p_thread, k, n;
    Args *args;
    results *local_res;
    global_results glob_res;
    double time_tot = 0;
    if (!((argc == 3) && sscanf(argv[1], "%d", &p_thread) == 1 && sscanf(argv[2], "%d", &n) == 1)) {
        printf("usage: %s p n\n", argv[0]);
        return 1;
    }

    local_res = new results[p_thread];
    args = new Args[p_thread];

    if (local_res == nullptr || args == nullptr) {

        printf("Error! Unable to allocate memory\n");

        if (local_res) {
            delete[] local_res;
        }

        if (args) {
            delete[] args;
        }

        return 3;
    }

    glob_res.n = n;

    for (k = 0; k < p_thread; k++) {
        args[k].k = k;
        args[k].res = local_res;
        args[k].p = p_thread;
        args[k].n = n;
        local_res[k].glob_res = &glob_res;
    }
    time_tot = get_time_tot();
    for (k = 1; k < p_thread; k++) {
        if (pthread_create(&args[k].tid, 0, thread_func, args + k)) {
            printf("Cannot create thread %d\n", k);
            abort();
        }
    }
    thread_func(args + 0);
    for (k = 1; k < p_thread; k++) {
        pthread_join(args[k].tid, 0);
    }
    time_tot = get_time_tot() - time_tot;
    for (k = 0; k < p_thread; k++) {
        printf("%d thread time = %lf sec\n", k, local_res[k].time_cpu);
    }
    printf ("elapsed_all = %.2f\n", time_tot);
    printf("Result = %llu\n", glob_res.max_six);
    delete[] local_res;
    delete[] args;
    return 0;
}