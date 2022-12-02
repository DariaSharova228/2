
#include <pthread.h>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include "sequence.h"

/*int main(int argc, char *argv[]) {
    Args *a = nullptr;
    Results *r = nullptr;
    Global_Results R;
    int p = 0, k = 0;
    int n = 0;
    FILE *fp;
    if(argc != 4) {
        printf("Usage: %s p n names\n", argv[0]);
        return -1;
    }
    if(!sscanf(argv[1], "%d", &p) || !sscanf(argv[2], "%d", &n)) {
        
    }


    if(argc <= 1) {
        printf("Usage: %s names\n", argv[0]);
        return -1;
    }
    p = argc -1;
    r = new Results[p];
    if(r == nullptr) {
        printf("could not allocate memory for results\n");
        return -1;
    }
    a = new Args[p];
    if(a == nullptr) {
        printf("could not allocate memory for args\n");
        delete []r;
        return -1;
    }
    
    for(k = 0; k < p; k++) {
        a[k].res = r;
        a[k].k = k;
        a[k].p = p;
        a[k].Res = &R;
        a[k].name = argv[k + 1];
    }

    for(k = 1; k < p; k++) {
        if(pthread_create(&a[k].tid, 0, thread_func, a + k)) {
            printf("Could not create pthread %d\n", k);
            delete []r;
            delete []a;
            abort();
        }
    }
    thread_func(a + 0);
    for(k = 1; k < p; k++) {
        pthread_join(a[k].tid, 0);
    }
    //no other threads except main
    //reuslt
    int err = 0;
    for(k = 0; k < p; k++) {
        if(r[k].status < 0) {
            switch(r[k].status) {
                case -1: 
                    printf("Could not open file %s\n", a[k].name);
                    break;
                case -2:
                    printf("Could not read file %s\n", a[k].name);
                    break;
                default:
                    printf("Unknown error %d %s\n", r[k].status, a[k].name);
                    break;
            }
            err++;
        }
    }
    if(err) {
        delete []r;
        delete []a; 
        return -1;
    }
    printf("Result = %d\n", R.answer);
    delete []r, delete[]a;
    return 0;
}*/
int main(int argc, char *argv[]) {
    Args *a = nullptr;
    Results *res = nullptr;
    int p = 0, k = 0, err = 0, n = 0, count = 0;
    double *arr = nullptr;
    char *name = nullptr;
    double time = 0;

    if (!((argc == 4) && (sscanf(argv[1], "%d", &p) == 1) && (sscanf(argv[2], "%d", &n)) && (p >= 1) && (p <= n))) {
        printf("Usage: %s p n name\n", argv[0]);
        return 0;
    }

    name = argv[3];

    arr = new double[n];
    if (!arr) {
        printf("Memory error!\n");
        return 0;
    }

    res = new Results[p];
    a = new Args[p];

    if (res == nullptr || a == nullptr) {
        printf("Not any new!\n");
        if (res) delete []res;
        if (a) delete []a;
        delete []arr;
        return 0;
    }

    err = read_arr(arr, n, name);
    if (err == -1) {
        printf("Cannot open file %s!\n", name);
        return 0;
    }

    if (err == -2) {
        printf("Cannot read file %s!\n", name);
        return 0;
    }

    for(k = 0; k < p; k++) {
        a[k].arr = arr;
        a[k].k = k;
        a[k].p = p;
        a[k].n = n;
        a[k].res = res;
    }

    time = clock();
    for(k = 1; k < p; k++) {
        if (pthread_create(&a[k].tid, 0, thread_func, a + k)) {
            printf("Cannot create thread %d!\n", k);
            delete []arr;
            delete []res;
            delete []a;
            abort();
        }
    }

    thread_func(a + 0);
    for(k = 1; k < p; k++) 
        pthread_join(a[k].tid, 0);
    time = (clock() - time) / CLOCKS_PER_SEC;
    
    for(k = 0; k < p; k++) 
        count += a[k].count;

    for(k = 0; k < p; k++)
        printf("%d elapsed: %2lf\n", k, a[k].time);
    printf("Global time = %2lf\n", time);
    printf("Count = %d\n", count);
    printf("RESULT %d: ", p);
        for(int i = 0; i < n; i++){
        printf("%.4f ", arr[i]);
    }
    delete []res;
    delete []a;
    delete []arr;
    return 1;
}
