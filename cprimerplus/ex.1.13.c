#include "stdio.h"

int main(void)
{
  float t_i=0;
  printf("Please input length(inch):");
  scanf("%f",&t_i);
  printf("%f inch equal to %f cm.\n",t_i*2.54);
  return(0);
}
