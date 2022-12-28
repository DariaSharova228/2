#include "values.h"

double calcNorm(double* a, int n){
  double norm = 0, tmp = 0;
  int i, j;
  for(i = 0; i < n; i++){
    tmp = 0;
    for( j = 0; j < n; j++){
      tmp += fabs(a[i*n+j]);
    }
    norm = (tmp > norm? tmp: norm);
  }
  return norm;
}

double Residual1(int n, double* a, double* x){
  double sum = 0, norm = calcNorm(a, n);
  int i;

  for(i = 0; i < n; i++){
    sum += a[i*n+i] - x[i];
  }

  if(fabs(norm) < EPS*calcNorm(a, n)) return -1;
  return fabs(sum)/fabs(norm);
}

double Residual2(int n, double* a, double* x){
  double norm = calcNorm(a, n);

  double normA = 0, normx =0;
  int i, j;
  for(i = 0; i < n; i++){
    for(j = 0; j < n; j++){
      normA += a[i*n+j]*a[j*n+i];
    }
    normx += x[i] * x[i];
  }
  normA = sqrt(normA);
  normx = sqrt(normx);
  if(fabs(norm) < EPS*calcNorm(a, n))return -1;
  return fabs(normA-normx)/norm;
}
int TridiagonalAlgo(int n, double* matrix,double eps)
{
    double current_sum = 0., matrix_norm = 0.;
    double correction = 0., dot_product = 0.;

    for (int numb = 0; numb < n - 2; numb++) {

        current_sum = 0.;
        for (int j = numb + 2; j < n; j++) {
            current_sum += matrix[numb * n + j] * matrix[numb * n + j];
        }

        matrix_norm = sqrt(matrix[numb * n + numb + 1] * matrix[numb * n + numb + 1] + current_sum);

        if (matrix_norm - fabs(matrix[numb * n + numb + 1]) < eps*calcNorm(matrix,n)) {
            continue;
        }

        matrix_norm = (matrix[numb * n + numb + 1] >= 0. ? -matrix_norm : matrix_norm);

        matrix[numb * n + numb + 1] -= matrix_norm;

        correction = 2. / (matrix[numb * n + numb + 1] * matrix[numb * n + numb + 1] + current_sum);

        for (int i = numb + 1; i < n; ++i) {
            dot_product = 0.;
            for (int j = numb + 1; j < n; ++j) {
                dot_product += matrix[numb * n + j] * matrix[i * n + j];
            }

            for (int j = numb + 1; j < n; ++j) {
                matrix[i * n + j] -= correction * dot_product * matrix[numb * n + j];
            }
        }

        for (int j = numb + 1; j < n; ++j) {

            dot_product = 0.;

            for (int i = numb + 1; i < n; ++i) {
                dot_product += matrix[numb * n + i] * matrix[i * n + j];
            }

            for (int i = j; i < n; ++i) {
                matrix[i * n + j] -= correction * dot_product * matrix[numb * n + i];
            }
        }

        for (int j = numb + 1; j < n; ++j) {
            for (int i = j + 1; i < n; ++i) {
                matrix[j * n + i] = matrix[i * n + j];
            }
        }

        matrix[(numb + 1) * n + numb] = matrix[numb * (n + 1) + 1] = matrix_norm;

        for (int j = numb + 2; j < n; ++j) {
            matrix[numb * n + j] = matrix[j * n + numb] = 0.;
        }
    }
    return 0;
}



int values(int n, double* A, double* x,double eps)
{
  int k=1, res;
  int its = 0;
  while(k <= n)
  {
    res=k_value(k, n, A, x, eps, &its);
    if(res <= 0)
    {
      k += n;
    }
    k+=res;
  }

  return its;
}


int k_value(int k, int n, double* A, double *x, double eps, int* its)
{
  double b = 2*calcNorm(A, n), a = -b, c = (b+a)/2;
  double tmp=calcNorm(A,n);
  int na = n_(a, A, n), nb = n_(b, A, n), nc = n_(c, A, n);
  int i, it = 0;
  
  while(b - a > eps*tmp)
  {
    it++;
    if(b-c < eps*tmp || c-a < eps*tmp){
    
   
      a = b;
      continue;
    }
   
    if(nc < k)
    {
      a = c;
      na = nc;
    }
    else
    {
      b = c;
      nb = nc;
    }
    c = a/2 + b/2;
    nc = n_(c, A, n);
  }
  int res = nb-na;
  if(res <= 0)
  {
  
    return res;
  }
  for(i = k-1; i < k+res-1; i++)
  {
    x[i] = c;
  }
  *its += it;
  return res;
}


int n_(double lambda, double* a, int n)
{
  

  int i;
	int rezult;
	double l;

	l = a[0 * n + 0] - lambda;
	rezult = l < 0.0 ? 1 : 0;

	for (i = 1; i < n; ++i)
	{
		if (fabs(l) < 1e-100)
			l = 1e-10;

		l = a[i * n + i] - lambda - a[i * n + i - 1] * a[(i - 1) * n + i] / l;

		if (l < 0)
			++rezult;
	}

	return rezult;
}