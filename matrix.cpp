#include "matrix.h"

double initElement(int n,int s, int i, int j){
  if(s == 1){
    return n - (i>j?i:j) + 1;
  }
  if(s == 2){
    if(abs(i - j) == 1)return -1;
    else if(i == j) return 2;
    else return 0;
  }
  if(s == 3){
      if(i == j && i < n) return 1;
      else if(i == n) return j;
      else if(j == n) return i;
      else return 0;
  }
  if(s==4){
    return 1/(i+j-1.0);
  }
  return 0;
}

int readMatrix(double* a, int n, char* filename){
  int i, j, res = 0;
  FILE * f = 0;

  f = fopen(filename, "r");

  if(f == 0){
    return 1;
  }


  for(i = 0; i < n; i++){
    for(j = 0; j < n; j++){
      res += fscanf(f, "%lf", &a[i*n+j]);
    }
  }
  fclose(f);
  if(res != n*n){
    return 2;
  }
  return 0;
}

int initMatrix(double* a, int n, int s, char* filename,double eps){
  int i, j, res = 0;
  if(s != 0){
    for(i = 0; i < n; i++){
      for(j =0; j < n; j++){
        a[i*n+j] = initElement(n, s, i+1, j+1);
      }
    }
  }
  else{
    res = readMatrix(a, n, filename);
  }
  
 if( res==0)
 { for (i = 0; i < n; i++)
    {
      for (j = 0; j < i; j++)
        {
          if (fabs (a[i * n + j] - a[j * n + i]) >= eps)
            res=3;
        }
    }
    }
  return res;
}

void printMatrix(double* a, int l, int n, int r){
  int i, j;

  for(i = 0; i < (l>r?r:l); i++){
    for(j = 0; j < (n>r?r:n); j++){
      printf(" %10.3e", a[i*n + j]);
    }
    printf("\n");
  }
}

