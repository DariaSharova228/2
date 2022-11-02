void *thread_func(void *ptr) {
    Args *a = (Args*) ptr;
    int k = a->k;
    char *name = a->name;
    Results *r = a->res;
    r[k].status = f(name, &r[k].count);
    return nullptr;
}

