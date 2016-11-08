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

#define def_buffsize 2048
#define def_namedsize 255

int initSock(char* v_intf,struct ifreq* v_ethreq,struct sockaddr_ll* v_sll,struct sockaddr_ll* v_linksll)
{
  int t_sock=0;
  t_sock=socket(PF_PACKET,SOCK_RAW,htons(ETH_P_ALL));
  if(0>t_sock) 
  {
    perror("socket");
    return(-1);
  }
  strncpy(v_ethreq->ifr_name,v_intf,IFNAMSIZ);
  ioctl(t_sock,SIOCGIFFLAGS,v_ethreq);
  ioctl(t_sock,SIOCGIFINDEX,v_ethreq);
  v_ethreq->ifr_flags|=IFF_PROMISC;
  ioctl(t_sock,SIOCGIFFLAGS,v_ethreq);
  ioctl(t_sock,SIOCGIFHWADDR,v_ethreq);
  printf("%16s[%d] bind MAC:%02X-%02X-%02X-%02X-%02X-%02X\n",v_intf,t_sock,v_ethreq->ifr_hwaddr.sa_data[0]&0xff,v_ethreq->ifr_hwaddr.sa_data[1]&0xff,v_ethreq->ifr_hwaddr.sa_data[2]&0xff,v_ethreq->ifr_hwaddr.sa_data[3]&0xff,v_ethreq->ifr_hwaddr.sa_data[4]&0xff,v_ethreq->ifr_hwaddr.sa_data[5]&0xff);
  v_linksll->sll_ifindex=v_ethreq->ifr_ifindex;
  memcpy(v_linksll->sll_addr,v_ethreq->ifr_hwaddr.sa_data,ETH_ALEN);
  strncpy(v_ethreq->ifr_name,v_intf,IFNAMSIZ);
  if(setsockopt(t_sock,SOL_SOCKET,SO_BINDTODEVICE,(void *)v_ethreq,sizeof(struct ifreq))<0)
  {
    perror("setsockopt SO_BINDTODEVICE");
    close(t_sock);
    return(-1);
  }
  return(t_sock);
}

int recvPacket(int v_sock,char* v_buff,struct sockaddr_ll* v_sll)
{
  struct ethhdr* t_eth=NULL;
  int t_recvLen=0;
  unsigned int t_sllLen=sizeof(struct sockaddr_ll);
  t_recvLen=recvfrom(v_sock,v_buff,def_buffsize,0,(struct sockaddr*)v_sll,&t_sllLen);
  t_eth=(struct ethhdr*)v_buff;
  if(t_recvLen>0)
  {
    //printf("sock[%d](ln:%d\t)-DMAC:%02x:%02x:%02x:%02x:%02x:%02x; SMAC:%02x:%02x:%02x:%02x:%02x:%02x\n",v_sock,t_recvLen,t_eth->h_dest[0],t_eth->h_dest[1],t_eth->h_dest[2],t_eth->h_dest[3],t_eth->h_dest[4],t_eth->h_dest[5],t_eth->h_source[0],t_eth->h_source[1],t_eth->h_source[2],t_eth->h_source[3],t_eth->h_source[4],t_eth->h_source[5]);
    return(t_recvLen);
  }
  return((int)t_eth-(int)t_eth-1);
}

int sendPacket(int v_sock,char* v_buff,int v_len,struct sockaddr_ll* v_sll)
{
  return(sendto(v_sock,v_buff,v_len,0,(struct sockaddr*)v_sll,sizeof(struct sockaddr_ll)));
}

int main(int argc,char **argv)
{
  int delayUs=20000;
  
  int smac1[6]={0,0x11,0x11,0x11,0x11,0x11};
  char intf1[def_namedsize]="enp0s26f0u2";  
  struct ifreq t_ethreq1={0};
  struct sockaddr_ll t_sll1={0};
  int sock1=0;
  char buffer1[def_buffsize]={0};
  struct sockaddr_ll linkSll1={0};
  int len1=0;
  struct ethhdr* t_eth1=NULL;
  
  int smac2[6]={0,0x22,0x22,0x22,0x22,0x22};
  char intf2[def_namedsize]="enp8s0";
  struct ifreq t_ethreq2={0};
  struct sockaddr_ll t_sll2={0};
  int sock2=0;
  char buffer2[def_buffsize]={0};
  struct sockaddr_ll linkSll2={0};
  int len2=0;
  struct ethhdr* t_eth2=NULL;

  if(argc>=3)
  {
    sscanf(argv[1],"%s",intf1);
    sscanf(argv[2],"%02x:%02x:%02x:%02x:%02x:%02x",&(smac1[0]),&(smac1[1]),&(smac1[2]),&(smac1[3]),&(smac1[4]),&(smac1[5]));
    sscanf(argv[3],"%s",intf2);
    sscanf(argv[4],"%02x:%02x:%02x:%02x:%02x:%02x",&(smac2[0]),&(smac2[1]),&(smac2[2]),&(smac2[3]),&(smac2[4]),&(smac2[5]));
    sscanf(argv[5],"%u",&delayUs);
  }
  else
  {
    printf("\ncommand notice: intfpass intf1 smac1 intf2 smac2 delayus\nexample: ./intfpass enp0s26f0u2 00:11:11:11:11:11 enp8s0 00:22:22:22:22:22 1000\n");
  }
  printf("\nDelay: %uus, intf1: %s[smac-%02x:%02x:%02x:%02x:%02x:%02x], intf2: %s[smac-%02x:%02x:%02x:%02x:%02x:%02x]\n\n",delayUs,intf1,smac1[0],smac1[1],smac1[2],smac1[3],smac1[4],smac1[5],intf2,smac2[0],smac2[1],smac2[2],smac2[3],smac2[4],smac2[5]);
  sock1=initSock(intf1,&t_ethreq1,&t_sll1,&linkSll1);
  if(sock1<=0)
  {
    printf("initSock %s fail!\n",intf1);
    return(0);
  }
  sock2=initSock(intf2,&t_ethreq2,&t_sll2,&linkSll2);
  if(sock2<=0)
  {
    printf("initSock %s fail!\n",intf2);
    return(0);
  }
  printf("\n");
  while(1) 
  {
    len1=recvPacket(sock1,buffer1,&t_sll1);
    t_eth1=(struct ethhdr*)buffer1;
    len2=recvPacket(sock2,buffer2,&t_sll2);
    t_eth2=(struct ethhdr*)buffer2;
    if((len1>0)||(len2>0))
    {
      //printf("while 1\n");
      //usleep(delayUs);
      if((len1>0)
      &&((char)(t_eth1->h_source[0])==(char)(smac1[0]&0xff))
      &&((char)(t_eth1->h_source[1])==(char)(smac1[1]&0xff))
      &&((char)(t_eth1->h_source[2])==(char)(smac1[2]&0xff))
      &&((char)(t_eth1->h_source[3])==(char)(smac1[3]&0xff))
      &&((char)(t_eth1->h_source[4])==(char)(smac1[4]&0xff))
      &&((char)(t_eth1->h_source[5])==(char)(smac1[5]&0xff)))
      {
        //printf("while 2\n");
        printf("sock[%d](ln:%d\t)-DMAC:%02x:%02x:%02x:%02x:%02x:%02x; SMAC:%02x:%02x:%02x:%02x:%02x:%02x\n",sock1,len1,t_eth1->h_dest[0],t_eth1->h_dest[1],t_eth1->h_dest[2],t_eth1->h_dest[3],t_eth1->h_dest[4],t_eth1->h_dest[5],t_eth1->h_source[0],t_eth1->h_source[1],t_eth1->h_source[2],t_eth1->h_source[3],t_eth1->h_source[4],t_eth1->h_source[5]);
        len1=sendPacket(sock2,buffer1,len1,&t_sll2);
        printf("%d->%d send %d\t  %02x %02x %02x %02x %02x %02x %02x %02x \n",sock1,sock2,len1,buffer1[12]&0xff,buffer1[13]&0xff,buffer1[14]&0xff,buffer1[15]&0xff,buffer1[16]&0xff,buffer1[17]&0xff,buffer1[18]&0xff,buffer1[19]&0xff);
      }
      if((len2>0)
      &&((char)(t_eth2->h_source[0])==(char)(smac2[0]&0xff))
      &&((char)(t_eth2->h_source[1])==(char)(smac2[1]&0xff))
      &&((char)(t_eth2->h_source[2])==(char)(smac2[2]&0xff))
      &&((char)(t_eth2->h_source[3])==(char)(smac2[3]&0xff))
      &&((char)(t_eth2->h_source[4])==(char)(smac2[4]&0xff))
      &&((char)(t_eth2->h_source[5])==(char)(smac2[5]&0xff)))
      {
        //printf("while 3\n");
        printf("sock[%d](ln:%d\t)-DMAC:%02x:%02x:%02x:%02x:%02x:%02x; SMAC:%02x:%02x:%02x:%02x:%02x:%02x\n",sock2,len2,t_eth2->h_dest[0],t_eth2->h_dest[1],t_eth2->h_dest[2],t_eth2->h_dest[3],t_eth2->h_dest[4],t_eth2->h_dest[5],t_eth2->h_source[0],t_eth2->h_source[1],t_eth2->h_source[2],t_eth2->h_source[3],t_eth2->h_source[4],t_eth2->h_source[5]);
        len2=sendPacket(sock1,buffer2,len2,&t_sll1);
        printf("%d->%d send %d\t  %02x %02x %02x %02x %02x %02x %02x %02x\n",sock2,sock1,len2,buffer2[12]&0xff,buffer2[13]&0xff,buffer2[14]&0xff,buffer2[15]&0xff,buffer2[16]&0xff,buffer2[17]&0xff,buffer2[18]&0xff,buffer2[19]&0xff);
      }
    }
    //usleep(1);
  }
  close(sock1);
  close(sock2);
  return(0);
}
