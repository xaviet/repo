//  filename  ofplib.c
//  desc      openflow protocol 1.3 library c file
//  contact   pioevh@163.com
//  date      20160809

//  g_    global value
//  td_   type define
//  m_    member value
//  p_    point value
//  def_  macro define

#ifndef __ofplibC__
#define __ofplibC__

#include "commonlib.h"
#include "global.h"
#include "ofplib.h"

int fun_ofp_echo_reply(struct td_linkedlist* v_pSocketLinkList,int v_ofpSocketFd,struct ofp_header* v_pmsg)
{
  struct ofp_header t_pofpHeader;
  t_pofpHeader.version=0x04;
  t_pofpHeader.type=OFPT_ECHO_REPLY;
  t_pofpHeader.length=swapEndian16((uint16_t)(sizeof(struct ofp_header)));;
  t_pofpHeader.xid=v_pmsg->xid;
  linkedlistPut(v_pSocketLinkList,v_ofpSocketFd,def_SocketTx,0,(void*)(&t_pofpHeader),sizeof(struct ofp_header),0,0);
  return(0);
}

int ofpSocketFinal(int v_SocketFd)
{
  if(v_SocketFd!=0)
  {
    close(v_SocketFd);
  }
  return(0);
}

int ofpSocketInit(char* v_ofpAddress,int v_ofpPort)
{
  int t_ofpSocketFd=0;
  struct sockaddr_in t_s_add;
  bzero(&t_s_add,sizeof(struct sockaddr_in));
  t_s_add.sin_family=AF_INET;
  t_s_add.sin_addr.s_addr=inet_addr(v_ofpAddress);
  t_s_add.sin_port=htons(v_ofpPort); 
  t_ofpSocketFd=socket(AF_INET,SOCK_STREAM,0);
  if(connect(t_ofpSocketFd,(struct sockaddr*)&t_s_add,sizeof(t_s_add))==0)
  {
    return(t_ofpSocketFd);
  }
  else
  {
    return(0);
  }
  return(0);  
}

int ofpSocketLinkListPut(struct td_linkedlist* v_pSocketLinkList,int v_ofpSocketFd,int v_direct,int v_function,char* v_buff,int v_len,int v_mtype,int v_stype)
{
  linkedlistPut(v_pSocketLinkList,v_ofpSocketFd,v_direct,v_function,(void*)v_buff,v_len,v_mtype,v_stype);
  return(0);  
}

int ofpMsgHeader(void* v_pMsg,int v_type,int v_length,int v_xid)
{
  ((struct ofp_header*)v_pMsg)->version=(uint8_t)OFP_VERSION13;
  ((struct ofp_header*)v_pMsg)->type=(uint8_t)v_type;
  ((struct ofp_header*)v_pMsg)->length=swapEndian16((uint16_t)(v_length));
  ((struct ofp_header*)v_pMsg)->xid=(uint32_t)v_xid;
  return(sizeof(struct ofp_header)); 
}

int ofpMsgHello(struct ofp_hello* v_pmsg,struct td_linkedlistNode* v_linklistNode)
{
  logStr("ofp Msg Hello",1);
  struct ofp_hello* t_pOfp=NULL;
  int t_len=sizeof(struct ofp_hello);
  t_pOfp=(struct ofp_hello*)malloc(sizeof(struct ofp_hello));
  t_pOfp->elements.type=swapEndian16(0x0001);
  t_pOfp->elements.length=swapEndian16(0x0008);
  t_pOfp->elements.bitmaps=swapEndian32(0x00000010);
  ofpMsgHeader(t_pOfp,OFPT_HELLO,t_len,v_pmsg->header.xid);
  ofpSocketLinkListPut(g_pSocketLinkList,v_linklistNode->m_handle,def_SocketTx,0,(void*)(t_pOfp),t_len,0,0);
  return(0);
}

int ofpMsgFeaturesRequest(struct ofp_hello* v_pmsg,struct td_linkedlistNode* v_linklistNode)
{
  logStr("ofp Msg Features Request",1);
  struct ofp_switch_features* t_pOfp=NULL;
  int t_len=sizeof(struct ofp_switch_features);
  t_pOfp=(struct ofp_switch_features*)malloc(sizeof(struct ofp_switch_features));
  t_pOfp->datapath_id=swapEndian64(((uint64_t)(g_ptnapiData.neId)&0x00000000ffffffffLL)|OFP_DATAPATHIDMASK);
  t_pOfp->n_buffers=swapEndian32(0x00000001);
  t_pOfp->n_tables=0x08;
  t_pOfp->auxiliary_id=0x0;
  t_pOfp->pad=0x0;
  t_pOfp->capabilities=swapEndian32(0x00000000);
  t_pOfp->reserved=0x0;
  ofpMsgHeader(t_pOfp,OFPT_FEATURES_REPLY,t_len,v_pmsg->header.xid);
  ofpSocketLinkListPut(g_pSocketLinkList,v_linklistNode->m_handle,def_SocketTx,0,(void*)(t_pOfp),t_len,0,0);
  return(0);
}

int ofpMPPortDescSingle(struct ofp_port* v_port,struct td_ptnapiPort* v_ptnapiPort)
{
  v_port->port_no=swapEndian32(v_ptnapiPort->portId);
  strcpy(v_port->name,v_ptnapiPort->portName);
  memcpy(v_port->hw_addr,v_ptnapiPort->portMac,6);
  v_port->config=swapEndian32(0x00000000);
  v_port->state=swapEndian32(0x00000000);
  v_port->curr=swapEndian32(0x00000820);
  v_port->curr_speed=swapEndian32(1000000);
  return(sizeof(struct ofp_port));
}

int ofpMPPortDesc(struct ofp_multipart_request* v_pmsg,struct td_linkedlistNode* v_linklistNode)
{
  logStr("ofp Msg Multipart Request(Port Desc)",1);
  struct ofp_multipart_reply* t_pOfp=NULL;
  int t_len=0;
  int t_i=0;
  t_pOfp=(struct ofp_multipart_reply*)malloc(def_stringBuff);
  memset(t_pOfp,0,def_stringBuff);
  t_len+=sizeof(struct ofp_multipart_reply);
  t_len+=ofpMPPortDescSingle((struct ofp_port*)((int)t_pOfp+t_len),&(g_ptnapiData.local));
  for(t_i=0;t_i<def_ptnapiPortMax;t_i++)
  {
    t_len+=ofpMPPortDescSingle((struct ofp_port*)((int)t_pOfp+t_len),&(g_ptnapiData.port[t_i]));
  }
  t_pOfp->type=swapEndian16((uint16_t)OFPMP_PORT_DESC);
  ofpMsgHeader(t_pOfp,OFPT_MULTIPART_REPLY,t_len,v_pmsg->header.xid);
  ofpSocketLinkListPut(g_pSocketLinkList,v_linklistNode->m_handle,def_SocketTx,0,(void*)(t_pOfp),t_len,0,0);
  return(0);
}

int ofpMsgMultipartRequest(struct ofp_multipart_request* v_pmsg,struct td_linkedlistNode* v_linklistNode)
{
  logStr("ofp Msg Multipart Request",1);
  enum ofp_multipart_type t_multipartType=0;
  t_multipartType=swapEndian16(v_pmsg->type);
  switch(t_multipartType)
  {
    def_switch(OFPMP_PORT_DESC,ofpMPPortDesc(v_pmsg,v_linklistNode));
    default:
          break;
  }
  return(0);
}
  
int ofpMsgFlowMod(struct ofp_flow_mod* v_pmsg,struct td_linkedlistNode* v_linklistNode)
{
  logStr("ofp Msg Flow Mod",1);
  return(0);
}

struct td_ptnapiPort* ofpGetPortDataFromId(uint32_t v_id)
{
  struct td_ptnapiPort* t_port=NULL;
  int t_i=0;
  for(t_i=0;t_i<def_ptnapiPortMax;t_i++)
  {
    if(g_ptnapiData.port[t_i].portId==v_id)
    {
      t_port=&(g_ptnapiData.port[t_i]);
    }
  }
  return(t_port);
}

int ofpLldpInquery(struct td_ptnapiPort* v_pPort)
{
  char* t_lldpPort=NULL;
  if((v_pPort>=&(g_ptnapiData.port[0]))&&(v_pPort<=&(g_ptnapiData.port[def_ptnapiPortMax])))
  {
    t_lldpPort=(char*)malloc(16);
    memset(t_lldpPort,0,16);
    memcpy(t_lldpPort,v_pPort->portName,16);
    logStr("ofp Lldp Inquery (portname): ",1);logStr(t_lldpPort,1);
    ofpMsgPut(def_ofpaLldpInquiry,0,0,t_lldpPort,0);
  }
  return(0);
}

int ofpMsgPacketInLldp(struct ofp_packet_out* v_pmsg,struct td_linkedlistNode* v_linklistNode)
{
  logStr("ofp Msg Packet In Lldp",1);
  struct ofp_packet_in* t_pOfpPacketIn=NULL;
  struct ofp_packet_head* t_pPacketHead=NULL;
  struct ofp_lldp_tlv* t_pLldpTlv=NULL;
  uint8_t * t_pLldpTlvValue=NULL;
  uint8_t t_len=0;
  struct td_ptnapiPort* t_pPort=NULL;
  t_pPort=ofpGetPortDataFromId(v_pmsg->actions.port);
 
  ofpLldpInquery(t_pPort);
  
  if((t_pPort==NULL)||(t_pPort->peerNeId==0)||(t_pPort->peerPortId==0))
  {
    return(0);
  }

  t_pOfpPacketIn=(struct ofp_packet_in*)malloc(def_stringBuff);
  memset(t_pOfpPacketIn,0,def_stringBuff);
  t_len+=(sizeof(struct ofp_packet_in)%8==0)?(sizeof(struct ofp_packet_in)+2):(sizeof(struct ofp_packet_in));
  t_pOfpPacketIn->buffer_id=swapEndian32(0xffffffff); 
  t_pOfpPacketIn->reason=0x01;  // OFPR_ACTION
  t_pOfpPacketIn->table_id=0x00;
  t_pOfpPacketIn->cookie=swapEndian64(0x0000000000000000LL);
  t_pOfpPacketIn->match.type=swapEndian16(0x0001);  // OFPMT_OXM
  t_pOfpPacketIn->match.length=swapEndian16(0x000c);
  t_pOfpPacketIn->match.oxm_fields[0]=0x80;
  t_pOfpPacketIn->match.oxm_fields[1]=0x00;
  t_pOfpPacketIn->match.oxm_fields[2]=0x00;
  t_pOfpPacketIn->match.oxm_fields[3]=0x04;
  t_pOfpPacketIn->match.value=swapEndian32(t_pPort->portId);
  t_pPacketHead=(void*)((int)t_pOfpPacketIn+t_len);
  t_len+=sizeof(struct ofp_packet_head);
  setMacAddress(t_pPacketHead->dstMac,0x01,0x80,0xc2,0x00,0x00,0x0e);
  memcpy(t_pPacketHead->srcMac,t_pPort->peerPortMac,6);
  t_pPacketHead->ethType=swapEndian16(0x88cc);
  
  t_pLldpTlv=(void*)((int)t_pOfpPacketIn+t_len);;
  t_len+=sizeof(struct ofp_lldp_tlv);
  t_pLldpTlv->tlvType=0x02;   // chassis
  t_pLldpTlv->tlvLen=t_len;
  t_pLldpTlvValue=(uint8_t*)((int)t_pOfpPacketIn+t_len);
  *t_pLldpTlvValue=0x07;
  t_len++;
  t_pLldpTlvValue=(uint8_t*)((int)t_pOfpPacketIn+t_len);
  sprintf((char*)t_pLldpTlvValue,"%s%016llx",OFP_DATAPATHIDPREFIX,((uint64_t)(t_pPort->peerNeId)&0x00000000ffffffffLL)|OFP_DATAPATHIDMASK);
  t_len+=strlen((char*)t_pLldpTlvValue);
  t_pLldpTlv->tlvLen=t_len-t_pLldpTlv->tlvLen;
  
  t_pLldpTlv=(void*)((int)t_pOfpPacketIn+t_len);;
  t_len+=sizeof(struct ofp_lldp_tlv);
  t_pLldpTlv->tlvType=0x04;   // port
  t_pLldpTlv->tlvLen=0x05;
  t_pLldpTlvValue=(uint8_t*)((int)t_pOfpPacketIn+t_len);
  *t_pLldpTlvValue=0x02;
  t_len++;
  t_pLldpTlvValue=(uint8_t*)((int)t_pOfpPacketIn+t_len);
  *((int*)t_pLldpTlvValue)=t_pPort->peerPortId;
  t_len+=4;
  
  t_pLldpTlv=(void*)((int)t_pOfpPacketIn+t_len);;
  t_len+=sizeof(struct ofp_lldp_tlv);
  t_pLldpTlv->tlvType=0x06;   // ttl
  t_pLldpTlv->tlvLen=0x02;
  t_pLldpTlvValue=(uint8_t*)((int)t_pOfpPacketIn+t_len);
  *t_pLldpTlvValue=0x00;
  t_len++;
  t_pLldpTlvValue=(uint8_t*)((int)t_pOfpPacketIn+t_len);
  *t_pLldpTlvValue=0x78;
  t_len+=1;
  
  t_pLldpTlv=(void*)((int)t_pOfpPacketIn+t_len);;
  t_len+=sizeof(struct ofp_lldp_tlv);
  t_pLldpTlv->tlvType=0x00;   // end
  t_pLldpTlv->tlvLen=0x00;
  
  t_len=(t_len<102)?102:t_len;
  t_pOfpPacketIn->total_len=swapEndian16(t_len-42);
  ofpMsgHeader(t_pOfpPacketIn,OFPT_PACKET_IN,t_len,0);
  ofpSocketLinkListPut(g_pSocketLinkList,v_linklistNode->m_handle,def_SocketTx,0,(void*)(t_pOfpPacketIn),t_len,0,0);
  
  return(0);
}

int ofpMsgPacketOut(struct ofp_packet_out* v_pmsg,struct td_linkedlistNode* v_linklistNode)
{
  logStr("ofp Msg Packet Out",1);
  struct ofp_packet_head* t_packetHead=NULL;
  uint16_t t_type=0;
  t_packetHead=(struct ofp_packet_head*)((int)v_pmsg+sizeof(struct ofp_packet_out));
  t_type=swapEndian16(t_packetHead->ethType);
  switch(t_type)
  {
    def_switch(OFP_ETH_TYPE_LLDP,ofpMsgPacketInLldp(v_pmsg,v_linklistNode));
    default:
          break;
  }
  return(0);
}

int ofpSocketRxProcess(struct td_linkedlistNode* v_linklistNode,void* v_p,int v_len)
{
  enum ofp_type t_ofpType=-1;
  int t_len=0;
  struct ofp_header* t_pOfpHeader=v_p;
  while(v_len>t_len)
  {
    t_ofpType=t_pOfpHeader->type;
    logStr("ofp Socket Rx Ofp Type: ",0);logInt(t_ofpType,1);
    switch(t_ofpType)
    {
      def_switch(OFPT_HELLO,ofpMsgHello((void*)t_pOfpHeader,v_linklistNode));
      def_switch(OFPT_FEATURES_REQUEST,ofpMsgFeaturesRequest((void*)t_pOfpHeader,v_linklistNode));
      def_switch(OFPT_MULTIPART_REQUEST,ofpMsgMultipartRequest((void*)t_pOfpHeader,v_linklistNode));
      def_switch(OFPT_FLOW_MOD,ofpMsgFlowMod((void*)t_pOfpHeader,v_linklistNode));
      def_switch(OFPT_PACKET_OUT,ofpMsgPacketOut((void*)t_pOfpHeader,v_linklistNode));
      default:
          break;
    }
    t_len+=(int)swapEndian16(t_pOfpHeader->length);
    t_pOfpHeader=(struct ofp_header*)((int)v_p+t_len);
  }
  return(0);
}

int ofpSocketRx(struct td_linkedlist* v_pSocketLinkList,int v_ofpSocketFd)
{
  ofpSocketLinkListPut(v_pSocketLinkList,v_ofpSocketFd,def_SocketRx,(int)ofpSocketRxProcess,NULL,0,0,0);
  return(0);  
}

#endif
