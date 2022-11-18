#include "sequence.h"
#define EPS 1e-16
double fabs(double a) {
    return (a > 0 ? a : -a);
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
int f1(const char* name, int* count, double* sum1) {// подсчет локальных минимумов
    FILE *in;
    double curr, next, prev;
    int quantity = 0;
    double s = 0;
    if(!(in = fopen(name, "r"))) {
        return -1;
    }
    if(fscanf(in,"%lf",&prev)!=1 && (!feof(in))){
        fclose(in);
        return -2;
    }
    if(fscanf(in,"%lf",&curr)!=1 && (!feof(in))){
        fclose(in);
        return -2;
    }
    while(fscanf(in,"%lf",&next) == 1) {
        if(curr < prev && curr < next) {
            s += curr;
            quantity++;
        }
        prev = curr;
        curr = next;
    }
    if (!feof(in)) 
    {
        fclose(in);
        return -2;
    }
    fclose(in);
    *count += quantity;
    *sum1 += s;
    return 0;

}
int f2(const char* name, double min, int* count){// подсчет числа элементов
    FILE *in;
    double curr;
    int sum1 = 0;
    if(!(in = fopen(name, "r"))) {
        return -1;
    }
    while (fscanf(in, "%lf", &curr) == 1)
    {
      if (curr < min)
        {
          sum1++;
        }
    }
    if (!feof(in))
    {
      fclose(in);
      return -2;
    }
    fclose(in);
    *count = sum1;
    return 0;
}
void* thread_func(void *ptr) {
    Args *a = (Args*) ptr;
    int k = a -> k;
    int p = a -> p;
    char *name = a -> name;
    Results *r = a -> res;
    int count = 0;
    int err = 0;
    double sum = 0;
    double mean;
    int i;
    double w;
    r[k].status = f1(name, &r[k].count, &r[k].sum);
    reduce_sum(p);
    err = 0;
    for(i = 0; i < p; i++) {
        if (r[i].status != 0) {
            err++;
            if (k == 0) {
                switch (r[i].status) {
                    case -1:
                        printf("Cannot open file %s!\n", a[k].name);
                        break;
                    case -2:
                        printf("Cannot read file %s!\n", a[k].name);
                        break;
                    default:
                        printf("Unknown error! %s\n", a[k].name);
                }
            }
        }
    }

    if (err) return 0;

    for(i = 0; i < p; i++) 
        sum += r[i].sum;

    for(i = 0; i < p; i++) 
        count += r[i].count;
    
    if(count == 0) {
        if(k == 0) {
            a->Res->status = 0;
            a->Res->count = 0;
            a->Res->answer = 0;
            reduce_sum(p);
            return 0;
        }
    }
    mean = sum/count;
    r[k].status = f2(name,mean,&r[k].count);
    w = r[k].count;
    reduce_sum(p, &w, 1);
    err = 0;
    for(i = 0; i < p; i++) {
        if (r[i].status != 0) {
            err++;
            if (k == 0) {
                switch (r[i].status) {
                    case -1:
                        printf("Cannot open file %s!\n", a[k].name);
                        break;
                    case -2:
                        printf("Cannot read file %s!\n", a[k].name);
                        break;
                    default:
                        printf("Unknown error! %s\n", a[k].name);
                }
            }
        }
    }
    if(k == 0){
        a->Res->status = err;
    }
    if(err) return 0;
    //w = r[k].count;
    //reduce_sum(p, &w, 1);
    if (k == 0) {
        a->Res->answer = (int)w;
    }

    /*reduce_sum(p,&(double)r[k].count, 1);
    if(k == 0) {
        a->Res->answer = r[k].count;
    }*/
    return 0;
}
