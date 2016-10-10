//  filename  ofplib.h
//  desc      openflow protocol 1.3 library h file
//  contact   pioevh@163.com
//  date      20160809

//  g_    global value
//  td_   type define
//  m_    member value
//  p_    point value
//  def_  macro define

#ifndef __ofplibH__
#define __ofplibH__

#include "commonlib.h"
#include "linkedlistlib.h"
#include "stdio.h"
#include "stdlib.h"
#include "sys/socket.h"
#include "netinet/in.h"
#include "sys/types.h"
#include "string.h"
#include "sys/time.h"
#include "unistd.h"
#include "arpa/inet.h"

#define OFP_ETH_ALEN 6
#define OFP_MAX_PORT_NAME_LEN 16
#define SERIAL_NUM_LEN 32
#define DESC_STR_LEN 256

#define OFP_VERSION13 0x04

#define OFP_ETH_TYPE_LLDP 0x88cc

enum ofp_type
{
  /* 不可改变的消息 */
  OFPT_HELLO=0, /* 对称消息 */
  OFPT_ERROR=1, /* 对称消息 */
  OFPT_ECHO_REQUEST=2, /* 对称消息 */
  OFPT_ECHO_REPLY=3, /* 对称消息 */
  OFPT_EXPERIMENTER=4, /* 对称消息 */
  /* 交换机配置消息 */
  OFPT_FEATURES_REQUEST=5, /* 控制器/交换机消息 */
  OFPT_FEATURES_REPLY=6, /* 控制器/交换机消息 */
  OFPT_GET_CONFIG_REQUEST=7, /* 控制器/交换机消息 */
  OFPT_GET_CONFIG_REPLY=8, /* 控制器/交换机消息 */
  OFPT_SET_CONFIG=9, /* 控制器/交换机消息 */
  /* 异步消息 */
  OFPT_PACKET_IN=10, /* 异步消息 */
  OFPT_FLOW_REMOVED=11, /* 异步消息 */
  OFPT_PORT_STATUS=12, /* 异步消息 */
  /* 控制器命令消息 */
  OFPT_PACKET_OUT=13, /* 控制器/交换机消息 */
  OFPT_FLOW_MOD=14, /* 控制器/交换机消息 */
  OFPT_GROUP_MOD=15, /* 控制器/交换机消息 */
  OFPT_PORT_MOD=16, /* 控制器/交换机消息 */
  OFPT_TABLE_MOD=17, /* 控制器/交换机消息 */
  /* 复合消息 */
  OFPT_MULTIPART_REQUEST=18, /* 控制器/交换机消息 */
  OFPT_MULTIPART_REPLY=19, /* 控制器/交换机消息 */
  /* 屏障消息（见 6.1.1） */
  OFPT_BARRIER_REQUEST=20, /* 控制器/交换机消息 */
  OFPT_BARRIER_REPLY=21, /* 控制器/交换机消息 */
  /* 队列配置消息. */
  OFPT_QUEUE_GET_CONFIG_REQUEST=22,/* 控制器/交换机消息*/
  OFPT_QUEUE_GET_CONFIG_REPLY=23, /*控制器/交换机消息*/
  /* 控制器角色改变请求消息 */
  OFPT_ROLE_REQUEST=24, /* 控制器/交换机消息 */
  OFPT_ROLE_REPLY=25, /* 控制器/交换机消息 */
  /* 异步消息配置 */
  OFPT_GET_ASYNC_REQUEST=26, /* 控制器/交换机消息 */
  OFPT_GET_ASYNC_REPLY=27, /* 控制器/交换机消息 */
  OFPT_SET_ASYNC=28, /* 控制器/交换机消息 */
  /* 测量器和速率限制器配置消息 */
  OFPT_METER_MOD=29, /* 控制器/交换机消息 */
};

enum ofp_multipart_type
{
  /* 描述这个 OpenFlow 交换机。*请求 body 是空的。*回复主体是 struct ofp_desc. */
  OFPMP_DESC=0,
  /* 单独流统计。 * 请求 body 是 ofp_flow_stats_request 结构。 *回复 body 是一个 structofp_flow_stats 数组。. */
  OFPMP_FLOW=1,
  /*总的流统计。* 请求 body 是 ofp_aggregate_stats_request 结构。*回复 body 是ofp_aggregate_stats_reply 结构. */
  OFPMP_AGGREGATE=2,
  /* 流表统计。*请求 body 是空的。*回复 body 是 ofp_table_stats 结构数组。 */
  OFPMP_TABLE=3,
  /* 端 口 统 计 。 * 请 求 body 是 ofp_port_stats_request 结 构 。 * 回 复 body 是ofp_port_stats 结构数组。 */
  OFPMP_PORT_STATS=4,
  /*一个端口的队列统计*请求的 body 是 ofp_queue_stats_request 结构体.*回复 body 是 ofp_queue_stats 结构体数组*/
  OFPMP_QUEUE=5,
  /*组计数器统计。*请求的 body 是 ofp_group_stats_request 结构体.*回复是 ofp_group_stats 结构体数组*/
  OFPMP_GROUP=6,
  /*组描述。*请求 body 是空的。回复 body 是 ofp_group_desc 结构体数组*/
  OFPMP_GROUP_DESC=7,
  /*组特征。*请求 body 为空。*回复 body 是 ofp_group_features 结构体。*/
  OFPMP_GROUP_FEATURES=8,
  /*计量器统计*请求 body 是 ofp_meter_multipart_requests 结构体.*回复 body 是 ofp_meter_stats 结构体数组*/
  OFPMP_METER=9,
  /*计量器配置*请求 body 是 ofp_meter_multipart_requests 结构体.*回复 body 是 ofp_meter_config 结构体数组.*/
  OFPMP_METER_CONFIG=10,
  /*计量器特征*请求 body 为空。*回复 body 是 ofp_meter_features 结构体。*/
  OFPMP_METER_FEATURES=11,
  /*表特征*请求 body 是空的或包含 ofp_table_features 结构体数组， 包含控制器所需的交换机视图。如果交换机不能设置特定的视图，就会返回一个 error.*回复 body 是一组 ofp_table_features 结构体.*/
  OFPMP_TABLE_FEATURES=12,
  /*端口说明*请求 body 是空的。*回复 body 是 ofp_port 结构体数组*/
  OFPMP_PORT_DESC=13,
  /*实验者扩展项*请求 body 和回复 body 都以 ofp_experimenter_multipart_header 结构体开始。*而请求 body 和回复 body 是由实验者定义的。*/
  OFPMP_EXPERIMENTER=0xffff
};

enum ofp_action_type
{
  OFPAT_OUTPUT=0, /*输出到交换机端口*/
  OFPAT_COPY_TTL_OUT=11, /*申请复制 TTL outwards 去打包，从相邻外层到最外层*/
  OFPAT_COPY_TTL_IN=12, /*申请复制 TTL inwards 打包，从最外层到相邻外层*/
  OFPAT_SET_MPLS_TTL=15, /* MPLS TTL*/
  OFPAT_DEC_MPLS_TTL=16,/*减少 MPLS TTL*/
  OFPAT_PUSH_VLAN=17, /* 压入一个新的 VLAN 标签*/
  OFPAT_POP_VLAN=18, /* 弹出最外面的 VLAN 标签*/
  OFPAT_PUSH_MPLS=19, /*压入一个新的 MPLS 标签*/
  OFPAT_POP_MPLS=20, /* 弹出最外面的 MPLS 标签 */
  OFPAT_SET_QUEUE=21, /* 设置输出端口一个队列的 ID*/
  OFPAT_GROUP=22, /*申请组 */
  OFPAT_SET_NW_TTL=23, /* IP TTL. */
  OFPAT_DEC_NW_TTL=24, /*减少 IP TTL. */
  OFPAT_SET_FIELD=25, /* 用 OXM TLV 格式设置一个头部字段*/
  OFPAT_PUSH_PBB=26, /* 压入一个新的 PBB 服务标签 (I-TAG) */
  OFPAT_POP_PBB=27, /* 弹出外面的 PBB 服务标签 (I-TAG) */
  OFPAT_EXPERIMENTER=0xffff
};

struct ofp_header
{
  uint8_t version; /* OFP_VERSION. */
  uint8_t type; /* One of the OFPT_ constants. */
  uint16_t length; /* Length including this ofp_header. */
  uint32_t xid; /* Transaction id associated with this packet.Replies use the same id as was in the request to facilitate pairing. */
}__attribute((aligned(1)));

struct ofp_hello_elem_header
{
  uint16_t type; /* One of OFPHET_*. */
  uint16_t length; /* Length in bytes of this element. */
  uint32_t bitmaps; /* List of bitmaps - supported versions */
}__attribute((aligned(1)));

struct ofp_hello
{
  struct ofp_header header;
  /* Hello element list */
  struct ofp_hello_elem_header elements; /* List of elements - 0 or more */
}__attribute((aligned(1)));

struct ofp_switch_features
{
  struct ofp_header header;
  uint64_t datapath_id; /* 数据通路唯一的 ID。低 48-bits 是 MAC 地址,高 16 位是开发者定义。 */
  uint32_t n_buffers; /*一次缓冲最大的数据包数。 */
  uint8_t n_tables; /* 数据通路支持的表数量。 */
  uint8_t auxiliary_id; /*标识辅助连接*/
  uint16_t pad; /* 64 位对齐. */
  /* 功能 */
  uint32_t capabilities; /* 位图的支持"ofp_capabilities". */
  uint32_t reserved;
}__attribute((aligned(1)));

struct ofp_switch_config
{
  struct ofp_header header;
  uint16_t flags; /* OFPC_* flags 位图. */
  uint16_t miss_send_len; /* 数据 通路 应该 发送 给控 制器 的数 据包 最大 字节 数。见ofp_controller_max_len 有效值*/
}__attribute((aligned(1)));

struct ofp_multipart_request
{
  struct ofp_header header;
  uint16_t type; /* OFPMP_* 之一. */
  uint16_t flags; /* OFPMPF_REQ_* 标记. */
  uint8_t pad[4];
  //uint8_t body[0]; /*请求的 body。0 或多个字节. */
}__attribute((aligned(1)));

struct ofp_multipart_reply
{
  struct ofp_header header;
  uint16_t type; /* 一个 OFPMP_*常数. */
  uint16_t flags; /* OFPMPF_REPLY_* 标记. */
  uint8_t pad[4];
  //uint8_t body[0]; /* 回答的 body，0 或更多字节. */
}__attribute((aligned(1)));

struct ofp_port
{
  uint32_t port_no;
  uint8_t pad[4];
  uint8_t hw_addr[OFP_ETH_ALEN];
  uint8_t pad2[2]; /* Align to 64 bits. */
  char name[OFP_MAX_PORT_NAME_LEN]; /* Null-terminated */
  uint32_t config; /* Bitmap of OFPPC_* flags. */
  uint32_t state; /* Bitmap of OFPPS_* flags. */
  /* Bitmaps of OFPPF_* that describe features. All bits zeroed if * unsupported or unavailable. */
  uint32_t curr; /* Current features. */
  uint32_t advertised; /* Features being advertised by the port. */
  uint32_t supported; /* Features supported by the port. */
  uint32_t peer; /* Features advertised by peer. */
  uint32_t curr_speed; /* Current port bitrate in kbps. */
  uint32_t max_speed; /* Max port bitrate in kbps */
}__attribute((aligned(1)));

struct ofp_meter_features
{
  uint32_t max_meter; /*计量器的最大数值. */
  uint32_t band_types; /*支持位图ofpmbt_ *的值*/
  uint32_t capabilities; /*ofp_meter_flags 的位图*/
  uint8_t max_bands; /*每个计量的的最大频带*/
  uint8_t max_color; /*最大的彩色值*/
  uint8_t pad[2];
}__attribute((aligned(1)));

struct ofp_group_features
{
  uint32_t types; /* OFPGT_* 值支持的位图. */
  uint32_t capabilities; /* OFPGFC_* 功能支持的位图. */
  uint32_t max_groups[4]; /* 每个类型的组的最大数量. */
  uint32_t actions[4]; /* 支持的OFPAT_* 的位图. */
}__attribute((aligned(1)));

struct ofp_desc
{
  char mfr_desc[DESC_STR_LEN]; /* 厂商说明. */
  char hw_desc[DESC_STR_LEN]; /* 硬件说明. */
  char sw_desc[DESC_STR_LEN]; /* 软件说明. */
  char serial_num[SERIAL_NUM_LEN]; /* 序列号. */
  char dp_desc[DESC_STR_LEN]; /* 可读的数据通道描述. */
}__attribute((aligned(1)));

struct ofp_action_header
{
  uint16_t type; /* OFPAT _* 之中的一个*/
  uint16_t len; /*行动的长度, 包括它的头部和填充，使行动达到 64-bit */
  uint8_t pad[4];
}__attribute((aligned(1)));

struct ofp_action_output
{
  uint16_t type; /* OFPAT_OUTPUT. */
  uint16_t len; /*长度是 16. */
  uint32_t port; /* 输出端口. */
  uint16_t max_len; /*发送给控制器的最大长度. */
  uint8_t pad[6]; /* 填充到 64 bits. */
}__attribute((aligned(1)));

struct ofp_match
{
  uint16_t type; /* One of OFPMT_ */
  uint16_t length; /* ofp_match 长度(包括填充字段) */
  /* Followed by:
  * - Exactly (length - 4) (possibly 0) bytes containing OXM TLVs, then
  * - Exactly ((length + 7)/8*8 - length) (between 0 and 7) bytes of
  * all-zero bytes
  *总之, 需要时就在 ofp_match 中进行填充, 使其为 8 的倍数，结构长度对齐。
  */
  uint8_t oxm_fields[4]; /* 0 or more OXM match fields */
  uint32_t value;  //  port id
  uint8_t pad[4]; /* Zero bytes - see above for sizing */
}__attribute((aligned(1)));

struct ofp_flow_mod
{
  struct ofp_header header;
  uint64_t cookie; /* 不透明 controller-issued 标识符. */
  uint64_t cookie_mask; /* 限制 cookie 位的掩码，当使用 OFPFC_MODIFY *或 OFPFC_DELETE*命令时必须匹配 cookie。值为 0 表示没有限制*/
  /* 流行动 */
  uint8_t table_id; /* 放入流的表 ID。对 OFPFC_DELETE_*命令,OFPTT_ALL 也可以用来删除所有表里匹配的流。 */
  uint8_t command; /* OFPFC_*之一. */
  uint16_t idle_timeout; /*丢弃之前的空闲时间(seconds). */
  uint16_t hard_timeout; /* 丢弃之前的最大时间 (seconds). */
  uint16_t priority; /*流表项优先级. */
  uint32_t buffer_id; /*缓冲的包使用。或 OFP_NO_BUFFER。对 OFPFC_DELETE *则无意义*/
  uint32_t out_port; /* 对于 OFPFC_DELETE *命令,要求匹配的表项包含这个作为输出端口。OFPP_ANY 表示没有限制*/
  uint32_t out_group; /* 对于 OFPFC_DELETE *命令,要求匹配的表项包括这个作为输出组。值 OFPG_ANY 表示没有限制。*/
  uint16_t flags; /* OFPFF_*的标记位图. */
  uint8_t pad[2];
  struct ofp_match match; /* 字段相匹配，变量的大小. */
  /* 变量的大小和填充匹配总是被指令跟着 */
  //struct ofp_instruction instructions[0]; /* . */
}__attribute((aligned(1)));

struct ofp_packet_out
{
  struct ofp_header header;
  uint32_t buffer_id; /*指定的标识符通过数据通路（OFP_NO_BUFFER 如果没有）*/
  uint32_t in_port; /*分组的输入端口或 OFPP_CONTROLLER*/
  uint16_t actions_len; /*以字节数组的大小的作用 */
  uint8_t pad[6];
  struct ofp_action_output actions; /*行动列表-0 或更多*/
  /*可变大小的行动清单可在后面跟着分组数据。如果 buffer_id=-1 目前的数据是有意义的。*/
  /*uint8_t data[0]；*/ /*分组数据 长度是从长度字段的头部计算的 */
}__attribute((aligned(1)));

struct ofp_packet_in
{
  struct ofp_header header;
  uint32_t buffer_id; /*分配的数据通路 ID。 */
  uint16_t total_len; /*帧全长 */
  uint8_t reason; /*包被发送的原因（一个ofpr_ *）*/
  uint8_t table_id; /*被查询的流表标识符*/
  uint64_t cookie; /*查询的流表项 cookie */
  struct ofp_match match; /*分组元数据。可变尺寸。 */
  /*可变大小和填充匹配总是跟着：
  *2 个全零填充字节，然后是以太网帧，其长度是根据 header.length 推断。
  *以太网帧前填充的字节，确保以太网报头后的 IP 头部（如果有的话）是 32 位
  */
  //uint8_t pad[2]; /* 对齐 n*64 位+ 16 位 */
  //uint8_t data[0]; /*以太网帧 */
}__attribute((aligned(1)));

struct ofp_packet_head
{
  uint8_t dstMac[6];
  uint8_t srcMac[6];
  uint16_t ethType;
}__attribute((aligned(1)));

struct ofp_lldp_tlv
{
  uint8_t tlvType;
  uint8_t tlvLen;
}__attribute((aligned(1)));

#endif
