#include "linkedlistlib.h"
#include "stdio.h"
#include "stdlib.h"
#include "sys/socket.h"
#include "netinet/in.h"
#include "sys/types.h"
#include "string.h"
#include "sys/time.h"
#include "time.h"
#include "unistd.h"
#include "arpa/inet.h"
#include "regex.h"
#include "commonlib.h"

struct s
{
  char c[8];
};

int main()
{
  float cost=12.99;
  float percent=80.0;
  printf("\a\nThis copy of \"War and Peace\" sells for $%2.2f.\nThat is %2.0f%% of list.\n",cost,percent);
  struct s s1;
  strcpy(s1.c,"12345678");
  memDisp(&s1,8);
  free(NULL);
  return(0);
}
