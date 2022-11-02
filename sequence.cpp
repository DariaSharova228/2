#include "sequence.h"
#define EPS 1e-16
double fabs(double a) {
    return (a > 0 ? a : -a);
}
int f(const char *name, int *count) {
    FILE *in;
    double x, y;
    int length = 0, sum = 0;
    if(!(in = fopen(name, "r"))) {
        return -1;
    }
    if (fscanf(in,"%lf",&x)!=1 && (!feof(in))){
        fclose(in);
        return -2;
    }
    y = x;
    length++;
    while (fscanf(in,"%lf",&x)==1)
    {
        if (fabs(x-y)<EPS) length++;
        else 
        { 
            if (length!=1) sum++;
            length = 1; 
        }
        y = x;
    }
       
    if (length>1) sum++;
    if (!feof(in)) 
    {
        fclose(in);
        return -2;
    }
    fclose(in);
    *count += sum;
    return 0;
}
void *thread_func(void *ptr) {
    Args *a = (Args*) ptr;
    int k = a->k;
    char *name = a->name;
    Results *r = a->res;
    r[k].status = f(name, &r[k].count);
    return nullptr;
}
