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
void full_array(double* arr, double *arr1, double *arr2, int n1, int n2, int p, int k, Results* res) {
    int l = n1 % p;
    int lenk = n1 / p + (k < l ? 1 : 0);
    int first = (k < l ? k * lenk : k * lenk + l);
    int i1 = 0;
    int h = (first + lenk < n1 ? first + lenk : n1);
    for(i1 = 0; i1 < n2; i1++) {
        arr1[i1] = arr[first * n2 + i1];
    }
    res[k].arr1 = arr1;
    for(i1 = 0; i1 < n2; i1++) {
        arr2[i1] = arr[(h-1) * n2 + i1];
    }
    res[k].arr2 = arr2;
}
void check_arr(double *arr, double *sum, int *count, int n1, int n2, int p, int k) {
    int l = n1 % p;
    int lenk = n1 / p + (k < l ? 1 : 0);
    int first = (k < l ? k * lenk : k * lenk + l);
    int i = 0;
    int j = 0;
    int h = (first + lenk < n1 ? first + lenk : n1);
    double a = 0;
    double sum1 = 0.;
    int count1 = 0;

    for(i = first + 1; i < h - 1; i++) {
        for(j = 1; j < n2 - 1; j++) {
            a = (arr[(i + 1) * n2 + j] + arr[i * n2 + j - 1] + arr[i * n2 + j + 1]  + arr[(i - 1) * n2 + j]) / 4.;
            if ((arr[i * n2 + j]) > a || (fabs(arr[i * n2 + j] - a) < EPS)) {
                sum1 += arr[i * n2 + j];
                count1++;
            }
        }
    }

    *sum = sum1;
    *count = count1;
}

void change_arr(double *arr, double *arr3, double mean, int n1, int n2, int p, int k, Results *res) {
    int l = n1 % p;
    int lenk = n1 / p + (k < l ? 1 : 0);
    int first = (k < l ? k * lenk : k * lenk + l);
    int i = 0, i1 = 0;
    int j = 0;
    int h = (first + lenk < n1 ? first + lenk : n1);
    double curr = 0;
    double buf = 0;
    //printf("thr %d     %d\n", k, first);
    //printf("%d\n", h);
    
    if(k > 0) {
        if(lenk > 1) {
            buf = arr[first * n2];
            for(j = 1; j < n2 - 1; j++) {
                curr = (res[k - 1].arr2[j] + arr[first * n2 + j + 1] + buf + arr[(first + 1) * n2 + j])/ 4.;
                buf = arr[first * n2 + j];
                if((arr[first * n2 + j] > curr) || (fabs(arr[first * n2 + j] - curr) < EPS)) {
                    arr[first * n2 + j] = mean;
                }
            }
        }
        else {
            if(k != p - 1){
                buf = arr[first * n2];
                for(j = 1; j < n2 - 1; j++) {
                    curr = (res[k - 1].arr2[j] + arr[first * n2 + j + 1] + buf + res[k + 1].arr1[j])/4.;
                    buf = arr[first * n2 + j];
                    if((arr[first * n2 + j] > curr) || (fabs(arr[first * n2 + j] - curr) < EPS)) {
                        arr[first * n2 + j] = mean;
                    }
                }
            }
        }
    }
    if(k < p - 1) {
        if(lenk > 2) {
            buf = arr[(h - 1) * n2];
            for(j = 1; j < n2 - 1; j++) {
                curr = (res[k + 1].arr1[j] + arr[(h - 1) * n2 + j + 1] + buf + arr[(h - 2) * n2 + j])/ 4.;
                buf = arr[(h - 1) * n2 + j];
                if((arr[(h - 1) * n2 + j] > curr) || (fabs(arr[(h - 1) * n2 + j] - curr) < EPS)) {
                    arr[(h - 1) * n2 + j] = mean;
                }
            }
        }
        if(lenk == 2) {
            buf = arr[(h - 1) * n2];
            for(j = 1; j < n2 - 1; j++) {
                curr = (res[k + 1].arr1[j] + arr[(h - 1) * n2 + j + 1] + buf + res[k].arr1[j])/ 4.;
                buf = arr[(h - 1) * n2 + j];
                if((arr[(h - 1) * n2 + j] > curr) || (fabs(arr[(h - 1) * n2 + j] - curr) < EPS)) {
                    arr[(h - 1) * n2 + j] = mean;
                }
            }
        }
    }
    for(i1 = 0; i1 < n2; i1++) {
        arr3[i1] = res[k].arr1[i1];
    }

    /*if (first + 1 < n1 - 1) {
        for(i1 = 0; i1 < n2; i1++) {
            arr2[i1] = arr[(first + 1) * n2 + i1];
        }
    }
    if(h - 1 < n1 - 1) {
        for(i1 = 0; i1 < n2; i1++) {
            arr2[i1] = arr[(h-1) * n2 + i1];
        }
    }
    res[k].arr2 = arr2;*/
//вохможно надо завести еще один массив или можно проверять ваще без него
    for(i = first + 1; i < h - 2; i++) {
        buf = arr[i * n2];
        for(j = 1; j < n2 - 1; j++) {
            //print_arr(arr1, 1, n2);
            curr = (arr[(i + 1) * n2 + j] + arr3[j] + arr[i * n2 + j + 1]  + buf) / 4.;
            arr3[j] = arr[i * n2 + j];
            buf = arr[i * n2 + j];
            if ((arr[i * n2 + j] > curr) || (fabs(arr[i * n2 + j] - curr) < EPS)) {
                arr[i * n2 + j] = mean;
            }
        }
        arr3[0] = arr[i * n2];
        arr3[n2 - 1] = arr[i * n2 + n2 - 1];
    }
    if(h - 2 > first) {
        buf = arr[(h - 2) * n2];
        for(j = 1; j < n2 - 1; j++) {
            curr = (arr[(h - 2) * n2 + j + 1] + buf + arr3[j] + res[k].arr2[j]) /4.;
            buf = arr[(h - 2) * n2 + j];
            if ((arr[(h - 2) * n2 + j] > curr)|| (fabs(arr[(h - 2) * n2 + j] - curr) < EPS)) {
                arr[(h - 2) * n2 + j] = mean;
            }
        }
    }
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
    double  mean = 0;
    double *arr1 = nullptr, *arr2 = nullptr, *arr3 = nullptr;
    //double *arr10 = nullptr, *arr20 = nullptr;
    int l = n1 % p;
    double val;
    int lenk1 = 0;
    int first1 = 0;
    int h = 0;
    double curr = 0;
    //int lenk = n1 / p + (k < l ? 1 : 0);
    //int first = (k < l ? k * lenk : k * lenk + l);
    //int h = (first + lenk < n1 ? first + lenk : n1);
    //int leni = 0, leni1 = 0, firsti = 0, lasti = 0;
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
    arr3 = new double[n2];
    if (!arr3) {
        res[k].err = -1;
        delete []arr1;
        delete []arr2;
        return nullptr;
    }

    for(i = 0; i < n2; i++) {
        arr1[i] = 0;
        arr2[i] = 0;
        arr3[i] = 0;
    }
    /*printf("--------------------------\n");
    print_arr(arr1, 1, n2);
    printf("--------------------------\n");
    print_arr(arr2, 1, n2);
    printf("--------------------------\n");*/
    time = clock();
    full_array(arr, arr1, arr2, n1, n2, p, k, res);
    reduce_sum(p);
    for(int k1 = 1; k1 < p; k1++) {
        lenk1 = n1 / p + (k1 < l ? 1 : 0);
        first1 = (k1 < l ? k1 * lenk1 : k1 * lenk1 + l);
        if(lenk1 > 1) {
            for(j = 1; j < n2 - 1; j++) {
                curr = (res[k1 - 1].arr2[j] + arr[first1 * n2 + j + 1] + arr[first1 * n2 + j - 1] + arr[(first1 + 1) * n2 + j])/ 4.;
                if((arr[first1 * n2 + j] > curr) || (fabs(arr[first1 * n2 + j] - curr) < EPS)) {
                    count++;
                    mean += arr[first1 * n2 + j];
                }
            }
        }
        else {
            if(k1 != p - 1){
                for(j = 1; j < n2 - 1; j++) {
                    curr = (res[k1 - 1].arr2[j] + arr[first1 * n2 + j + 1] + arr[first1 * n2 + j - 1] + res[k1 + 1].arr1[j])/4.;
                    if((arr[first1 * n2 + j] > curr) || (fabs(arr[first1 * n2 + j] - curr) < EPS)) {
                        count++;
                        mean += arr[first1 * n2 + j];
                    }
                }
            }
        }
    }
    for(int k1 = 0; k1 < p - 1; k1++) {
        lenk1 = n1 / p + (k1 < l ? 1 : 0);
        first1 = (k1 < l ? k1 * lenk1 : k1 * lenk1 + l);
        h = (first1 + lenk1 < n1 ? first1 + lenk1 - 1: n1 - 1);
        if(lenk1 > 1) {
            for(j = 1; j < n2 - 1; j++){
                curr = (res[k1 + 1].arr1[j] + arr[h * n2 + j + 1] + arr[h * n2 + j - 1] + arr[(h - 1) * n2 + j] )/4.;
                if((arr[h * n2 + j] > curr) || (fabs(arr[h * n2 + j] - curr) < EPS)) {
                    count++;
                    mean += arr[h * n2 + j];
                }
            }
        }
    }
    check_arr(arr, &res[k].sum, &res[k].count, n1, n2, p, k);
    
    /*for(i = 1; i < p; i++) {
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
    }*/

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
    change_arr(arr, arr3, val, n1, n2, p, k, res);
    /*for(i = 1; i < p; i++) {
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
    }*/
    time = (clock() - time) / CLOCKS_PER_SEC;
    reduce_sum(p);
    a -> time = time;
    delete []arr1;
    delete []arr2;
    delete []arr3;   
    return nullptr;
}