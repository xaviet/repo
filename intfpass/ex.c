#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "errno.h"
#include "unistd.h"
#include "sys/socket.h"
#include "sys/types.h"
#include "netinet/in.h"
#include "netinet/ip.h"
#include "netinet/if_ether.h"
#include "net/if.h"
#include "sys/ioctl.h"
#include "linux/if_packet.h"
#include "net/ethernet.h"

int main(int argc,char **argv)
{
  int sock,n;
  char buffer[2048];
  struct ethhdr *eth;
  struct ifreq ethreq;
  int firstVid,secondVid,delayUs=1;
  //if(argc<3)
  //{
    //printf("\nethcap 1st_vid 2nd_vid delay_us\n");
    //return(1);
  //}
  //sscanf(argv[1],"%u",&firstVid);
  //sscanf(argv[2],"%u",&secondVid);
  //sscanf(argv[3],"%u",&delayUs);
  //printf("Delay: %u, 1st_vid: %u, 2nd_vid: %u\n",delayUs,firstVid,secondVid);
  
  sock=socket(PF_PACKET,SOCK_RAW,htons(ETH_P_ALL));
  if (0>sock) 
  {
    perror("socket");
    return(1);
  }
  strncpy(ethreq.ifr_name,"eno33554984",IFNAMSIZ);
  if(-1==ioctl(sock,SIOCGIFFLAGS,&ethreq))
  {
    perror("SIOCGIFFLAGS");
    close(sock);
    return(1);
  }
  ethreq.ifr_flags |=IFF_PROMISC;
  if(-1==ioctl(sock,SIOCGIFFLAGS,&ethreq))
  {
    perror("SIOCGIFFLAGS");
    close(sock);
    return(1);
  }
  if(-1==ioctl(sock,SIOCGIFHWADDR,&ethreq))
  {
    perror("ioctl SIOCGIFHWADDR");
    close(sock);
    return(-1);
  }
  while (1) 
  {
    n=recvfrom(sock,buffer,2048,0,0,0);
    eth=(struct ethhdr*)buffer;
    if(n>0)
    {
      printf("sock[]-DMAC:%02x:%02x:%02x:%02x:%02x:%02x; ",eth->h_dest[0],eth->h_dest[1],eth->h_dest[2],eth->h_dest[3],eth->h_dest[4],eth->h_dest[5]);
      printf("SMAC:%02x:%02x:%02x:%02x:%02x:%02x\n",eth->h_source[0],eth->h_source[1],eth->h_source[2],eth->h_source[3],eth->h_source[4],eth->h_source[5]);
      usleep(delayUs);
    }
  }
  return(0);
}
