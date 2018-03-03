/*******************************************************************************
**
** FILE NAME    : ppa_api_session_limit.h
** PROJECT      : PPA
** MODULES      : PPA API (Session Limiting APIs)
**
** DATE         : 20 May 2014
** AUTHOR       : Lantiq
** DESCRIPTION  : PPA Session Limiting Header
** COPYRIGHT    :              Copyright (c) 2009
**                          Lantiq Deutschland GmbH
**                   Am Campeon 3; 85579 Neubiberg, Germany
**
**   For licensing information, see the file 'LICENSE' in the root folder of
**   this software module.
**
** HISTORY
** $Date        $Author                $Comment
**                                     Features
*******************************************************************************/

#include <net/netfilter/nf_conntrack_session_limit.h>
#define EWMA_WINDOW_PACKETS 10
#define EWMA_IGNORE_SEED_PACKETS 30
#define LAN_RESERVE_COLLISIONS                  0
#define WAN_RESERVE_COLLISIONS                  0
extern uint32_t g_ppa_lan_reserve_collisions;
extern uint32_t g_ppa_wan_reserve_collisions;
extern uint32_t g_ppa_lan_current_collisions;
extern uint32_t g_ppa_wan_current_collisions;
extern uint32_t g_ppa_low_prio_data_rate;
extern uint32_t g_ppa_def_prio_data_rate;
extern uint32_t g_ppa_low_prio_thresh;
extern uint32_t g_ppa_def_prio_thresh;
extern uint32_t ppa_reset_ewma_stats( struct session_list_item *p_item,uint32_t flags);
extern uint32_t ppa_session_pass_criteria(PPA_BUF *ppa_buf, struct session_list_item *p_item,uint32_t flags);
extern uint32_t ppa_update_ewma(PPA_BUF *ppa_buf,struct session_list_item *p_item,uint32_t flags);
extern uint32_t ppa_session_update_collisions(uint32_t flags);
extern uint32_t ppa_session_store_ewma( PPA_BUF *ppa_buf, struct session_list_item *p_item,uint32_t flags);
extern uint32_t ppa_decide_collision(struct session_list_item *p_item);
extern uint32_t ppa_num_collision_sessions( uint32_t flag );
extern uint32_t ppa_session_limit_update_flags(struct session_list_item *p_item,uint32_t flags);
extern uint32_t  ppa_session_record_time(PPA_BUF *ppa_buf,struct session_list_item *p_item,uint32_t flags);
extern uint32_t ppa_init_session_limit_params(PPA_INIT_INFO *p_info,uint32_t flags);
