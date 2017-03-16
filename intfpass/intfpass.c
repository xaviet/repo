#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/if_ether.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <time.h>
#include <sys/time.h>
#include <signal.h>

#define def_buffsize 2048
#define def_namedsize 255
#define def_packetBuffer 0x80000


struct packetBuffer
{
  unsigned int sSock;
  unsigned int dSock;
  char buff[def_buffsize];
  unsigned int length;
  unsigned int timestamp;
  struct sockaddr_ll* dSll;
};

unsigned int timerCount=0;

int setTimerOneMS()  
{  
    struct itimerval t_itv;  
    t_itv.it_interval.tv_sec=0;  
    t_itv.it_interval.tv_usec=1000;  
    t_itv.it_value.tv_sec=0;  
    t_itv.it_value.tv_usec=1000;  
    return(setitimer(ITIMER_REAL,&t_itv,NULL));  
}  

void signalTimerHandler()  
{  
    ++timerCount;  
}

struct packetBuffer* putBuff(struct packetBuffer* v_pBuff,
            unsigned int* v_putBuffPos,
            unsigned int* v_getBuffPos)
{
  struct packetBuffer* t_pb=NULL;
  if((*v_putBuffPos!=(*v_getBuffPos-1))||
    ((*v_getBuffPos==0)&&(*v_putBuffPos!=(def_packetBuffer-1))))
  {
    ((*v_putBuffPos)==(def_packetBuffer-1))?*v_putBuffPos=0:
                                            (*v_putBuffPos)++;
    t_pb=v_pBuff+*v_putBuffPos;
  }
  return(t_pb);
}

struct packetBuffer* getBuff(struct packetBuffer* v_pBuff,
            unsigned int* v_putBuffPos,
            unsigned int* v_getBuffPos)
{
  struct packetBuffer* t_pb=NULL;
  if((*v_getBuffPos!=(*v_putBuffPos-1))||
    ((*v_putBuffPos==0)&&(*v_getBuffPos!=(def_packetBuffer-1))))
  {
    ((*v_getBuffPos)==(def_packetBuffer-1))?*v_getBuffPos=0:
                                            (*v_getBuffPos)++;
    t_pb=v_pBuff+*v_getBuffPos;
  }
  return(t_pb);
}

unsigned int initSock(char* v_intf,
                      struct ifreq* v_ethreq,
                      struct sockaddr_ll* v_sll,
                      struct sockaddr_ll* v_linksll)
{ 
  unsigned int t_sock=0;
  t_sock=socket(AF_PACKET,SOCK_RAW,htons(ETH_P_ALL));
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
  printf("    %16s[%d] bind MAC:%02X-%02X-%02X-%02X-%02X-%02X\n",
        v_intf,t_sock,
        v_ethreq->ifr_hwaddr.sa_data[0]&0xff,
        v_ethreq->ifr_hwaddr.sa_data[1]&0xff,
        v_ethreq->ifr_hwaddr.sa_data[2]&0xff,
        v_ethreq->ifr_hwaddr.sa_data[3]&0xff,
        v_ethreq->ifr_hwaddr.sa_data[4]&0xff,
        v_ethreq->ifr_hwaddr.sa_data[5]&0xff);
  v_linksll->sll_ifindex=v_ethreq->ifr_ifindex;
  memcpy(v_linksll->sll_addr,v_ethreq->ifr_hwaddr.sa_data,ETH_ALEN);
  strncpy(v_ethreq->ifr_name,v_intf,IFNAMSIZ);
  if(setsockopt(t_sock,SOL_SOCKET,SO_BINDTODEVICE,
                (void *)v_ethreq,sizeof(struct ifreq))<0)
  {
    perror("setsockopt SO_BINDTODEVICE");
    close(t_sock);
    return(-1);
  }
  return(t_sock);
}

int recvPacket(unsigned int v_sSock,
              unsigned int v_dSock,
              struct packetBuffer* v_pb,
              unsigned int* v_putBuffPos,
              unsigned int* v_getBuffPos,
              struct sockaddr_ll* v_sSll,
              struct sockaddr_ll* v_dSll)
{
  struct packetBuffer* t_pb=putBuff(v_pb,v_putBuffPos,v_getBuffPos);
  if(t_pb==NULL)
  {
    return(-1);
  }
  struct ethhdr* t_eth=NULL;
  t_pb->length=0;
  unsigned int t_sllLen=sizeof(struct sockaddr_ll);
  t_pb->length=recvfrom(v_sSock,&(t_pb->buff),def_buffsize,MSG_DONTWAIT,
                    (struct sockaddr*)v_sSll,&t_sllLen);
  if((t_pb->length>=14)&&(t_pb->length!=-1))
  {
    t_eth=(struct ethhdr*)&(t_pb->buff);
    printf("sock[%d]recv(ln:%d\t)-DMAC:%02x:%02x:%02x:%02x:%02x:%02x;\
SMAC:%02x:%02x:%02x:%02x:%02x:%02x\n",
          v_sSock,
          t_pb->length,
          t_eth->h_dest[0],
          t_eth->h_dest[1],
          t_eth->h_dest[2],
          t_eth->h_dest[3],
          t_eth->h_dest[4],
          t_eth->h_dest[5],
          t_eth->h_source[0],
          t_eth->h_source[1],
          t_eth->h_source[2],
          t_eth->h_source[3],
          t_eth->h_source[4],
          t_eth->h_source[5]);
    t_pb->sSock=v_sSock;     
    t_pb->dSock=v_dSock; 
    t_pb->timestamp=timerCount;
    t_pb->dSll=v_dSll;
    return(t_pb->length);
  }
  else
  {
    v_putBuffPos--;
  }
  return(-1);
}

int sendPacket(struct packetBuffer* v_pb,
              unsigned int* v_putBuffPos,
              unsigned int* v_getBuffPos,
              struct sockaddr_ll* v_sll)
{
  struct packetBuffer* t_pb=putBuff(v_pb,v_putBuffPos,v_getBuffPos);
  if(t_pb==NULL)
  {
    return(-1);
  }
  unsigned int t_len=0;
  char* t_buff=v_pb->buff;
  t_len=sendto(v_pb->dSock,v_pb->buff,v_pb->length,0,
              (struct sockaddr*)v_sll,sizeof(struct sockaddr_ll));
  printf("sock[%d->%d]send(ln:%d:%d\t)%02x %02x %02x %02x \
%02x %02x %02x %02x\n",
        v_pb->sSock,v_pb->dSock,t_len,v_pb->length,
        t_buff[12]&0xff,
        t_buff[13]&0xff,
        t_buff[14]&0xff,
        t_buff[15]&0xff,
        t_buff[16]&0xff,
        t_buff[17]&0xff,
        t_buff[18]&0xff,
        t_buff[19]&0xff);
  return(t_len);
}

int main(int argc,char **argv)
{
  unsigned int delayMs=1;
  struct packetBuffer* pBuff=NULL;
  pBuff=(struct packetBuffer*)malloc(sizeof(struct packetBuffer)
                                    *def_packetBuffer);
  unsigned int putBuffPos=0;
  unsigned int getBuffPos=0;
  
  int smac1[6]={0x00,0x00,0x22,0x86,0x00,0x10};
  char intf1[def_namedsize]="eno16777736";  
  struct ifreq t_ethreq1={0};
  struct sockaddr_ll t_sll1={0};
  unsigned int sock1=0;
  struct sockaddr_ll linkSll1={0};
  
  int smac2[6]={0x00,0x00,0x22,0x86,0x00,0x10};
  char intf2[def_namedsize]="eno33554984";
  struct ifreq t_ethreq2={0};
  struct sockaddr_ll t_sll2={0};
  unsigned int sock2=0;
  struct sockaddr_ll linkSll2={0};

  if(argc>=3)
  {
    sscanf(argv[1],"%s",intf1);
    sscanf(argv[2],"%02x:%02x:%02x:%02x:%02x:%02x",
          &(smac1[0]),&(smac1[1]),&(smac1[2]),
          &(smac1[3]),&(smac1[4]),&(smac1[5]));
    sscanf(argv[3],"%s",intf2);
    sscanf(argv[4],"%02x:%02x:%02x:%02x:%02x:%02x",
          &(smac2[0]),&(smac2[1]),&(smac2[2]),
          &(smac2[3]),&(smac2[4]),&(smac2[5]));
    sscanf(argv[5],"%u",&delayMs);
  }
  else
  {
    printf("\ncommand notice: intfpass \
intf1 smac1 intf2 smac2 delayms\nexample: ./intfpass \
eno16777736 00:11:11:11:11:11 \
eno33554984 00:22:22:22:22:22 1\n");
  }
  printf("\nDelay: %ums\n\
intf1: %16s[smac-%02x:%02x:%02x:%02x:%02x:%02x]\n\
intf2: %16s[smac-%02x:%02x:%02x:%02x:%02x:%02x]\n\n",
        delayMs,
        intf1,smac1[0],smac1[1],smac1[2],smac1[3],smac1[4],smac1[5],
        intf2,smac2[0],smac2[1],smac2[2],smac2[3],smac2[4],smac2[5]);
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
  signal(SIGALRM,signalTimerHandler);  
  setTimerOneMS();
  while(1) 
  {
    recvPacket(sock1,sock2,pBuff,
              &putBuffPos,&getBuffPos,&t_sll1,&t_sll2);
    recvPacket(sock2,sock1,pBuff,
              &putBuffPos,&getBuffPos,&t_sll2,&t_sll1);
    while((pBuff[getBuffPos].timestamp)<(timerCount-delayMs))
    {
      if(sendPacket(pBuff,&putBuffPos,&getBuffPos,&t_sll2)<=0)
      {
        break;
      }
    }
  }
  close(sock1);
  close(sock2);
  free(pBuff);
  return(0);
}
