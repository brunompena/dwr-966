
/******************************************************************************
**
** FILE NAME    : swa_stack_al.h
** PROJECT      : PPA
** MODULES      : Software Acceleration Stack Adaption Layer (Linux)
**
** DATE         : 13 MAR 2014
** AUTHOR       : Lantiq
** DESCRIPTION  : PPA Protocol Stack Adaption Layer (Linux) Header File
** COPYRIGHT    :              Copyright (c) 2009
**                          Lantiq Deutschland GmbH
**                   Am Campeon 3; 85579 Neubiberg, Germany
**
**   For licensing information, see the file 'LICENSE' in the root folder of
**   this software module.
**
** HISTORY
** $Date        $Author         $Comment
*******************************************************************************/

/*! \file ppa_stack_al.h
    \brief This file contains es.
                provide linux os depenent api for PPA to use
*/


#define NULL ((void *)0)

#define SESSION_IS_TCP			0x00000004
#define SESSION_IS_IPV6			0x40000000
#define SESSION_VALID_PPPOE		0x00010000
#define SESSION_LAN_ENTRY		0x10000000
#define SESSION_VALID_VLAN_INS		0x00001000
#define SESSION_VALID_OUT_VLAN_INS	0x00004000
#define SESSION_TX_ITF_IPOA             0x00200000
#define SESSION_TX_ITF_PPPOA            0x00400000
#define SESSION_TX_ITF_IPOA_PPPOA_MASK  (SESSION_TX_ITF_IPOA | SESSION_TX_ITF_PPPOA)
#define SESSION_CAN_NOT_ACCEL		0x00000020
#define SESSION_ADDED_IN_SW		0x80000000
#define PPA_SESSION_EXISTS		1
#define SESSION_VALID_NAT_PORT		0x00000200
#define SESSION_VALID_NAT_IP            0x00000100
#define FLG_PPA_PROCESSED               0x100
#define NETIF_BRIDGE                    0x00000002
#define DBG_ENABLE_MASK_DEBUG_PRINT     (1 << 1)

#define SWA_SUCCESS 	0
#define SWA_FAILURE 	-1
#define SWA_ENOMEM	-12

#define ETH_P_IPV6      0x86DD
#define ETH_P_IP        0x0800
#define ETH_P_PPP_SES   0x8864
#define ETH_P_8021Q     0x8100
#define ENOMEM          12

enum {

	IPPROTO_IP = 0,
	IPPROTO_TCP = 6,
	IPPROTO_UDP = 17,
	IPPROTO_IPV6   = 41
};

void print_fn();
void print_fn1();
void print_fn2();
void print_fn3();
void print_fn4();
void print_fn5();
void print_fn6();
void print_fn7();
void print_fn8(unsigned int x);
typedef struct swa_timespec {
        long		tv_sec;                 /* seconds */
        long            tv_nsec;                /* nanoseconds */
} SWA_TIMESPEC;

typedef union {
        unsigned long ip;        /*!< the storage buffer for ipv4 */
#ifdef CONFIG_LTQ_PPA_IPv6_ENABLE
        unsigned long ip6[4];    /*!< the storage buffer for ipv6 */
#endif
struct udphdr {
        unsigned short src;
        unsigned short dest;
        unsigned short len;
        unsigned short checksum;
} __packed;
 }SWA_IPADDR;

typedef struct swa_hlist_node {
        struct swa_hlist_node *next, **pprev;
}SWA_HLIST_NODE;

struct swa_session_list_item {

    SWA_HLIST_NODE              hlist;
    void                        *session;
    //PPA_LOCK                    uc_lock;
    //PPA_SESSION_EXTRA           session_extra;
    unsigned short                    ip_proto;
    unsigned short                    ip_tos;
    SWA_IPADDR			src_ip;
    unsigned short                    src_port;
    unsigned char                     src_mac[6];
    SWA_IPADDR                  dst_ip;
    unsigned short                    dst_port;
    unsigned char                     dst_mac[6];
    SWA_IPADDR			nat_ip;         //  IP address to be replaced by NAT if NAT applies
    unsigned short                    nat_port;       //  Port to be replaced by NAT if NAT applies
    unsigned short                    num_adds;       //  Number of attempts to add session
#if defined(CONFIG_LTQ_PPA_HANDLE_CONNTRACK_SESSIONS)
    unsigned short                    ewma_num_adds;	//  Number of attempts over which session criteria is decided
    unsigned long long                    ewma_bytes;	// EWMA in bytes 
    unsigned long long                    ewma_time;	// EWMA in time 
    SWA_TIMESPEC		timespent;		// Time spent in EWMA window
    unsigned long long                    ewma_session_bytes;	// bytes accmulated by mips over a EWMA window
    unsigned long                    session_priority;	// priority of session
#endif
    void        	          *rx_if;
    void	                  *tx_if;
    unsigned long                    timeout;
    unsigned long                    last_hit_time;  //  Updated by bookkeeping thread
    unsigned long                    new_dscp;
    unsigned short                    pppoe_session_id;
    unsigned short                    new_vci;
    unsigned long                    out_vlan_tag;
    unsigned long                    mtu;
    unsigned short                    dslwan_qid;
    unsigned short                    dest_ifid;

    unsigned long                    flags;          //  Internal flag : SESSION_IS_REPLY, SESSION_IS_TCP,
                                                //                  SESSION_ADDED_IN_HW, SESSION_NOT_ACCEL_FOR_MGM
                                                //                  SESSION_VALID_NAT_IP, SESSION_VALID_NAT_PORT,
                                                //                  SESSION_VALID_VLAN_INS, SESSION_VALID_VLAN_RM,
                                                //                  SESSION_VALID_OUT_VLAN_INS, SESSION_VALID_OUT_VLAN_RM,
                                                //                  SESSION_VALID_PPPOE, SESSION_VALID_NEW_SRC_MAC,
                                                //                  SESSION_VALID_MTU, SESSION_VALID_NEW_DSCP,
                                                //                  SESSION_VALID_DSLWAN_QID,
                                                //                  SESSION_TX_ITF_IPOA, SESSION_TX_ITF_PPPOA
                                                //                  SESSION_LAN_ENTRY, SESSION_WAN_ENTRY, SESSION_IS_IPV6
#if defined(CONFIG_LTQ_PPA_API_SW_FASTPATH)
    void			*sw_acc_hdr;
#endif
#if defined(ENABLE_SESSION_DEBUG_FLAGS) && ENABLE_SESSION_DEBUG_FLAGS
    unsigned long                    debug_flags;
#endif

    unsigned long                    routing_entry;
    unsigned long                    pppoe_entry;
    unsigned long                    mtu_entry;
    unsigned long                    src_mac_entry;
    unsigned long                    out_vlan_entry;
#if defined(SKB_PRIORITY_DEBUG) && SKB_PRIORITY_DEBUG
    unsigned long                    priority;    //skb priority. for debugging only
    unsigned long                    mark;    //skb mark value. for debugging only
#endif
    unsigned long long                    mips_bytes; //bytes handled by mips
    unsigned long long                    acc_bytes; //bytes handled by PPE acceleration
    unsigned long                    last_bytes; //last updated bytes handled by PPE acceleration
    unsigned long long                    prev_sess_bytes; //last updated bytes. This is for PPA session management purpose
    unsigned long long                    prev_clear_acc_bytes;  //last cleared bytes. We don't really clear session's acceleration mib in PPE FW, but instead, we just record the mib counter for adjustment later
    unsigned long long                    prev_clear_mips_bytes;  //last cleared bytes. We don't really clear session's acceleration mib in PPE FW, but instead, we just record the mib counter for adjustment later
    unsigned long                    tunnel_idx; //tunnel idx for PPE action table
    unsigned char                     collision_flag; // 1 mean the entry is in collsion table or no hashed table, like ASE/Danube

#if defined(CONFIG_LTQ_PPA_IF_MIB) && CONFIG_LTQ_PPA_IF_MIB
    void	                   *br_tx_if;  //record its subinterface name under bridge interface
    void	                   *br_rx_if;  //record its subinterface name under bridge interface
#endif    

#if defined(SESSION_STATISTIC_DEBUG) && SESSION_STATISTIC_DEBUG 
    /*below variable is used for session management debugging purpose */
    unsigned long                     flag2; //SESSION_FLAG2_HASH_INDEX_DONE/SESSION_FLAG2_ADD_HW_FAIL 
    unsigned short                     hash_index;
    unsigned short                     hash_table_id; /* 0-first hash table, 1 WAN */
    unsigned short                     src_ip6_index;  /* Note, 0 means not valid data. so for its correct value, it should be "real index + 1 "  */
    unsigned short                     src_ip6_psuedo_ip; 
    unsigned short                     dst_ip6_index;  /* Note, 0 means not valid data. so for its correct value, it should be "real index + 1 " */
    unsigned short                     dst_ip6_psuedo_ip;    
#endif   

};

extern signed long (*ppa_sw_add_session_hook)(void *skb, void *p_item);
extern signed long (*ppa_sw_update_session_hook)(void *skb, void *p_item,void *tx_ifinfo);
extern void (*ppa_sw_del_session_hook)(void *p_item);

extern signed long (*ppa_sw_fastpath_enable_hook)(unsigned long, unsigned long);
extern signed long (*ppa_get_sw_fastpath_status_hook)(unsigned long *, unsigned long);
extern signed long (*ppa_sw_fastpath_send_hook)(void *);

extern signed long sw_fastpath_send(void *skb);
extern signed long get_sw_fastpath_status(unsigned long *f_enable, unsigned long flags);
extern signed long sw_fastpath_enable(unsigned long f_enable, unsigned long flags);

extern void sw_del_session(void *pitem);
extern signed long sw_add_session(void *skb, void *pitem);
extern signed long sw_update_session(void *skb, void *pitem, void *tx_ifinfo);

void *swa_malloc(unsigned long size);

signed long swa_free(void *buf);

void swa_memset(void *dst, unsigned long pad, unsigned long n);

void swa_skb_dev(void *skb, void *itf);

void swa_skb_update_mark(void *skb1,int flg);

unsigned int swa_skb_len(void *skb);

int swa_skb_mark(void *skb);

unsigned char *swa_skb_head(void *skb);

unsigned char *swa_skb_pull(void *skb,unsigned int len);

unsigned char *swa_skb_data(void *skb);

unsigned char *swa_skb_push(void *skb,unsigned int len);

unsigned int swa_skb_headroom(void *skb);

struct sk_buff *swa_skb_realloc_headroom(void *skb, unsigned int len);

void swa_nf_conntrack_put(void *sess);

void SWA_SKB_FREE(void *skb);

struct sock *swa_skb_sk(void *skb);

void swa_skb_set_owner_w(void *skb,void *sk);

void swa_skb_set_mac_header(void *skb, int offset);

void swa_skb_set_network_header(void *skb, int offset);

void swa_skb_set_transport_header(void *skb, int offset);

unsigned char *swa_skb_network_header(void *skb);

unsigned short swa_ip_fast_csum(void *iph, unsigned int len);

unsigned char *swa_skb_transport_header(void *skb);

struct net_device *swa_get_netif(void *ifinfo);

unsigned int swa_get_flags(void *ifinfo);

int swa_get_br_dst_port(void *netif, void *skb, void **p_netif);

void swa_memcpy_data(void *dst, void *src, unsigned long offset);

void swa_memcpy(void *dst, void *src, unsigned long offset);

int swa_get_netif_hwaddr(void *tx_if, unsigned char *hdr, unsigned long i);

int swa_get_conntrack_from_skb(void *skb, void *sess, int *reply);

int __swa_lookup_session(void *sess, int reply, void *pitem);

void swa_inet_proto_csum_replace4_u(void *hdr, void *skb, unsigned int from, unsigned int to);

void swa_inet_proto_csum_replace4_t(void *hdr, void *skb, unsigned int from, unsigned int to);

void swa_inet_proto_csum_replace2_u(void *hdr, void *skb, unsigned short from, unsigned short to);

void swa_inet_proto_csum_replace2_t(void *hdr, void *skb, unsigned short from, unsigned short to);

int swa_get_time_in_sec();

int swa_dev_queue_xmit(void *skb);



