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
        if (fscanf(inp, "%lf", a + i) != 1) {
            fclose(inp);
            return -2;
        }
    }

    fclose(inp);
    return 0;
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
/*int f1(const char* name, int* count, double* sum1) {// подсчет локальных минимумов
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
}*/
/*double get_full_time()
{
  struct timeval buf;
  gettimeofday(&buf, NULL);
  return buf.tv_sec + buf.tv_usec / 1.e6;
}

double get_CPU_time()
{
  struct rusage buf;
  getrusage(RUSAGE_THREAD, &buf);
  return buf.ru_utime.tv_sec + buf.ru_utime.tv_usec / 1.e6;
}*/
void full_array(double *arr, int n, int p, int k, Results *res) {
    int l = n % p;
    int lenk = n / p + (k < l ? 1 : 0);
    int first = (k < l ? k * lenk : k * lenk + l);
    //printf("thr %d     %d\n", k, first);
    
    if (first) {
        res[k].flagl1 = 1;
        res[k].prev1 = arr[first - 1];
    }
    if(first - 1) {
        res[k].flagl2 = 1;
        res[k].prev2 = arr[first - 2];
    }
    if (first + lenk < n) {
        res[k].flagr1 = 1;
        res[k].next1 = arr[first + lenk];
    }
    if(first + lenk + 1< n) {
        res[k].flagr2 = 1;
        res[k].next2 = arr[first + lenk + 1];
    }
}
int count_change(double *arr, int n, int p, int k, Results *res) {
    int l = n % p;
    int lenk = n / p + (k < l ? 1 : 0);
    int first = (k < l ? k * lenk : k * lenk + l);
    int count = 0;
    double a = 0;
    double prev = 0;
    double pprev = 0;
    int i = 0;
    if (p > n) return 0;
    pprev = arr[first];
    prev = arr[first + 1];
    if(res[k].flagl1 == 1 && res[k].flagl2 == 1) {
        if(lenk == 1) {
            if(res[k].flagr1 == 1 && res[k].flagr2 == 1) {
                a = res[k].prev1 * res[k].prev2 * res[k].next1 * res[k].next2;
                if(a > 0 || (a - 0) < EPS) {
                    arr[first] = pow(a, 1/4.0);
                    return 1;
                } 
                return 0;
            }
            return 0;
        }
        if(lenk == 2) {
            if(res[k].flagr1 == 1) {
                a = res[k].prev1 * res[k].prev2 * arr[first + 1] * res[k].next1;
                if(a > 0 || (a - 0) < EPS) {
                    pprev = arr[first];
                    arr[first] = pow(a, 1/4.0);
                    count++;
                }
                if(res[k].flagr2 == 1) {
                    a = res[k].prev1 * pprev * res[k].next1 * res[k].next2;
                    if(a > 0 || (a - 0) < EPS) {
                        arr[first + 1] = pow(a, 1/4.0);
                        count++;
                        return count;
                    }
                    return count;
                }
                return count;
            }
            return 0;
        }
        if(lenk == 3) {
            a = res[k].prev1 * res[k].prev2 * arr[first + 1] * arr[first + 2];
            if(a > 0 || (a - 0) < EPS) {
                pprev = arr[first];
                arr[first] = pow(a, 1/4.0);
                count++;
                printf("HERE\n");
            }
            if(res[k].flagr1 == 1){
                a = res[k].prev1 * pprev * arr[first + 2] * res[k].next1;
                if(a > 0 || (a - 0) < EPS) {
                    prev = arr[first + 1];
                    arr[first + 1] = pow(a, 1/4.0);
                    count++;
                }
            }
            if(res[k].flagr2 == 1) {
                a = pprev * prev * res[k].next1 * res[k].next2;
                if(a > 0 || (a - 0) < EPS) {
                    pprev = prev;
                    prev = arr[first + 2];
                    arr[first + 2] = pow(a, 1/4.0);
                    count++;
                }
            }
            return count;
        }
        if(lenk > 3) {
            a = res[k].prev1 * res[k].prev2 * arr[first + 1] * arr[first + 2];
            if(a > 0 || fabs(a - 0) < EPS) {
                pprev = arr[first];
                arr[first] = pow(a, 1/4.0);
                count++;
            }
            a = res[k].prev1 * pprev * arr[first + 2] * arr[first + 3];
            if(a > 0 || fabs(a - 0) < EPS) {
                prev = arr[first + 1];
                arr[first + 1] = pow(a, 1/4.0);
                count++;
            }
        }
    }
    for(i = 2; i < lenk - 2; i++) {
        a = pprev * prev * arr[first + i + 1] * arr[first + i + 2];
        pprev = prev;
        prev = arr[first + i];
        if(a > 0 || fabs(a - 0) < EPS) {
            arr[first + i] = pow(a, 1/4.0);
            count++;
        }
    }
    if(lenk == 3) {
        if(res[k].flagr1 == 1 && res[k].flagr2 == 1){
            a = pprev * prev * res[k].next1 * res[k].next2;
            if(a > 0 || fabs(a - 0) < EPS) {
            arr[first + 2] = pow(a, 1/4.0);
            count++;
            }
        }
        return count;
    }
    if(lenk > 3) {
    if(res[k].flagr1 == 1) {
        a = pprev * prev * arr[first + lenk - 1] * res[k].next1;
        pprev = prev;
        prev = arr[first + lenk - 2];
        if(a > 0 || fabs(a - 0) < EPS) {
            arr[first + lenk - 2] = pow(a, 1/4.0);
            count++;
        }
        if(res[k].flagr2 == 1) {
            a = pprev * prev * res[k].next1 * res[k].next2;
            if(a > 0 || fabs(a - 0) < EPS) {
                arr[first + lenk - 1] = pow(a, 1/4.0);
                count++;
            }
        }
    }
    }
    return count;
}
void* thread_func(void *ptr) {
double time = 0;
Args* args = (Args*) ptr;
double* arr = args->arr;
int n = args->n;
int k = args->k;
int p = args->p;
Results *res = args->res;
int countk = 0;
if(n <= 4)
{
    return 0;
}
time = clock();
full_array(arr, n, p, k, res);
reduce_sum(p);
countk = count_change(arr,n, p, k, res);
reduce_sum(p);
time = (clock() - time) / CLOCKS_PER_SEC;
args -> count = countk;
args -> time = time;
return nullptr;
}
/*void* thread_func(void *ptr) {
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
        }
        reduce_sum(p);
        return 0;
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

    //reduce_sum(p,&(double)r[k].count, 1);
    //if(k == 0) {
        //a->Res->answer = r[k].count;
    //}
    return 0;
}*/