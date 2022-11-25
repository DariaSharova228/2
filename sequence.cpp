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

}*/
int f1(const char *name,int* len,int* min, double* value_min, double* first, double* last, int* first_len, int* last_len) {
    FILE *in;
    double x, y;
    int length = 0;
    int f_len = 1;
    int l_len = 1;
    int min_length = 0;
    double value_min_length = 0;
    double la;
    double fi;
    int flaag = 0;
    int length_seq = 0;
    if(!(in = fopen(name, "r"))) {
        return -1;
    }
    if (fscanf(in,"%lf",&x)!=1 && (!feof(in))){
        *len = length_seq;
        fclose(in);
        return -2;
    }
    fi = x;
    y = x;
    length++;
    length_seq++;
    la = x;
    while (fscanf(in,"%lf",&x)==1)
    {
        if (fabs(x-y)<EPS) length++;
        else 
        { 
            if (length !=1) {
                if(min_length == 0) {
                    min_length = length;
                    value_min_length = y;
                }
                else {
                    if(length < min_length) {
                        min_length = length;
                        value_min_length = y;
                    }
                }
            }
            if(flaag == 0) {
                flaag = 1;
                f_len = length;
                *first_len = f_len;
            }
            length = 1; 
        }
        y = x;
        la = x;
        length_seq++;
    }
       
    if (length>1) {
        if(min_length == 0) {
                    min_length = length;
                    value_min_length = y;
                }
                else {
                    if(length < min_length) {
                        min_length = length;
                        value_min_length = y;
                    }
                }
                l_len = length;
                *last_len = l_len;
    }
    //value_min_length = y;
    if (!feof(in)) 
    {
        fclose(in);
        return -2;
    }
    fclose(in);
    *len = length_seq;
    *min = min_length;
    *value_min = value_min_length;
    *first = fi;
    *last = la;
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
    int len = 0;
    int min_len = 0;
    int err = 0;
    double value = 0;
    int i;
    r[k].status = f1(name, &r[k].len, &r[k].local_min_length, &r[k].value, &r[k].first, &r[k].last, &r[k].first_length,&r[k].last_length);
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

    /*for(i = 0; i < p; i++) 
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
    mean = sum/count;*/
    int j = 0;
    if(p == 1) {
        value = r[0].value;
    }
    for(int i = 0; i < p - 1; i = j) {
        j = i + 1;
        while(j < p - 1 && r[j].len == 0) {
            j++;
        }
        if(min_len == 0){
            min_len = r[i].local_min_length;
            value = r[i].value;
        }
        if(fabs(r[i].last - r[j].first) < EPS) {
            len = r[i].last_length + r[j].first_length;
            if(r[j].local_min_length == r[j].len){
                r[j].last_length += r[i].last_length;
            }
            if(len < min_len && min_len > 0) {
                min_len = len;
                value = r[i].last;
            }
            if(min_len == 0) {
                min_len = len;
                value = r[i].last;
            }
        }
        if(r[i].local_min_length < min_len && r[i].local_min_length > 0) {
                min_len = r[i].local_min_length;
                value = r[i].value;
        }
        if(r[j].local_min_length < min_len && r[j].local_min_length > 0) {
                min_len = r[j].local_min_length;
                value = r[j].value;
        }
    }
    //printf(" %d\n", min_len);
    //printf(" %lf\n", value);
    /*for(int i = 0; i < p - 1; i = j + 1 + q) {
        j = i + 1;
        q = 0;
        while(j < p - 1 && r[j].len == 0) {
            j++;
            q++;
        }
        if(min_len == 0){
            min_len = r[i].local_min_length;
            value = r[i].value;
        }
        if(r[i].local_min_length < min_len) {
                min_len = r[i].local_min_length;
                value = r[i].value;
        }
        if(r[j].local_min_length < min_len) {
                min_len = r[j].local_min_length;
                value = r[j].value;
        }
        if(fabs(r[i].last - r[j].first) < EPS && fabs(r[i].value - r[i].last) < EPS && fabs(r[j].first - r[j].value) < EPS) {
                len = r[i].local_min_length + r[j].local_min_length + r[i].flag1;
                if(min_len == 0) {
                    min_len = len;
                    r[j].flag1 = len;
                    value = r[i].value;
                }
                if(len < min_len && min_len > 0) {
                    min_len = len;
                    r[j].flag1 = len;
                    value = r[i].value;
                }
        }
        if(fabs(r[i].last - r[j].first) < EPS && r[i].last_lon == 1 && r[j].first_lon == 1) {
                len = 2;
                if(min_len == 0) {
                    min_len = len;
                    r[j].flag1 = 2;
                    value = r[i].last;
                }
                if(len < min_len && min_len > 0) {
                    min_len = len;
                    r[j].flag1 = 2;
                    value = r[i].last;
                }
        }
        if(fabs(r[i].last - r[j].first) < EPS && fabs(r[i].value - r[i].last) < EPS && r[j].first_lon == 1) {
                len = r[i].local_min_length + 1 + r[i].flag1;
                if(len > r[j].local_min_length) {
                    min_len = r[j].local_min_length;
                    value = r[j].value;
                }
                if(len > r[i].local_min_length) {
                    min_len = r[i].local_min_length;
                    value = r[i].value;
                }
                if(min_len == 0) {
                    min_len = len;
                    r[j].flag1 = len;
                    value = r[i].last;
                }
                if(len < min_len && min_len > 0) {
                    min_len = len;
                    r[j].flag1 = len;
                    value = r[i].last;
                }
            }
            if(fabs(r[i].last - r[j].first) < EPS && fabs(r[j].value - r[j].first) < EPS && r[i].last_lon == 1){
                len = r[j].local_min_length + 1 + r[i].flag1;
                if(len > r[j].local_min_length) {
                    min_len = r[j].local_min_length;
                    value = r[j].value;
                }
                if(len > r[i].local_min_length) {
                    min_len = r[i].local_min_length;
                    value = r[i].value;
                }
                if(min_len == 0) {
                    min_len = len;
                    r[j].flag1 = len;
                    value = r[i].last;
                }
                if(len < min_len && min_len > 0) {
                    min_len = len;
                    r[j].flag1 = len;
                    value = r[i].last;
                }
            }
            if(r[i].local_min_length < min_len) {
                min_len = r[i].local_min_length;
                value = r[i].value;
        }
        if(r[j].local_min_length < min_len) {
                min_len = r[j].local_min_length;
                value = r[j].value;
        }
    }*/
    /*int q = 0;
    for(int i = 0; i < p; i = i + 1 + q) {
        int j = i + 1;
        q = 0;
        while(j < p) {
            if(r[j].len == 0) {
                j++;
                q++;
            }
            if(fabs(r[i].last - r[j].first) < EPS && fabs(r[i].value - r[i].last) < EPS && fabs(r[j].first - r[j].value) < EPS) {
                len = r[i].local_min_length + r[j].local_min_length + r[i].flag1;
                if(min_len == 0) {
                    min_len = len;
                    r[j].flag1 = len;
                    value = r[i].value;
                }
                if(len < min_len && min_len > 0) {
                    min_len = len;
                    r[j].flag1 = len;
                    value = r[i].value;
                }
                break;
            } 
            if(fabs(r[i].last - r[j].first) < EPS && r[i].last_lon == 1 && r[j].first_lon == 1) {
                len = 2;
                if(min_len == 0) {
                    min_len = len;
                    r[j].flag1 = 2;
                    value = r[i].last;
                }
                if(len < min_len && min_len > 0) {
                    min_len = len;
                    r[j].flag1 = 2;
                    value = r[i].last;
                }
                break;
            }
            if(fabs(r[i].last - r[j].first) < EPS && fabs(r[i].value - r[i].last) < EPS && r[j].first_lon == 1) {
                len = r[i].local_min_length + 1 + r[i].flag1;
                if(len > r[j].local_min_length) {
                    min_len = r[j].local_min_length;
                    value = r[j].value;
                }
                if(len > r[i].local_min_length) {
                    min_len = r[i].local_min_length;
                    value = r[i].value;
                }
                if(min_len == 0) {
                    min_len = len;
                    r[j].flag1 = len;
                    value = r[i].last;
                }
                if(len < min_len && min_len > 0) {
                    min_len = len;
                    r[j].flag1 = len;
                    value = r[i].last;
                }
                break;
            }
            if(fabs(r[i].last - r[j].first) < EPS && fabs(r[j].value - r[j].first) < EPS && r[i].last_lon == 1){
                len = r[j].local_min_length + 1 + r[i].flag1;
                if(len > r[j].local_min_length) {
                    min_len = r[j].local_min_length;
                    value = r[j].value;
                }
                if(len > r[i].local_min_length) {
                    min_len = r[i].local_min_length;
                    value = r[i].value;
                }
                if(min_len == 0) {
                    min_len = len;
                    r[j].flag1 = len;
                    value = r[i].last;
                }
                if(len < min_len && min_len > 0) {
                    min_len = len;
                    r[j].flag1 = len;
                    value = r[i].last;
                }
                break;
            }
        }
        if(min_len == 0){
            min_len = r[i].local_min_length;
            value = r[i].value;
            break;
        }
        if(r[i].local_min_length < min_len) {
                min_len = r[i].local_min_length;
                value = r[i].value;
                break;
        }
    }*/
    double w;
    r[k].status = f2(name,value,&r[k].count);
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
    if (k == 0) {
        a->Res->answer = (int)w;
    }
    //w = r[k].count;
    //reduce_sum(p, &w, 1);
    /*reduce_sum(p,&(double)r[k].count, 1);
    if(k == 0) {
        a->Res->answer = r[k].count;
    }*/
    //a->Res->answer += r[k].count;
    return 0;
}
