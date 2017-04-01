/*
 *  publisher.c
 *  by Pioevh@163.com 20170317
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

#define DEF_NIC "eno33554984"
#define DEF_macAddrLen 6
#define DEF_maxFrameLen 1522

#define DEF_timeAccurSec 0
#define DEF_timeAccurUSec 1

#define DEF_svDefaultDMac {0x01,0x0c,0xcd,0x04,0x00,0x01}
#define DEF_svDefaultPriority 4
#define DEF_svDefaultVlanId 1
#define DEF_svDefaultAppid 0x4001

#define DEF_gooseDefaultDMac {0x01,0x0c,0xcd,0x01,0x00,0x01}
#define DEF_gooseDefaultPriority 4
#define DEF_gooseDefaultVlanId 1
#define DEF_gooseDefaultAppid 0x0001

//  struct

struct s_ethernetSocket 
{
  int m_rawSocket;
  int m_isBind;
  struct sockaddr_ll m_socketAddress;
  char m_interfaceMacAddr[DEF_macAddrLen];
};

struct s_svPublisher 
{
  char* mp_buffer;
  uint16_t m_appId;
  char m_dAddr[DEF_macAddrLen];
  char m_sAddr[DEF_macAddrLen]; 
  char m_priority;
  uint16_t m_vlanId;
  int m_lengthField;
  int m_payloadStart;
  char* m_id;
  char* m_dataSetRef; /* date set reference */
  uint16_t m_smpCount; /* sample counter - reset by sync */
  uint32_t m_confRev; /* Configuration revision according to CB */
  char m_smpSynch; /* Synchronization status */
  uint16_t m_smpRate;
  uint64_t m_refreshTime; /* local buffer refresh time */
  struct timeval m_timestamp;
  int m_hasDataSetName; /* optional fields in sv asdu */
  int m_hasRefreshTime;
  int m_hasSampleRate;
  struct s_appData* mp_appData;
};

struct s_goosePublisher 
{
  char* mp_buffer;
  uint16_t m_appId;
  char m_dAddr[DEF_macAddrLen];
  char m_sAddr[DEF_macAddrLen]; 
  char m_priority;
  uint16_t m_vlanId;
  int m_lengthField;
  int m_payloadStart;
  char* m_goID;
  char* m_goCBRef;
  char* m_dataSetRef;
  uint16_t m_minTime;
  uint16_t m_maxTime;
  int m_fixedOffs;
  uint32_t m_confRev;
  uint32_t m_stNum;
  uint32_t m_sqNum;
  uint32_t m_timeAllowedToLive;
  int m_needsCommission;
  int m_simulation;
  struct timeval m_timestamp;
  struct s_appData* mp_appData;
};

struct s_threadData 
{
  int m_threadId;
  struct s_threadData* mp_next;
};

struct s_appData 
{
  int m_running;
  long long m_timerCount;
  struct s_ethernetSocket* mp_socket;
  struct s_threadData* mp_threadHead;
};

//  global

static struct s_appData* gp_appData=NULL;

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
    ++gp_appData->m_timerCount;  
}

void sigintHandler(int v_signalId)
{
    gp_appData->m_running=0;
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

struct s_ethernetSocket* createSocket(char* v_nic,char* v_dAddr)
{
  struct s_ethernetSocket* t_socket=calloc(1,sizeof(struct s_ethernetSocket));
  t_socket->m_rawSocket=socket(AF_PACKET,SOCK_RAW,htons(ETH_P_ALL));
  if (t_socket->m_rawSocket==-1)
  {
    perror("LINUX: Failed to creating raw socket -> exit");
    free(t_socket);
    exit(1);
  }
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

void getInterfaceMACAddress(char* v_interfaceId,char* v_addr)
{
  struct ifreq t_buffer;
  int t_sock=socket(PF_INET,SOCK_DGRAM,0);
  memset(&t_buffer,0x00,sizeof(t_buffer));
  strcpy(t_buffer.ifr_name,v_interfaceId);
  ioctl(t_sock,SIOCGIFHWADDR,&t_buffer);
  close(t_sock);
  int i;
  for(i=0;i<6;i++)
  {
    v_addr[i]=(unsigned char)t_buffer.ifr_hwaddr.sa_data[i];
  }
}

void createAppData(char* v_nic)
{
  gp_appData=(struct s_appData*)malloc(sizeof(struct s_appData));
  memset(gp_appData,0,sizeof(struct s_appData));
  gp_appData->m_running=1;
  gp_appData->m_timerCount=0;
  gp_appData->mp_socket=createSocket(v_nic,NULL);
  gp_appData->mp_threadHead=NULL;
  getInterfaceMACAddress(v_nic,gp_appData->mp_socket->m_interfaceMacAddr);
}

void createSignal()
{
  signal(SIGINT,sigintHandler);
  signal(SIGALRM,signalTimerHandler);  
  setTimer(DEF_timeAccurSec,DEF_timeAccurUSec);  
}

void prepareGooseBuffer(struct s_goosePublisher* vp_gooseData)
{
  char t_mac[DEF_macAddrLen]=DEF_gooseDefaultDMac;
  memcpy(vp_gooseData->m_dAddr,t_mac,6);
  memcpy(vp_gooseData->m_sAddr,vp_gooseData->mp_appData->mp_socket->m_interfaceMacAddr,6);
  vp_gooseData->m_priority=DEF_gooseDefaultPriority;
  vp_gooseData->m_vlanId=DEF_gooseDefaultVlanId;
  vp_gooseData->m_appId=DEF_gooseDefaultAppid;  
  vp_gooseData->mp_buffer=(char*)malloc(DEF_maxFrameLen);
  memset(vp_gooseData->mp_buffer,0,DEF_maxFrameLen);
  memcpy(vp_gooseData->mp_buffer,vp_gooseData->m_dAddr,6);
  memcpy(vp_gooseData->mp_buffer+6,vp_gooseData->m_sAddr,6);
  int bufPos=12;
  /* Priority tag - IEEE 802.1Q */
  vp_gooseData->mp_buffer[bufPos++]=0x81;
  vp_gooseData->mp_buffer[bufPos++]=0x00;
  vp_gooseData->mp_buffer[bufPos++]=(vp_gooseData->m_priority<<5)+(vp_gooseData->m_vlanId/256); /* Priority + VLAN-ID */
  vp_gooseData->mp_buffer[bufPos++]=vp_gooseData->m_vlanId%256; /* VLAN-ID */
  /* EtherType GOOSE */
  vp_gooseData->mp_buffer[bufPos++]=0x88;
  vp_gooseData->mp_buffer[bufPos++]=0xB8;
  vp_gooseData->mp_buffer[bufPos++]=DEF_gooseDefaultAppid/256;
  vp_gooseData->mp_buffer[bufPos++]=DEF_gooseDefaultAppid%256;
  vp_gooseData->m_lengthField=bufPos;
  vp_gooseData->mp_buffer[bufPos++]=0x00;
  vp_gooseData->mp_buffer[bufPos++]=0x08;
  vp_gooseData->mp_buffer[bufPos++]=0x00;
  vp_gooseData->mp_buffer[bufPos++]=0x00;
  vp_gooseData->mp_buffer[bufPos++]=0x00;
  vp_gooseData->mp_buffer[bufPos++]=0x00;
  vp_gooseData->m_payloadStart=bufPos;
  memDisp(vp_gooseData->mp_buffer,DEF_maxFrameLen);
}

int createThread(void* vp_function,struct s_appData* vp_appData)
{
  pthread_t t_thread;
  if(pthread_create(&t_thread,NULL,vp_function,vp_appData)!=0)
  {
    perror("LINUX: Failed to create pthread -> exit");
  }
  return(0);
}

int main(int argc,char** argv)
{
  createAppData(DEF_NIC);
  createSignal();
  struct s_goosePublisher vp_gooseData={0}; 
  vp_gooseData.mp_appData=gp_appData; 
  prepareGooseBuffer(&vp_gooseData);
  while(gp_appData->m_running) 
  {
    sleep(1);
  }
  return(0);
}
