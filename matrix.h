#ifndef MATRINIT
#define MATRINIT
#include <cstdio>
#include <cmath>


double initElement(int n,int s, int i, int j);

int readMatrix(double* a, int n, char* filename);

int initMatrix(double* a, int n, int s, char* filename,double eps);


void printMatrix(double* a, int l, int n, int r);
#endif
