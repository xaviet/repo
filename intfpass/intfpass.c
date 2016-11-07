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

int initSock(char* v_intf,struct ifreq* v_ethreq,struct sockaddr_ll* v_sll)
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
  printf("%s[%d] bind MAC:%02X-%02X-%02X-%02X-%02X-%02X\n",v_intf,t_sock,v_ethreq->ifr_hwaddr.sa_data[0]&0xff,v_ethreq->ifr_hwaddr.sa_data[1]&0xff,v_ethreq->ifr_hwaddr.sa_data[2]&0xff,v_ethreq->ifr_hwaddr.sa_data[3]&0xff,v_ethreq->ifr_hwaddr.sa_data[4]&0xff,v_ethreq->ifr_hwaddr.sa_data[5]&0xff);
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
    printf("sock[%d](ln:%d\t)-DMAC:%02x:%02x:%02x:%02x:%02x:%02x; ",v_sock,t_recvLen,t_eth->h_dest[0],t_eth->h_dest[1],t_eth->h_dest[2],t_eth->h_dest[3],t_eth->h_dest[4],t_eth->h_dest[5]);
    printf("SMAC:%02x:%02x:%02x:%02x:%02x:%02x\n",t_eth->h_source[0],t_eth->h_source[1],t_eth->h_source[2],t_eth->h_source[3],t_eth->h_source[4],t_eth->h_source[5]);
  }
  return(0);
}

int main(int argc,char **argv)
{
  char buffer[def_buffsize]={0};
  int delayUs=1;
  
  char intf1[def_namedsize]="eno16777736";  
  struct ifreq t_ethreq1={0};
  struct sockaddr_ll t_sll1={0};
  int sock1=0;
  //struct sockaddr_ll destSll1={0};
  
  char intf2[def_namedsize]="eno33554984";
  struct ifreq t_ethreq2={0};
  struct sockaddr_ll t_sll2={0};
  int sock2=0;
  //struct sockaddr_ll destSll2={0};

  if(argc>=3)
  {
    sscanf(argv[1],"%s",intf1);
    sscanf(argv[2],"%s",intf2);
    sscanf(argv[3],"%u",&delayUs);
  }
  else
  {
    printf("\ncommand notice: intfpass intf1 intf2 delayus\n");
  }
  printf("\nDelay: %uus, intf1: %s, intf2: %s\n",delayUs,intf1,intf2);
  sock1=initSock(intf1,&t_ethreq1,&t_sll1);
  if(sock1<=0)
  {
    printf("initSock %s fail!\n",intf1);
    return(0);
  }
  sock2=initSock(intf2,&t_ethreq2,&t_sll2);
  if(sock2<=0)
  {
    printf("initSock %s fail!\n",intf2);
    return(0);
  }
  while(1) 
  {
    recvPacket(sock1,buffer,&t_sll1);
    recvPacket(sock2,buffer,&t_sll2);
    usleep(delayUs);
  }
  return(0);
}

  //memset(v_sll,0,sizeof(struct sockaddr_ll));
  //v_sll->sll_family=AF_PACKET;
  //v_sll->sll_protocol=htons(ETH_P_ALL);
  //v_sll->sll_ifindex=v_ethreq->ifr_ifindex;
  ////v_sll->sll_halen=ETH_ALEN;
  //memcpy(v_sll->sll_addr,v_ethreq->ifr_hwaddr.sa_data,ETH_ALEN);
  ////v_sll->sll_hatype=ARPHRD_ETHER;
  ////v_sll->sll_pkttype=PACKET_OTHERHOST;

//if(setsockopt(t_sock,SOL_SOCKET,SO_BINDTODEVICE,(char*)v_ethreq,sizeof(struct ifreq))<0)
  //{
    //perror("setsockopt SO_BINDTODEVICE");
    //close(t_sock);
    //return(-1);
  //}

//int initSock(char* v_intf)
//{
  //int t_sock=0;
  //struct ifreq t_ethreq={0};
  //struct sockaddr_ll t_sll={0};
  //if(0>(t_sock=socket(PF_PACKET,SOCK_RAW,htons(ETH_P_ALL)))) 
  //{
    //perror("socket");
    //return(-1);
  //}
  //strncpy(t_ethreq.ifr_name,v_intf,IFNAMSIZ);
  //if(-1==ioctl(t_sock,SIOCGIFFLAGS,&t_ethreq))
  //{
    //perror("ioctl SIOCGIFFLAGS");
    //close(t_sock);
    //return(1);
  //}
  //t_ethreq.ifr_flags|=IFF_PROMISC;
  //if(-1==ioctl(t_sock,SIOCGIFFLAGS,&t_ethreq))
  //{
    //perror("ioctl SIOCGIFFLAGS");
    //close(t_sock);
    //return(-1);
  //}
  //if(-1==ioctl(t_sock,SIOCGIFHWADDR,&t_ethreq))
  //{
    //perror("ioctl SIOCGIFHWADDR");
    //close(t_sock);
    //return(-1);
  //}
  //t_sll.sll_family=AF_PACKET;
  //t_sll.sll_ifindex=t_ethreq.ifr_ifindex;
  //t_sll.sll_protocol=htons(ETH_P_ALL);
  //t_sll.sll_halen=ETH_ALEN;
  //memcpy(t_sll.sll_addr,t_ethreq.ifr_hwaddr.sa_data,ETH_ALEN);
  //printf("%s[%d] bind MAC:%02X-%02X-%02X-%02X-%02X-%02X\n",v_intf,t_sock,t_sll.sll_addr[0],t_sll.sll_addr[1],t_sll.sll_addr[2],t_sll.sll_addr[3],t_sll.sll_addr[4],t_sll.sll_addr[5]);
  //t_sll.sll_hatype=ARPHRD_ETHER;
  //t_sll.sll_pkttype=PACKET_OTHERHOST;
  ////if(bind(t_sock,(struct sockaddr*)&t_sll,sizeof(t_sll))<0)
  ////{
    ////perror("bind");
    ////close(t_sock);
    ////return(-1);
  ////}
  //return(t_sock);
//}

//int main(int argc,char **argv)
//{
  //int sock1=0,recvLen=0;
  ////int sock2=0;
  //char buffer[2048]={0};
  ////struct sockaddr_ll destSll1={0};
  ////struct sockaddr_ll destSll2={0};
  //char intf1[255]="eno16777736";
  //char intf2[255]="eno33554984";
  //int delayUs=1;
  //if(argc>=3)
  //{
    //sscanf(argv[1],"%s",intf1);
    //sscanf(argv[2],"%s",intf2);
    //sscanf(argv[3],"%u",&delayUs);
  //}
  //else
  //{
    //printf("\ncommand notice: intfpass intf1 intf2 delayus\n");
  //}
  //printf("Delay: %uus, intf1: %s, intf2: %s\n",delayUs,intf1,intf2);
  //if((sock1=initSock(intf1)<=0))
  //{
    //printf("initSock %s fail!\n",intf1);
    //return(0);
  //}
  //while(1) 
  //{
    //recvLen=recvfrom(sock1,buffer,2048,0,0,0);
    //if(recvLen>0)
    //{
      //printf("sock[%d]-DMAC:%02x:%02x:%02x:%02x:%02x:%02x; ",sock1,buffer[0],buffer[1],buffer[2],buffer[3],buffer[4],buffer[5]);
      //printf("SMAC:%02x:%02x:%02x:%02x:%02x:%02x\n",buffer[6],buffer[7],buffer[8],buffer[9],buffer[10],buffer[11]);
    //}
    //usleep(delayUs);
  //}
  //return(0);
//}

    //n=recvfrom(sock,buffer,2048,0,0,0);
    //eth=(struct ethhdr*)buffer;
    //buffer[2]=0xa5;buffer[8]=0xa5;
    //printf("=====================================\n");
    //printf("%d bytes read\n",n);
    //printf("Dest MAC addr:%02x:%02x:%02x:%02x:%02x:%02x\n",eth->h_dest[0],eth->h_dest[1],eth->h_dest[2],eth->h_dest[3],eth->h_dest[4],eth->h_dest[5]);
    //printf("Source MAC addr:%02x:%02x:%02x:%02x:%02x:%02x\n",eth->h_source[0],eth->h_source[1],eth->h_source[2],eth->h_source[3],eth->h_source[4],eth->h_source[5]);
    //usleep(delayUs);
    //usleep(100000);
    //memcpy(dest_sll.sll_addr,eth->h_dest,ETH_ALEN);
    //sendto(sock,buffer,n,0,(const struct sockaddr*)(&dest_sll),sizeof(dest_sll));
    
    
  //memcpy(src_mac,ethreq.ifr_hwaddr.sa_data,ETH_ALEN);
  //printf("MAC :%02X-%02X-%02X-%02X-%02X-%02X\n",src_mac[0],src_mac[1],src_mac[2],src_mac[3],src_mac[4],src_mac[5]);
  //memset(&sll,0,sizeof(sll));
  //sll.sll_family=AF_PACKET;
  //sll.sll_ifindex=ethreq.ifr_ifindex;
  //sll.sll_protocol=htons(ETH_P_ALL);
  //sll.sll_halen=ETH_ALEN;
  ////sll.sll_hatype=ARPHRD_ETHER;
  //sll.sll_pkttype=PACKET_OTHERHOST;
  //memcpy(sll.sll_addr,ethreq.ifr_hwaddr.sa_data,ETH_ALEN);
  ////if(bind(sock,(struct sockaddr*)&sll,sizeof(sll))<0)
  ////{
    ////perror("bind");
    ////close(sock);
    ////return(1);
  ////}

  //dest_sll.sll_family=AF_PACKET;
  //dest_sll.sll_ifindex=ethreq.ifr_ifindex;
  //dest_sll.sll_protocol=htons(ETH_P_ALL);
  //dest_sll.sll_halen=ETH_ALEN;
  //dest_sll.sll_pkttype=PACKET_OTHERHOST;
