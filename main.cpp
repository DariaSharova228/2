#include <stdio.h>
#include "sequence.h"

int main(int argc, char *argv[]) {
    Args *a = nullptr;
    Results *r = nullptr;
    Global_Results R;
    int p = 0, k = 0;
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
}
