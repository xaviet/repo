#include "stdio.h"

int main(void)
{
  int i=0x7fffffff;
  printf("%d,%d\n",i,i+1);
  
  float f=0.123456789;
  float f1=f*1e100;
  float f2=f/10;
  printf("%f\n%f",f1,f2);

  return(0);
}
