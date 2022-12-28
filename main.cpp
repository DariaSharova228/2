#include <iostream>
#include <cstdio>
#include <ctime>
#include "values.h"


int main(int argc, char* argv[]){
  int n, m, k, res, its = 0;
  char* filename = 0;
  double eps, res1 = -1, res2 = -1, t1 = 0, t2 = 0;
  unsigned int start, end;

  double *a;
  double *x;
 

  if(!((argc == 5 || argc == 6) && sscanf(argv[1], "%d", &n) == 1 && sscanf(argv[2], "%d", &m) == 1 && sscanf(argv[3], "%lf", &eps) == 1 && sscanf(argv[4], "%d", &k) == 1) || (k == 0 && argc == 5)){
    printf("Usage %s n m eps k (file)\n", argv[0]);
    return 1;
  }

  if(k == 0){
    filename = argv[5];
  }


  a = new double[n*n];
  if(a == 0){
    printf("Not enough memory\n");
    printf ("%s : Residual1 = %e Residual2 = %e Iterations = %d \
  Iterations1 = %d Elapsed1 = %.2f Elapsed2 = %.2f\n",
  argv[0], res1, res2, its, its / n, t1, t2);
    delete[] a;
    return 1;
  }
  res = initMatrix(a, n, k, filename,eps);

  if (n==1)
  {
      printf("values %e \n",a[0]);
      printf ("%s : Residual1 = %e Residual2 = %e Iterations = %d \
    Iterations1 = %d Elapsed1 = %.2f Elapsed2 = %.2f\n",
    argv[0], 0., 0., its, its / n, t1, t2);
      delete[] a;
      return 0;

  }
  if(res == 1){
    delete[] a;
    printf("File not found.\n");
    printf ("%s : Residual1 = %e Residual2 = %e Iterations = %d \
  Iterations1 = %d Elapsed1 = %.2f Elapsed2 = %.2f\n",
  argv[0], res1, res2, its, its / n, t1, t2);
    return 1;
  }
  if(res == 2){
    delete[] a;
    printf("Error in file.\n");
    printf ("%s : Residual1 = %e Residual2 = %e Iterations = %d \
  Iterations1 = %d Elapsed1 = %.2f Elapsed2 = %.2f\n",
  argv[0], res1, res2, its, its / n, t1, t2);
    return 2;
  }


  printMatrix(a, n, n, m);
  printf("\n");
  if(res == 3)
  {
  
  printf ("Matrix is not symmetrical\n");
  printf ("%s : Residual1 = %e Residual2 = %e Iterations = %d \
  Iterations1 = %d Elapsed1 = %.2f Elapsed2 = %.2f\n",
  argv[0], res1, res2, its, its / n, t1, t2);
  delete [] a;
  return 3;
  }

  x = new double[n];
  
  if(x == 0) {
    printf("Not enough memory\n");
    delete[] a;
    if(x != 0)delete[] x;
    printf ("%s : Residual1 = %e Residual2 = %e Iterations = %d \
  Iterations1 = %d Elapsed1 = %.2f Elapsed2 = %.2f\n",
  argv[0], res1, res2, its, its / n, t1, t2);
    
    return 1;
  }

  start = clock();
  res = TridiagonalAlgo(n, a,eps);
  end = clock();
  t1 = (double)(end - start)/CLOCKS_PER_SEC;



  start = clock();
  its = values(n, a, x, eps);
  end = clock();
  t2 = (double)(end - start)/CLOCKS_PER_SEC;

  printMatrix(x, 1, n, m);

  initMatrix(a, n, k, filename,eps);

  res1 = Residual1(n, a, x);
  res2 = Residual2(n, a, x);

  printf ("%s : Residual1 = %e Residual2 = %e Iterations = %d \
  Iterations1 = %d Elapsed1 = %.2f Elapsed2 = %.2f\n",
  argv[0], res1, res2, its, its / n, t1, t2);
  delete[] a;
  delete[] x;
  
  return 0;
}
