/*
 *  frameforwarder.c
 *  by Pioevh@163.com 20170315
 */

//include files

#include "sys/socket.h"
#include "sys/ioctl.h"
#include "linux/if_packet.h"
#include "linux/if_ether.h"
#include "linux/if_arp.h"
#include "arpa/inet.h"
#include "unistd.h"
#include "stdint.h"
#include "stdlib.h"
#include "string.h"
#include "stdbool.h"
#include "stdio.h"
#include "time.h"
#include "sys/time.h"
#include "signal.h"
#include "pthread.h"

//  macro define

#define DEF_namedSize 64
#define DEF_frameSize 1518
#define DEF_frameBUfferVolum 0x10000
#define DEF_timeAccurSec 0
#define DEF_timeAccurUSec 1000
#define DEF_1stNIC "eno16777736"
#define DEF_2ndNIC "eno33554984"
#define DEF_delayTime 50

//  struct

struct s_ethernetSocket 
{
  int m_rawSocket;
  int m_isBind;
  struct sockaddr_ll m_socketAddress;
};

struct s_frameInfo
{
  struct s_ethernetSocket* mp_sSocket;
  struct s_ethernetSocket* mp_dSocket;
  char m_buff[DEF_frameSize];
  int m_length;
  long long m_timerCount;
  struct timeval m_rxTimeStamp;
  struct timeval m_txTimeStamp;
};

struct s_frameBuffer
{
  int m_running;
  pthread_mutex_t m_threadAccessing;
  long long m_timerCount;
  int m_dealyTime;
  char m_1stNicName[DEF_namedSize];
  struct s_ethernetSocket* mp_1stSocket;
  char m_2ndNicName[DEF_namedSize];
  struct s_ethernetSocket* mp_2ndSocket;
  int m_putBuffPos;
  int m_getBuffPos;
  struct s_frameInfo m_frameInfoList[DEF_frameBUfferVolum];
};

//  global

static struct s_frameBuffer* gp_frameBuffer=NULL;

//  function

int memDisp(char* vp_buffer,int v_length)
{
  int t_i=0;
  char* t_ch=vp_buffer;
  for(t_i=0;t_i<v_length;t_i++)
  {
    if(t_i%16==0)
    {
      printf("\n");
    }
    else if(t_i%8==0)
    {
      printf("  ");
    }
    printf("%02x ",(*(t_ch+t_i))&0xff);
  }
  printf("\n");
  return(0);
}

void getTime(struct timeval* vp_time)
{
  gettimeofday(vp_time,NULL);
}

void sigintHandler(int v_signalId)
{
    gp_frameBuffer->m_running=0;
}

int setTimer(int v_sec,int v_usec)  
{  
    struct itimerval t_itv;  
    t_itv.it_interval.tv_sec=v_sec;  
    t_itv.it_interval.tv_usec=v_usec;  
    t_itv.it_value.tv_sec=v_sec;  
    t_itv.it_value.tv_usec=v_usec;  
    return(setitimer(ITIMER_REAL,&t_itv,NULL));  
}  

void signalTimerHandler()  
{  
    ++gp_frameBuffer->m_timerCount;  
}


struct s_frameInfo* putBuff(struct s_frameInfo* vp_buff,int* vp_putBuffPos,int* vp_getBuffPos)
{
  struct s_frameInfo* tp_buff=vp_buff+*vp_putBuffPos;
  if(((*vp_putBuffPos)<((*vp_getBuffPos)-1))||(((*vp_putBuffPos)>=(*vp_getBuffPos))&&((*vp_putBuffPos)<(DEF_frameBUfferVolum-1))))
  {
    (*vp_putBuffPos)++;
  }
  else if(((*vp_putBuffPos)>(*vp_getBuffPos))&&((*vp_putBuffPos)==(DEF_frameBUfferVolum-1))&&((*vp_getBuffPos)>0))
  {
    (*vp_putBuffPos)=0;
  }
  else
  {
    perror("APP: Failed to put buffer -> exit");
    exit(1);
  }
  return(tp_buff);
}

struct s_frameInfo* getBuff(struct s_frameInfo* vp_buff,int* vp_putBuffPos,int* vp_getBuffPos)
{
  struct s_frameInfo* tp_buff=vp_buff+*vp_getBuffPos;
  if(((*vp_getBuffPos)<((*vp_putBuffPos)-1))||(((*vp_getBuffPos)>(*vp_putBuffPos))&&((*vp_getBuffPos)<(DEF_frameBUfferVolum-1))))
  {
    (*vp_getBuffPos)++;
  }
  else if(((*vp_getBuffPos)>(*vp_putBuffPos))&&((*vp_getBuffPos)==(DEF_frameBUfferVolum-1))&&((*vp_putBuffPos)>0))
  {
    (*vp_getBuffPos)=0;
  }
  else
  {
    tp_buff=NULL;
  }
  return(tp_buff);
}

void undoBuff(int* vp_buffPos)
{
  ((*vp_buffPos)==0)?(*vp_buffPos)=(DEF_frameBUfferVolum-1):(*vp_buffPos)--;
}

int getInterfaceIndex(int v_socket,char* v_nic)
{
  struct ifreq t_ifr;
  strncpy(t_ifr.ifr_name,v_nic,IFNAMSIZ);
  if(ioctl(v_socket,SIOCGIFINDEX,&t_ifr)==-1) 
  {
    perror("LINUX: Failed to get interface index -> exit");
    exit(1);
  }
  int interfaceIndex=t_ifr.ifr_ifindex;
  if(ioctl(v_socket,SIOCGIFFLAGS,&t_ifr)==-1)
  {
    perror ("LINUX: Problem getting device flags -> exit");
    exit(1);
  }
  t_ifr.ifr_flags|=IFF_PROMISC;
  if(ioctl(v_socket,SIOCSIFFLAGS,&t_ifr)==-1)
  {
    perror("LINUX: Setting device to promiscuous mode failed -> exit");
    exit(1);
  }
  return(interfaceIndex);
}

struct s_ethernetSocket* createSocket(char* vp_buffNICName,char* v_nic,uint8_t* v_dAddr)
{
  struct s_ethernetSocket* t_socket=calloc(1,sizeof(struct s_ethernetSocket));
  t_socket->m_rawSocket=socket(AF_PACKET,SOCK_RAW,htons(ETH_P_ALL));
  if (t_socket->m_rawSocket==-1)
  {
    perror("LINUX: Failed to creating raw socket -> exit");
    free(t_socket);
    exit(1);
  }
  strcpy(vp_buffNICName,v_nic);
  t_socket->m_socketAddress.sll_family=PF_PACKET;
  t_socket->m_socketAddress.sll_protocol=htons(ETH_P_IP);
  //t_socket->m_socketAddress.sll_protocol=htons(ETH_P_ALL);
  //t_socket->m_socketAddress.sll_protocol=htons(0x0806);
  t_socket->m_socketAddress.sll_ifindex=getInterfaceIndex(t_socket->m_rawSocket,v_nic);
  t_socket->m_socketAddress.sll_hatype=ARPHRD_ETHER;
  t_socket->m_socketAddress.sll_pkttype=PACKET_OTHERHOST;
  t_socket->m_socketAddress.sll_halen=ETH_ALEN;
  memset(t_socket->m_socketAddress.sll_addr,0,8);
  if(v_dAddr!=NULL)
  {
    memcpy(t_socket->m_socketAddress.sll_addr,v_dAddr,6);
  }
  t_socket->m_isBind=false;
  return(t_socket);
}

int createFrameBUffer()
{
  gp_frameBuffer=calloc(1,sizeof(struct s_frameBuffer));
  memset(gp_frameBuffer,0,sizeof(struct s_frameBuffer));
  gp_frameBuffer->m_running=1;
  gp_frameBuffer->m_timerCount=0;
  gp_frameBuffer->m_getBuffPos=0;
  gp_frameBuffer->m_putBuffPos=0;
  gp_frameBuffer->m_dealyTime=DEF_delayTime;
  return(0);
}

int receiveData(struct s_ethernetSocket* vp_socket,char* vp_buffer,int v_length)
{
  if(vp_socket->m_isBind==false) 
  {// raw socket must be binded,else the frame be duplicate
    if(bind(vp_socket->m_rawSocket,(struct sockaddr*)&vp_socket->m_socketAddress,sizeof(vp_socket->m_socketAddress))==0)
    {
      vp_socket->m_isBind = true;
    }
    else
    {
      perror("LINUX: Failed to bind -> exit");
      exit(1);
    }
  }
  return(recvfrom(vp_socket->m_rawSocket,vp_buffer,v_length,MSG_DONTWAIT,0,0));
}

void printBuffInfo(struct s_frameBuffer* vp_frameBuffer,struct s_frameInfo* vp_buffer,int v_rx0OrTx1)
{
  char t_ch=(v_rx0OrTx1==0)?'R':'T';
  printf("[%010ld.%-6ld]->[%010ld.%-6ld] %1d->%1d %cx %4dbyte putpos %-7d getpos %-7d",vp_buffer->m_rxTimeStamp.tv_sec,vp_buffer->m_rxTimeStamp.tv_usec,vp_buffer->m_txTimeStamp.tv_sec,vp_buffer->m_txTimeStamp.tv_usec,vp_buffer->mp_sSocket->m_rawSocket,vp_buffer->mp_dSocket->m_rawSocket,t_ch,vp_buffer->m_length,vp_frameBuffer->m_putBuffPos,vp_frameBuffer->m_getBuffPos);
  memDisp(vp_buffer->m_buff,0xe);
  printf("\n");  
}

void frameReceive(struct s_ethernetSocket* vp_socket,struct s_frameBuffer* vp_frameBuffer)
{
  struct s_frameInfo* tp_buffer=NULL;
  int t_len=0;
  tp_buffer=putBuff(vp_frameBuffer->m_frameInfoList,&(vp_frameBuffer->m_putBuffPos),&(vp_frameBuffer->m_getBuffPos));
  t_len=receiveData(vp_socket,tp_buffer->m_buff,DEF_frameSize);
  if(t_len>0)
  {
    getTime(&(tp_buffer->m_rxTimeStamp));
    memset(&(tp_buffer->m_txTimeStamp),0,sizeof(struct timeval));
    tp_buffer->m_timerCount=vp_frameBuffer->m_timerCount;
    tp_buffer->m_length=t_len;
    tp_buffer->mp_sSocket=vp_socket;
    if((vp_socket->m_rawSocket)==(vp_frameBuffer->mp_1stSocket->m_rawSocket))
    {
      tp_buffer->mp_dSocket=vp_frameBuffer->mp_2ndSocket;
    }
    else
    {
      tp_buffer->mp_dSocket=vp_frameBuffer->mp_1stSocket;
    }
    printBuffInfo(vp_frameBuffer,tp_buffer,0);
  }
  else
  {
    undoBuff(&(vp_frameBuffer->m_putBuffPos));
  }
}

void frameRx(struct s_ethernetSocket* vp_socket,struct s_frameBuffer* vp_frameBuffer)
{
  while(vp_frameBuffer->m_running)
  {
    pthread_mutex_lock(&(vp_frameBuffer->m_threadAccessing));
    frameReceive(vp_socket,vp_frameBuffer);
    pthread_mutex_unlock(&(vp_frameBuffer->m_threadAccessing));
    //usleep(0);
  }
}

void frameRxForm1stNIC(struct s_frameBuffer* vp_frameBuffer)
{
  frameRx(vp_frameBuffer->mp_1stSocket,vp_frameBuffer);
}

void frameRxForm2ndNIC(struct s_frameBuffer* vp_frameBuffer)
{
  frameRx(vp_frameBuffer->mp_2ndSocket,vp_frameBuffer);
}

void frameSend(struct s_frameBuffer* vp_frameBuffer)
{
  struct s_frameInfo* tp_buffer=NULL;
  tp_buffer=getBuff(vp_frameBuffer->m_frameInfoList,&(vp_frameBuffer->m_putBuffPos),&(vp_frameBuffer->m_getBuffPos));
  if(tp_buffer!=NULL)
  {
    if(vp_frameBuffer->m_timerCount>=(tp_buffer->m_timerCount+vp_frameBuffer->m_dealyTime))
    {
      getTime(&(tp_buffer->m_txTimeStamp));
      sendto(tp_buffer->mp_dSocket->m_rawSocket,tp_buffer->m_buff,tp_buffer->m_length,0,(struct sockaddr*)&(tp_buffer->mp_dSocket->m_socketAddress),sizeof(tp_buffer->mp_dSocket->m_socketAddress));
      printBuffInfo(vp_frameBuffer,tp_buffer,1);
    }
    else
    {
      undoBuff(&(vp_frameBuffer->m_getBuffPos));
    }
  }
}

void frameTx(struct s_frameBuffer* vp_frameBuffer)
{
  while(vp_frameBuffer->m_running)
  {
    pthread_mutex_lock(&(vp_frameBuffer->m_threadAccessing));
    frameSend(vp_frameBuffer);
    pthread_mutex_unlock(&(vp_frameBuffer->m_threadAccessing));
    //usleep(0);
  }
}

int createThread(void* vp_function,struct s_frameBuffer* vp_frameBuffer)
{
  pthread_t t_thread;
  if(pthread_create(&t_thread,NULL,vp_function,vp_frameBuffer)!=0)
  {
    perror("LINUX: Failed to create pthread -> exit");
    gp_frameBuffer->m_running=0;
    exit(1);
  }
  return(0);
}

int main(int argc, char** argv)
{
  createFrameBUffer(); 
  printf("\nusage: firstNIC secondNIC dealyTime(ms)\nexample: ./frameforwarder eth0 eth1 10\n");
  if(argc>3)
  {
    gp_frameBuffer->mp_1stSocket=createSocket(gp_frameBuffer->m_1stNicName,argv[1],NULL);
    gp_frameBuffer->mp_2ndSocket=createSocket(gp_frameBuffer->m_2ndNicName,argv[2],NULL);
    sscanf(argv[3],"%u",&(gp_frameBuffer->m_dealyTime));
  }
  else
  {
    gp_frameBuffer->mp_1stSocket=createSocket(gp_frameBuffer->m_1stNicName,DEF_1stNIC,NULL);
    gp_frameBuffer->mp_2ndSocket=createSocket(gp_frameBuffer->m_2ndNicName,DEF_2ndNIC,NULL);
    gp_frameBuffer->m_dealyTime=DEF_delayTime;
  }
  printf("\nrunning:\n\t1stNIC: %-16s 2ndNIC: %-16s delayTime: %-4dms\n\n",gp_frameBuffer->m_1stNicName,gp_frameBuffer->m_2ndNicName, gp_frameBuffer->m_dealyTime);
  sleep(2);
  pthread_mutex_init(&(gp_frameBuffer->m_threadAccessing),NULL);
  createThread(frameRxForm1stNIC,gp_frameBuffer);
  createThread(frameRxForm2ndNIC,gp_frameBuffer);
  createThread(frameTx,gp_frameBuffer);
  signal(SIGINT,sigintHandler);
  signal(SIGALRM,signalTimerHandler);  
  setTimer(DEF_timeAccurSec,DEF_timeAccurUSec);  
  while(gp_frameBuffer->m_running) 
  {
    sleep(1);
  }
  return(0);
}
