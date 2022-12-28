#include "sequence.h"

void reduce_sum(int p, double *a = nullptr, int n = 0) {
    static pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
    static pthread_cond_t c_in = PTHREAD_COND_INITIALIZER;
    static pthread_cond_t c_out = PTHREAD_COND_INITIALIZER;
    static int t_in = 0;
    static int t_out = 0;
    static double *r = nullptr;
    int i = 0;
    if (p <= 1) return;
    pthread_mutex_lock(&m);
    if (r == nullptr)
        r = a;
    else
        for (i = 0; i < n; i++)
            r[i] += a[i];
    t_in++;
    if (t_in >= p) {
        t_out = 0;
        pthread_cond_broadcast(&c_in);
    } else
        while (t_in < p)
            pthread_cond_wait(&c_in, &m);

    if (r != a)
        for (i = 0; i < n; i++)
            a[i] = r[i];
    t_out++;
    if (t_out >= p) {
        t_in = 0;
        r = nullptr;
        pthread_cond_broadcast(&c_out);
    } else
        while (t_out < p)
            pthread_cond_wait(&c_out, &m);
    pthread_mutex_unlock(&m);
}

double get_cpu_time() {
    struct rusage buf;
    getrusage(RUSAGE_THREAD, &buf);
    return buf.ru_utime.tv_sec + buf.ru_utime.tv_usec / 1.e+6;
}



int find_6y_numbers(Args *arg) {
    int k = arg->k;
    results *r = arg->res;
    long long unsigned int i, j, start = r[k].start, finish = r[k].start + r[k].ch_size - 1;
    long long unsigned int last = 0, first = 0, second = 0, pred_last = 0, pred_pred_last = 0, prev = 0, current_max = 0;
    bool is_prime;
    start = (start < 2 ? 2 : start);
    r[k].count_six_prime2 = 0;
    for (i = start; i <= finish; i++) {
        is_prime = true;
        // Здесь определям, простое оно или нет
        for (j = 2; j * j <= i; j++) {
            if (i % j == 0) {
                is_prime = false;
                break;
            }
        }
        if (is_prime) {
            if (first == 0) {
                first = i;
            } else {
                if (second == 0) {
                    second = i;
                }
            }
            last = i;
            pred_pred_last = pred_last;
            pred_last = prev;
            if ((prev > 0) && (i - prev == 6)) {
                r[k].count_six_prime2++;
                current_max = i;
            }
            if ((pred_pred_last > 0) && (i - pred_pred_last == 6)) {
                r[k].count_six_prime2++;
            }
            prev = i;
        }
    }
    r[k].first_prime = first;
    r[k].second_prime = second;
    r[k].prev_last_prime = pred_last;
    r[k].last_prime = last;
    r[k].current_max = current_max;
    return 0;
}

int global_res_fun(Args *arg) {
    int i, n, p = arg->p, current_count;
    results *r = arg->res;
    long long unsigned int tek_last, tek_prev_last, tek_max;
    current_count = r[0].glob_res->current_count;
    tek_max = r[0].glob_res->max_six;
    n = r[0].glob_res->n;
    tek_last = r[0].glob_res->last;
    tek_prev_last = r[0].glob_res->prev_last;
    for (i = 0; i < p; i++) {
        if (tek_last > 0) {
            if (r[i].first_prime > 0) {
                if (r[i].first_prime - tek_last == 6) {
                    current_count++;
                    tek_max = r[i].first_prime;
                }
                if ((tek_prev_last > 0) && (r[i].first_prime - tek_prev_last == 6)) {
                    current_count++;
                    tek_max = r[i].first_prime;
                }
            }
            r[0].glob_res->current_count = current_count;
            r[0].glob_res->max_six = tek_max;
            if (current_count == n) {
                r[0].glob_res->index_last = i;
                return i;
            }
            if (r[i].second_prime > 0) {
                if (r[i].second_prime - tek_last == 6) {
                    current_count++;
                    //Непонятно, вляется ли максимальным
                    tek_max = r[i].second_prime;
                }
            }
        }
        r[0].glob_res->current_count = current_count;
        r[0].glob_res->max_six = tek_max;
        current_count += r[i].count_six_prime2;
        if (current_count == n) {
            r[0].glob_res->current_count = current_count;
            r[0].glob_res->max_six = tek_max;
        }
        if (current_count >= n) {
            r[0].glob_res->index_last = i;
            return i;
        }
        if (r[i].prev_last_prime > 0) {
            tek_last = r[i].last_prime;
            tek_prev_last = r[i].prev_last_prime;
        } else if (r[i].last_prime > 0) {
            tek_prev_last = tek_last;
            tek_last = r[i].last_prime;
        }

    }
    r[0].glob_res->current_count = current_count;
    r[0].glob_res->max_six = tek_max;
    r[0].glob_res->last = tek_last;
    r[0].glob_res->prev_last = tek_prev_last;
    return -1;
}

void *thread_func(void *ptr) {
    Args *arg = (Args *) ptr;
    int u = arg->k;
    int p = arg->p; 
    int n = arg->n;
    int *glob_count;
    results *r = arg->res;
    long long unsigned int start = u * r[0].ch_size + 1, i, j, prev = 0, pred_prev = 0;
    long long unsigned int *glob_max;
    bool is_prime;
    double t_cpu = 0.;
    glob_count = &r[0].glob_res->current_count;
    glob_max = &r[0].glob_res->max_six;
    r[u].start = start;
    t_cpu = get_cpu_time();
    while (true) {
        find_6y_numbers(arg);
        reduce_sum(p);
        if (u == 0) global_res_fun(arg);
        reduce_sum(p);
        if (r[0].glob_res->index_last >= 0) break;
        r[u].start += p * r[0].ch_size;
    }
    if ((u == 0) && ((*glob_count) < n))
        for (i = r[r[0].glob_res->index_last].start;; i++) {
            if (i < 2) continue;
            is_prime = true;
            for (j = 2; j * j <= i; j++) {
                if (i % j == 0) {
                    is_prime = false;
                    break;
                }
            }
            if (is_prime) {
                if ((prev > 0) && (i - prev == 6)) {
                    (*glob_count)++;
                    (*glob_max) = i;
                }
                if ((pred_prev > 0) && (i - pred_prev == 6)) {
                    (*glob_count)++;
                    (*glob_max) = i;
                }
                if ((*glob_count) == n) break;
                pred_prev = prev;
                prev = i;
            }
        }
    t_cpu = get_cpu_time() - t_cpu;
    r[u].time_cpu = t_cpu;
    return nullptr;
}