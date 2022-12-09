#include "sequence.h"
#define EPS 1e-16
double fabs(double a) {
    return (a > 0 ? a : -a);
}
int read_arr(double *a, int n, char *name) {
    FILE *inp;
    int i = 0;

    if (!(inp = fopen(name, "r"))) {
        return -1;
    }

    for(i = 0; i < n; i++) {
        a[i] = 0;
        if (fscanf(inp, "%lf", &a[i]) != 1) {
            fclose(inp);
            return -2;
        }
    }

    fclose(inp);
    return 0;
}
void print_arr(double *a, int n1, int n2) {
    int i = 0, j = 0;
    for(i = 0; i < n1; i++) {
        for(j = 0; j < n2; j++)
            printf("%lf ", a[i * n2 + j]);
        printf("\n");
    }
}
void reduce_sum(int p, double* a, int n){
  static pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
  static pthread_cond_t c_in = PTHREAD_COND_INITIALIZER;
  static pthread_cond_t c_out = PTHREAD_COND_INITIALIZER;
  static int t_in = 0;
  static int t_out = 0;
  static double*r = nullptr;
  int i;
  if(p <= 1)return;
  pthread_mutex_lock(&m);
  if(r == nullptr){
    r = a;
  }
  else{
    for(i = 0; i < n; i++) r[i]+=a[i];
  }
  t_in++;
  if(t_in >= p){
    t_out = 0;
    pthread_cond_broadcast(&c_in);
  }else{
    while(t_in<p){
      pthread_cond_wait(&c_in, &m);
    }
  }


  if(r != a){
    for(i = 0; i < n; i++) a[i] = r[i];
  }
  t_out++;
  if(t_out >=p){
    t_in = 0;
    r = nullptr;
    pthread_cond_broadcast(&c_out);
  }else{
    while(t_out < p){
      pthread_cond_wait(&c_out, &m);
    }
  }
  pthread_mutex_unlock(&m);
}
void check_arr(double *arr, double *sum, int *count, int n1, int n2, int p, int k) {
    int l = n1 % p;
    int lenk = n1 / p + (k < l ? 1 : 0);
    int first = (k < l ? k * lenk : k * lenk + l);
    int i = 0;
    int j = 0;
    int h = (first + lenk < n1 ? first + lenk : n1);
    double mean0 = 0;

    for(i = first + 1; i < h - 1; i++) {
        for(j = 1; j < n2 - 1; j++) {
            mean0 = (arr[(i + 1) * n2 + j] + arr[i * n2 + j - 1] + arr[i * n2 + j + 1]  + arr[(i - 1) * n2 + j]) / 4.;
            if ((arr[i * n2 + j]) > mean0 || (fabs(arr[i * n2 + j] - mean0) < EPS)) {
                (*sum) += arr[i * n2 + j];
                (*count)++;
            }
        }
    }
}

void change_arr(double *arr, double *arr1, double *arr2, double mean, int n1, int n2, int p, int k, Results *res) {
    int l = n1 % p;
    int lenk = n1 / p + (k < l ? 1 : 0);
    int first = (k < l ? k * lenk : k * lenk + l);
    int i = 0, i1 = 0;
    int j = 0;
    int h = (first + lenk < n1 ? first + lenk : n1);
    double mean0 = 0;
    //printf("thr %d     %d\n", k, first);
    //printf("%d\n", h);
    
    for(i1 = 0; i1 < n2; i1++) {
        arr1[i1] = arr[first * n2 + i1];
    }

    if (first + 1 < n1 - 1) {
        for(i1 = 0; i1 < n2; i1++) {
            arr2[i1] = arr[(first + 1) * n2 + i1];
        }
    }
    res[k].arr2 = arr2;

    for(i = first + 1; i < h - 1; i++) {
        for(j = 1; j < n2 - 1; j++) {
            //print_arr(arr1, 1, n2);
            mean0 = (arr[(i + 1) * n2 + j] + arr1[j] + arr[i * n2 + j + 1]  + arr[i * n2 + j - 1]) / 4.;
            //printf("\n%lf   %lf   %d %d\n\n\n", arr[i * n2 + j], mean0 * 5, i, j);
            arr1[j] = arr[i * n2 + j];
            if ((arr[i * n2 + j]) > mean0 || (fabs(arr[i * n2 + j] - mean0) < EPS)) {
                arr[i * n2 + j] = mean;
            }
        }
        arr1[n2 - 1] = arr[i * n2 + n2 - 1];
    }
    res[k].arr1 = arr1;
    /*printf("--------------------------\n");
    printf("--------------------------\n");
    print_arr(arr1, 1, n2);
    printf("--------------------------\n");
    print_arr(arr2, 1, n2);
    printf("--------------------------\n");
    printf("--------------------------\n");*/
}

void* thread_func(void *ptr) {
    Args *a = (Args*) ptr;
    int k = a -> k;
    int p = a -> p;
    int n1 = a -> n1;
    int n2 = a -> n2;
    double *arr = a -> arr;
    Results *res = a -> res;
    double time = 0;
    int count = 0;
    double mean0 = 0, mean = 0;
    double *arr1 = nullptr, *arr2 = nullptr;
    double *arr10 = nullptr, *arr20 = nullptr;
    int l = n1 % p;
    double val;
    //int lenk = n1 / p + (k < l ? 1 : 0);
    //int first = (k < l ? k * lenk : k * lenk + l);
    int leni = 0, leni1 = 0, firsti = 0, lasti = 0;
    int i = 0, j = 0;

    arr1 = new double[n2];
    if (!arr1) {
        res[k].err = -1;
        return nullptr;
    }

    arr2 = new double[n2];
    if (!arr2) {
        res[k].err = -1;
        delete []arr1;
        return nullptr;
    }

    for(i = 0; i < n2; i++) {
        arr1[i] = 0;
        arr2[i] = 0;
    }
    /*printf("--------------------------\n");
    print_arr(arr1, 1, n2);
    printf("--------------------------\n");
    print_arr(arr2, 1, n2);
    printf("--------------------------\n");*/
    time = clock();
    check_arr(arr, &res[k].sum, &res[k].count, n1, n2, p, k);
    
    for(i = 1; i < p; i++) {
        leni1 = n1 / p + (i - 1 < l ? 1 : 0);
        leni = n1 / p + (i < l ? 1 : 0);
        firsti = (i < l ? i * leni : i * leni + l);
        lasti = (i - 1 < l ? (i - 1) * leni1 : (i - 1) * leni1 + l) + leni1 - 1;
        if (leni1 > 1) {
            if (lasti < n1 - 1 && lasti > 0)
                for(j = 1; j < n2 - 1; j++) {
                    mean0 = (arr[lasti * n2 + j + 1] + arr[(lasti - 1) * n2 + j] + arr[lasti * n2 + j - 1]+ arr[(lasti + 1) * n2 + j]) / 4.;  
                    if ((arr[lasti * n2 + j] > mean0) || (fabs(arr[lasti * n2 + j] - mean0) < EPS)) {
                        count++;
                        mean += arr[lasti * n2 + j];
                    }
                }
        }
        if (firsti < n1 - 1 && firsti > 0) {
            for(j = 1; j < n2 - 1; j++) {
                mean0 = (arr[firsti * n2 + j + 1] + arr[(firsti - 1) * n2 + j] + arr[firsti * n2 + j - 1] + arr[(firsti + 1) * n2 + j] ) / 4.;  
                if ((arr[firsti * n2 + j] > mean0) || (fabs(arr[firsti * n2 + j] - mean0) < EPS)) {
                    count++;
                    mean += arr[firsti * n2 + j];
                }
            }
        }
    }
    reduce_sum(p);
    for(i = 0; i < p; i++) {
        mean += res[i].sum;
        count += res[i].count;
    }
    if(count == 0) {
        if(k == 0) {
            a->res->err = -2;
            a->count = 0;
        }
        reduce_sum(p);
        return 0;
    }
    val = mean/count;
    a -> count = count;

    change_arr(arr, arr1, arr2, val, n1, n2, p, k, res);
    reduce_sum(p);
    for(i = 1; i < p; i++) {
        arr10 = res[i - 1].arr1;
        arr20 = res[i].arr2;
        leni1 = n1 / p + (i - 1 < l ? 1 : 0);
        leni = n1 / p + (i < l ? 1 : 0);
        firsti = (i < l ? i * leni : i * leni + l);
        lasti = (i - 1 < l ? (i - 1) * leni1 : (i - 1) * leni1 + l) + leni1 - 1;
        //printf("\n%d 1st %d     last %d\n", i, firsti, lasti);
        
        if (leni1 > 1) {
            if (lasti < n1 - 1 && lasti > 0)
                for(j = 1; j < n2 - 1; j++) {
                    mean0 = (arr[lasti * n2 + j + 1] + arr10[j]  + arr[(lasti + 1) * n2 + j] + arr[lasti * n2 + j - 1]) / 4.;  
                    if ((arr[lasti * n2 + j] > mean0) || (fabs(arr[lasti * n2 + j] - mean0) < EPS)) {
                        arr[lasti * n2 + j] = val;
                    }
                }
        }
        if (firsti < n1 - 1 && firsti > 0) {
            for(j = 1; j < n2 - 1; j++) {
                mean0 = (arr[firsti * n2 + j + 1] + arr[(firsti - 1) * n2 + j] + arr20[j] + arr[firsti * n2 + j - 1]) / 4.;  
                if ((arr[firsti * n2 + j] > mean0) || (fabs(arr[firsti * n2 + j] - mean0) < EPS)) {
                    arr[firsti * n2 + j] = val;
                }
            }
        }
    }
    time = (clock() - time) / CLOCKS_PER_SEC;
    reduce_sum(p);
    a -> time = time;
    delete []arr1;
    delete []arr2;   
    return nullptr;
}