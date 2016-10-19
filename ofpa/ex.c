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
  struct s s1;
  strcpy(s1.c,"12345678");
  memDisp(&s1,8);
  
  return(0);
}
