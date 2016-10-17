//  filename  ptnapilib.c
//  desc      ptn api library c file
//  contact   pioevh@163.com
//  date      20160809

//  g_    global value
//  td_   type define
//  m_    member value
//  p_    point value
//  def_  macro define

#ifndef __ptnapilibC__
#define __ptnapilibC__

#include "commonlib.h"
#include "global.h"

#define def_ptnapiPrompt "# "
#define def_ptnapiLogin "User"
#define def_ptnapiAuth "Password"
#define def_ptnapiUser "admin\n"
#define def_ptnapiPassword "cMPC_pxn\n"
#define def_ptnapiData "show / all\n"
#define def_ptnapiPort "ge.1.%d"
#define def_ptnapiPortData "show interfaces/eth/%s\n"
#define def_ptnapiLldpNeighbour "call interfaces/eth/%s/lldp neighbour\n"
#define def_ptnapiLldpNeighbourNone "false"
#define def_ptnapiLldpNeighbourExist "Chassis"

#define def_portNoRange(v_portNo) (v_portNo>def_ptnapiPortMax)||(v_portNo<=0)

char g_defaultCfg[][def_msgBuff]={"option page off\n",
  "secu\n", 
  "idletime admin 604800\n",
  "exit\n",
  "atomconfig\n",
//  "dropdb\n",
//  "set slot/1/lg type XCSB\n",
  "mset interfaces/eth ge.1.1 {admin='up',role='uni'}\n",
  "mset interfaces/eth ge.1.2 {admin='up',role='uni'}\n",
  "mset interfaces/eth ge.1.3 {admin='up',role='uni'}\n",
  "mset interfaces/eth ge.1.4 {admin='up',role='uni'}\n",
  "mset interfaces/eth ge.1.5 {admin='up',role='uni'}\n",
  "mset interfaces/eth ge.1.6 {admin='up',role='uni'}\n",
  "mset interfaces/eth ge.1.7 {admin='up',role='uni'}\n",
  "mset interfaces/eth ge.1.8 {admin='up',role='uni'}\n",
  "mset interfaces/eth ge.1.9 {admin='up',role='uni'}\n",
  "mset interfaces/eth ge.1.10 {admin='up',role='uni'}\n",
  "mset interfaces/eth ge.1.11 {admin='up',role='uni'}\n",
  "mset interfaces/eth ge.1.12 {admin='up',role='uni'}\n",
  "mset interfaces/eth ge.1.13 {admin='up',role='uni'}\n",
  "mset interfaces/eth ge.1.14 {admin='up',role='uni'}\n",
  "mset interfaces/eth ge.1.15 {admin='up',role='uni'}\n",
  "mset interfaces/eth ge.1.16 {admin='up',role='uni'}\n",
  "create interfaces/eth/ge.1.1 lldp\n",
  "create interfaces/eth/ge.1.2 lldp\n",
  "create interfaces/eth/ge.1.3 lldp\n",
  "create interfaces/eth/ge.1.4 lldp\n",
  "create interfaces/eth/ge.1.5 lldp\n",
  "create interfaces/eth/ge.1.6 lldp\n",
  "create interfaces/eth/ge.1.7 lldp\n",
  "create interfaces/eth/ge.1.8 lldp\n",
  "create interfaces/eth/ge.1.9 lldp\n",
  "create interfaces/eth/ge.1.10 lldp\n",
  "create interfaces/eth/ge.1.11 lldp\n",
  "create interfaces/eth/ge.1.12 lldp\n",
  "create interfaces/eth/ge.1.13 lldp\n",
  "create interfaces/eth/ge.1.14 lldp\n",
  "create interfaces/eth/ge.1.15 lldp\n",
  "create interfaces/eth/ge.1.16 lldp\n"
};

int ptnapiSocketFinal(int v_ptnapiSocketFd)
{
  if(v_ptnapiSocketFd!=0)
  {
    close(v_ptnapiSocketFd);
  }
  return(0);
}

int ptnapiSocketInit(char* v_ptnapiAddress,int v_ptnapiPort)
{
  int t_ptnapiSocketFd=0;
  struct sockaddr_in t_s_add;
  bzero(&t_s_add,sizeof(struct sockaddr_in));
  t_s_add.sin_family=AF_INET;
  t_s_add.sin_addr.s_addr=inet_addr(v_ptnapiAddress);
  t_s_add.sin_port=htons(v_ptnapiPort); 
  t_ptnapiSocketFd=socket(AF_INET,SOCK_STREAM,0);
  if(connect(t_ptnapiSocketFd,(struct sockaddr*)&t_s_add,sizeof(t_s_add))==0)
  {
    return(t_ptnapiSocketFd);
  }
  else
  {
    return(0);
  }
  return(0);  
}

int ptnapiLoginProcess(struct td_linkedlistNode* v_linklistNode,void* v_p,int v_len)
{
  return((int)(strstr((char*)v_p,(char*)(v_linklistNode->m_pbuff))));  
}

int ptnapiSocketLinkListPut(struct td_linkedlist* v_pSocketLinkList,int v_ptnapiSocketFd,char* v_str,int v_direct,int v_function)
{
  char* t_pBuff=NULL;
  t_pBuff=(char*)malloc(def_msgBuff);
  strcpy(t_pBuff,v_str);
  linkedlistPut(v_pSocketLinkList,v_ptnapiSocketFd,v_direct,v_function,(void*)t_pBuff,strlen(t_pBuff),1,0);
  return(0);  
}

int ptnapiSocketSessionCfg(struct td_linkedlist* v_pSocketLinkList,int v_ptnapiSocketFd,char* v_rxStr,char* v_txStr,int v_function)
{
  ptnapiSocketLinkListPut(v_pSocketLinkList,v_ptnapiSocketFd,v_rxStr,def_SocketRx,v_function);
  ptnapiSocketLinkListPut(v_pSocketLinkList,v_ptnapiSocketFd,v_txStr,def_SocketTx,0);
  return(0);  
}

int ptnapiSocketSessionInquery(struct td_linkedlist* v_pSocketLinkList,int v_ptnapiSocketFd,char* v_rxStr,char* v_txStr,int v_function)
{
  ptnapiSocketLinkListPut(v_pSocketLinkList,v_ptnapiSocketFd,v_txStr,def_SocketTx,0);
  ptnapiSocketLinkListPut(v_pSocketLinkList,v_ptnapiSocketFd,v_rxStr,def_SocketRx,v_function);
  return(0);  
}

int ptnapiSocketLogin(struct td_linkedlist* v_pSocketLinkList,int v_ptnapiSocketFd)
{
  //td_callbackLinklistNode t_callbackLinklistNode=ptnapiLoginProcess;
  ptnapiSocketSessionCfg(v_pSocketLinkList,v_ptnapiSocketFd,def_ptnapiLogin,def_ptnapiUser,(int)ptnapiLoginProcess);
  ptnapiSocketSessionCfg(v_pSocketLinkList,v_ptnapiSocketFd,def_ptnapiAuth,def_ptnapiPassword,(int)ptnapiLoginProcess);
  return(0);
}

int ptnapiDefaultCfgProcess(struct td_linkedlistNode* v_linklistNode,void* v_p)
{
  return((int)(strstr((char*)v_p,(char*)(v_linklistNode->m_pbuff))));  
}

int ptnapiData(struct td_linkedlistNode* v_linklistNode,void* v_p)
{
  char t_strValue[def_msgBuff]={0};
  char* t_strRegex=NULL;
  int t_iMac[6]={0};
  if((int)(strstr((char*)v_p,(char*)(v_linklistNode->m_pbuff))))
  {
    logStr("ptnapi Data got",1);
    t_strRegex=regexGetLine((char*)v_p,"[^e]id\\s*:\\s*\\S*");
    if(t_strRegex!=0)
    {
      sscanf(t_strRegex,"%*s%s",t_strValue);
      free(t_strRegex);
      g_ptnapiData.neId=swapEndian32(inet_addr(t_strValue));
    }
    t_strRegex=regexGetLine((char*)v_p,"sysmac\\s*:\\s*\\S*");
    if(t_strRegex!=0)
    {
      sscanf(t_strRegex,"sysmac%*s%2x%2x.%2x%2x.%2x%2x",&(t_iMac[0]),&(t_iMac[1]),&(t_iMac[2]),&(t_iMac[3]),&(t_iMac[4]),&(t_iMac[5]));
      free(t_strRegex);
      mac6CharsFrom6Ints(g_ptnapiData.local.portMac,t_iMac);
      g_ptnapiData.local.portId=swapEndian32(def_ptnapiLocalPortId);
      strcpy(g_ptnapiData.local.portName,"lo");
    }
  }
  else
  {
    return(0);  
  }
  return(1);  
}

int ptnapiPortData(struct td_linkedlistNode* v_linklistNode,void* v_p)
{
  char t_strValue[def_msgBuff]={0};
  int t_intValue=0;
  int t_portNo=0;
  char* t_strRegex=NULL;
  int t_iMac[6]={0};
  if((int)(strstr((char*)v_p,(char*)(v_linklistNode->m_pbuff))))
  {
    t_strRegex=regexGetLine((char*)v_p,"ifname\\s*:\\s*\\S*");
    if(t_strRegex!=0)
    {
      sscanf(t_strRegex,"%*s%s",t_strValue);
      sscanf(t_strValue,"ge.%*d.%d",&t_portNo);
      free(t_strRegex);
      strcpy(g_ptnapiData.port[t_portNo-1].portName,t_strValue);
      logStr("ptnapi Port Data got Number: ",0);logInt(t_portNo,1);
      if(def_portNoRange(t_portNo))
      {
        logStr("ptnapi Port Number out!!!",1);
        return(1);
      }
      t_strRegex=regexGetLine((char*)v_p,"ifindex\\s*:\\s*\\S*");
      if(t_strRegex!=0)
      {
        sscanf(t_strRegex,"%*s%d",&t_intValue);
        free(t_strRegex);
        g_ptnapiData.port[t_portNo-1].portId=swapEndian32(t_intValue);
        logStr("ptnapi Port Data got ID: ",0);logInt(t_intValue,1);
      }
      t_strRegex=regexGetLine((char*)v_p,"mac\\s*:\\s*\\S*");
      if(t_strRegex!=0)
      {
        sscanf(t_strRegex,"mac%*s%2x%2x.%2x%2x.%2x%2x",&(t_iMac[0]),&(t_iMac[1]),&(t_iMac[2]),&(t_iMac[3]),&(t_iMac[4]),&(t_iMac[5]));
        free(t_strRegex);
        mac6CharsFrom6Ints(g_ptnapiData.port[t_portNo-1].portMac,t_iMac);
      }
    }
    else
    {
      logStr("ptnapi Port Data got fault",1);
    }
  }
  else
  {
    return(0);
  }
  return(1);  
}

int ptnapiPortDataInquery(struct td_linkedlist* v_pSocketLinkList,int v_ptnapiSocketFd,int v_portNo,int v_function)
{
  char t_tempChars[def_msgBuff]={0};
  char* t_tempStr=NULL;
  t_tempStr=(char*)malloc(def_msgBuff);
  sprintf(t_tempChars,def_ptnapiPort,v_portNo);
  sprintf(t_tempStr,def_ptnapiPortData,t_tempChars);
  ptnapiSocketSessionInquery(v_pSocketLinkList,v_ptnapiSocketFd,def_ptnapiPrompt,t_tempStr,v_function);  
  return(0);  
}

int ptnapiPortDataAllInquery(struct td_linkedlist* v_pSocketLinkList,int v_ptnapiSocketFd,const int v_portMax)
{
  int t_i=0;  
  for(t_i=1;t_i<=v_portMax;t_i++)
  {
    ptnapiPortDataInquery(v_pSocketLinkList,v_ptnapiSocketFd,t_i,(int)ptnapiPortData);
  }
  return(0);  
}

int ptnapiLldpData(struct td_linkedlistNode* v_linklistNode,void* v_p)
{
  char t_strValue[def_msgBuff]={0};
  int t_intValue=0;
  int t_portNo=0;
  char* t_strRegex=NULL;
  int t_iMac[6]={0};  
  int t_intNeighbourNone=0;
  t_intNeighbourNone=(int)(strstr((char*)v_p,def_ptnapiLldpNeighbourNone));
  if(t_intNeighbourNone||(int)(strstr((char*)v_p,def_ptnapiLldpNeighbourExist)))
  {
    t_strRegex=regexGetLine((char*)v_p,"eth/ge\\.[0-9][0-9]*\\.[0-9][0-9]*");
    if(t_strRegex!=0)
    {
      sscanf(t_strRegex,"eth/ge.%*d.%d",&t_portNo);
      free(t_strRegex);
      logStr("ptnapi Port Lldp Port Number: ",0);logInt(t_portNo,1);
      if(def_portNoRange(t_portNo))
      {
        logStr("ptnapi Port Number out!!!",1);
        return(1);
      }
      g_ptnapiData.port[t_portNo-1].peerNeId=0;
      g_ptnapiData.port[t_portNo-1].peerPortId=0;
      memset(&(g_ptnapiData.port[t_portNo-1].peerPortMac),0,6);
      memset(&(g_ptnapiData.port[t_portNo-1].peerPortName),0,16);
      if(t_intNeighbourNone)
      {
        logStr("ptnapi this Port Lldp is not exist neighbour",1);
        return(1);
      }
      else
      {
        t_strRegex=regexGetLine((char*)v_p,"MAC\\s*str:\\s*\\S*");
        if(t_strRegex!=0)
        {
          sscanf(t_strRegex,"%*s%*4s%2x%2x.%2x%2x.%2x%2x",&(t_iMac[0]),&(t_iMac[1]),&(t_iMac[2]),&(t_iMac[3]),&(t_iMac[4]),&(t_iMac[5]));
          free(t_strRegex);
          mac6CharsFrom6Ints(g_ptnapiData.port[t_portNo-1].peerPortMac,t_iMac);
          t_strRegex=regexGetLine((char*)v_p,"IPV4:\\S*");
          if(t_strRegex!=0)
          {
            sscanf(t_strRegex,"IPV4:%s",t_strValue);
            g_ptnapiData.port[t_portNo-1].peerNeId=swapEndian32(inet_addr(t_strValue));
            free(t_strRegex);
          }
          else
          {
            logStr("ptnapi this Port Lldp peer NeID error!!!",1);
          }        
          t_strRegex=regexGetLine((char*)v_p,"NAME\\s\\s*str:\\S*");
          if(t_strRegex!=0)
          {
            sscanf(t_strRegex,"%*s%*4s%s",t_strValue);
            strcpy(g_ptnapiData.port[t_portNo-1].peerPortName,t_strValue);
            free(t_strRegex);
          }
          else
          {
            logStr("ptnapi this Port Lldp peer Port Name error!!!",1);
          }
          
          t_strRegex=regexGetLine((char*)v_p,"value\\s\\s*\\S*");
          if(t_strRegex!=0)
          {
            sscanf(t_strRegex,"%*s%*2s%x",&t_intValue);
            g_ptnapiData.port[t_portNo-1].peerPortId=swapEndian32(t_intValue);
            free(t_strRegex);
          }
          else
          {
            logStr("ptnapi this Port Lldp peer Port ID error!!!",1);
          }
        } 
        else
        {
          return(0);
        }
      }  
    }
    else
    {
      logStr("ptnapi Lldp Port Number got fault",1);
      return(1);  
    }
  }
  else
  {
    return(0);
  }
  return(1);  
}

int ptnapiLldpInquery(struct td_linkedlist* v_pSocketLinkList,int v_ptnapiSocketFd,int v_portNo,int v_function)
{
  char t_tempChars[def_msgBuff]={0};
  char* t_tempStr=NULL;
  t_tempStr=(char*)malloc(def_msgBuff);
  sprintf(t_tempChars,def_ptnapiPort,v_portNo);
  sprintf(t_tempStr,def_ptnapiLldpNeighbour,t_tempChars);
  ptnapiSocketSessionInquery(v_pSocketLinkList,v_ptnapiSocketFd,def_ptnapiPrompt,t_tempStr,v_function);  
  return(0);  
}

int ptnapiLldpAllInquery(struct td_linkedlist* v_pSocketLinkList,int v_ptnapiSocketFd,const int v_portMax)
{
  int t_i=0;  
  for(t_i=1;t_i<=v_portMax;t_i++)
  {
    ptnapiLldpInquery(v_pSocketLinkList,v_ptnapiSocketFd,t_i,(int)ptnapiLldpData);
  } 
  return(0);  
}

int ptnapiLldpSignleInquery(char* v_portName)
{
  char* t_tempStr=NULL;
  t_tempStr=(char*)malloc(def_msgBuff);
  sprintf(t_tempStr,def_ptnapiLldpNeighbour,v_portName);
  logStr("ptnapi Lldp Signle Inquery (command):",1);logStr(t_tempStr,1);
  ptnapiSocketSessionInquery(g_pSocketLinkList,g_ptnapiSocketFd,def_ptnapiPrompt,t_tempStr,(int)ptnapiLldpData); 
  pause(); 
  return(0);  
}

int ptnapiSocketOptCodeDefaultCfg(struct td_linkedlist* v_pSocketLinkList,int v_ptnapiSocketFd)
{
  //td_callbackLinklistNode t_callbackLinklistNode=ptnapiDefaultCfgProcess;
  int t_i=0;
  int t_len=sizeof(g_defaultCfg)/def_msgBuff;
  for(t_i=0;t_i<t_len;t_i++)
  {
    ptnapiSocketSessionCfg(v_pSocketLinkList,v_ptnapiSocketFd,def_ptnapiPrompt,g_defaultCfg[t_i],(int)ptnapiDefaultCfgProcess);
  }
  // delete the last prompt
  ptnapiSocketLinkListPut(v_pSocketLinkList,v_ptnapiSocketFd,def_ptnapiPrompt,def_SocketRx,(int)ptnapiDefaultCfgProcess);
  // get ptnapi data
  ptnapiSocketSessionInquery(v_pSocketLinkList,v_ptnapiSocketFd,def_ptnapiPrompt,def_ptnapiData,(int)ptnapiData);
  // get ptnapi port data
  ptnapiPortDataAllInquery(v_pSocketLinkList,v_ptnapiSocketFd,def_ptnapiPortMax);
  // get ptnapi port lldp data
  ptnapiLldpAllInquery(v_pSocketLinkList,v_ptnapiSocketFd,def_ptnapiPortMax);
  return(0);
}

int ptnapiMock()
{
  g_ptnapiData.neId=swapEndian32(inet_addr("127.0.0.1"));
  return(0);
}

#endif
