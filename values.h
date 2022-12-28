#include <cmath>
#include "matrix.h"

#define EPS 1e-16

int TridiagonalAlgo(int n, double* a,double eps);
int values(int n, double* a, double* x,double eps);

int n_(double m, double* a, int n);

int k_value(int k, int n, double* A, double *x, double eps, int* its);





double calcNorm(double* a, int n);

double Residual1(int n, double* a, double* x);
double Residual2(int n, double* a, double* x);
