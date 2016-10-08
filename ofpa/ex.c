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

int main()
{
  char s1[]="MAC  str:48df.3c00.010e";
  char s2[16]={0};
  int t_iMac[6]={0};
  sscanf(s1,"%*s%*4s%2x%2x.%2x%2x.%2x%2x",&(t_iMac[0]),&(t_iMac[1]),&(t_iMac[2]),&(t_iMac[3]),&(t_iMac[4]),&(t_iMac[5]));
  printf("%d",t_iMac[0]);
  return(0);
}
