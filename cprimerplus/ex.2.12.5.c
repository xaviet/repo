#include "stdio.h"

int br(void)
{
  printf("Brazil,Russia");
  return(0);
}

int ic(void)
{
  printf("India,China");
  return(0);
}

int main(void)
{
  br();
  printf(",");
  ic();
  printf("\n");
  ic();
  printf("\n");
  br();
  printf("\n");
  return(0);
}
