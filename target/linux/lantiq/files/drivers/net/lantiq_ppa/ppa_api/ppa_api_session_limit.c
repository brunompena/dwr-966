/*******************************************************************************
**
** FILE NAME    : ppa_api_session_limit.c
** PROJECT      : PPA
** MODULES      : PPA API (Session Limiting APIs)
**
** DATE         : 20 May 2014
** AUTHOR       : Lantiq
** DESCRIPTION  : PPA Session Limiting APIs
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


#include <linux/version.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 33)
#include <linux/autoconf.h>
#else
#include <generated/autoconf.h>
#endif


#include <linux/swap.h>
#include <net/ppa_api.h>
#include <net/ppa_ppe_hal.h>
#include "ppa_api_misc.h"
#include "ppa_api_netif.h"
#include "ppa_api_session.h"
#include "ppa_api_session_limit.h"
#include "ppa_api_mib.h"
#include "ppe_drv_wrapper.h"
#include "ppa_api_tools.h"
#include <net/ppa_stack_al.h>
#if defined(CONFIG_LTQ_CPU_FREQ) || defined(CONFIG_LTQ_PMCU) || defined(CONFIG_LTQ_PMCU_MODULE)
#include "ppa_api_pwm.h"
#endif
uint32_t g_ppa_lan_reserve_collisions = LAN_RESERVE_COLLISIONS;
uint32_t g_ppa_wan_reserve_collisions = WAN_RESERVE_COLLISIONS;
uint32_t g_ppa_lan_current_collisions = 0;
uint32_t g_ppa_wan_current_collisions = 0;
uint32_t g_ppa_low_prio_data_rate = 0;
uint32_t g_ppa_def_prio_data_rate = 0;
uint32_t g_ppa_low_prio_thresh = 0;
uint32_t g_ppa_def_prio_thresh = 0;

uint32_t ppa_reset_ewma_stats( struct session_list_item *p_item,uint32_t flags)
{
	if( p_item->ewma_num_adds >= EWMA_WINDOW_PACKETS ) {
		p_item->ewma_session_bytes = 0;
		p_item->ewma_num_adds = 0;
	}
	return PPA_SUCCESS;
}
EXPORT_SYMBOL(ppa_reset_ewma_stats);

uint32_t ppa_session_pass_criteria(PPA_BUF *ppa_buf, struct session_list_item *p_item,uint32_t flags)
{
    uint64_t time_msec;
    uint64_t total_bytes;
    PPA_TIMESPEC after;
    int multiply_offset = 1;

    // This is the most basic critria which should be met for all sessions
    if ( p_item->num_adds < g_ppa_min_hits )
	  return PPA_FAILURE;

    // Return if session limit check is not enabled
    if ( ! ppa_get_session_limit_enable(0) )
	  return PPA_SUCCESS;
		
    p_item->session_priority = ppa_get_session_priority( ppa_buf );
    g_ppa_def_prio_thresh = ppa_get_def_prio_thresh(0);

    // High priority can directly be added to acceleration
    if ( p_item->session_priority  > g_ppa_def_prio_thresh)
	  return PPA_SUCCESS;

    g_ppa_low_prio_data_rate = ppa_get_low_prio_data_rate(0);
    g_ppa_def_prio_data_rate = ppa_get_def_prio_data_rate(0);

    // Do the math only if any one one of the data checks is enabled
    if ( ( g_ppa_low_prio_data_rate != 0) || (g_ppa_def_prio_data_rate != 0 ) ) {
	  if( (p_item->ewma_num_adds == EWMA_WINDOW_PACKETS) && (p_item->num_adds >= EWMA_IGNORE_SEED_PACKETS) ){
	      ppa_get_monotonic(&after);
	      PPA_TIMESPEC sleep_time = ppa_timespec_sub(after, p_item->timespent);
	      time_msec = (uint64_t ) ppa_timespec_to_ns(&sleep_time); 
	      // Convert bytes to bits = Multiply by 8 = Left shift by 3
	      total_bytes = (uint64_t) (p_item->ewma_session_bytes << 3) * NSEC_PER_SEC; 
	  
	      // EWMA calculation: We give 1/4 to current value and 3/4 to previous value. Such ratios allow shift operations
	      if ( p_item->ewma_bytes == 0 ) {
		      p_item->ewma_bytes = total_bytes; 
		      p_item->ewma_time = time_msec;
		      if ( ppa_get_pkt_ip_proto(ppa_buf) == PPA_IPPROTO_TCP )
		      	multiply_offset = ppa_get_tcp_initial_offset(0);
	      } else {
		      p_item->ewma_bytes = ( total_bytes + (p_item->ewma_bytes * 3)) >> 2;
		      p_item->ewma_time = ( time_msec + (p_item->ewma_time * 3)) >> 2;
		      if ( ppa_get_pkt_ip_proto(ppa_buf) == PPA_IPPROTO_TCP )
		      	multiply_offset = ppa_get_tcp_steady_offset(0);
	      }

	      total_bytes = p_item->ewma_bytes;
	      time_msec = p_item->ewma_time;

	      if ( flags == 1 ) // Updated moving average bytes and time. Return.
		      return PPA_SUCCESS;

	      // Data rate is EWMA of bytes divided by EWMA of time
	      do_div(total_bytes,time_msec);
	      ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"Session data rate=%llu bytes=%llu time=%llu prio=%d low_thresh=%d def_thresh=%d low_rate=%lu def_rate=%lu\n",total_bytes,p_item->ewma_bytes,time_msec,p_item->session_priority,g_ppa_low_prio_thresh,g_ppa_def_prio_thresh,g_ppa_low_prio_data_rate,g_ppa_def_prio_data_rate);
	  }

	  // We ignore first few packets to negate the bursty nature of traffic which wil give us wrong seed to start with for EWMA
	  if ( p_item->ewma_num_adds < EWMA_WINDOW_PACKETS || p_item->num_adds < EWMA_IGNORE_SEED_PACKETS )
		return PPA_FAILURE;

	  g_ppa_low_prio_thresh = ppa_get_low_prio_thresh(0);
	  // If low priority session does not meet its threshold data rate return failure.
	  if ( p_item->session_priority <= g_ppa_low_prio_thresh && total_bytes <= ( g_ppa_low_prio_data_rate * multiply_offset) )
		return PPA_FAILURE;

	  // If default priority session does not meet its threshold data rate return failure.
	  if ( p_item->session_priority > g_ppa_low_prio_thresh && total_bytes <= ( g_ppa_def_prio_data_rate * multiply_offset))
	      return PPA_FAILURE;

    }

    // Either data rate checks are not enabled or session has passed all criteria. Can be added to acceleration hence return success
    return PPA_SUCCESS;
}
EXPORT_SYMBOL(ppa_session_pass_criteria);

uint32_t ppa_update_ewma(PPA_BUF *ppa_buf,struct session_list_item *p_item,uint32_t flags)
{
		if ( ppa_get_session_limit_enable(0) ) {
			ppa_session_pass_criteria(ppa_buf,p_item,1); // Just update moving average bytes and time
			ppa_reset_ewma_stats(p_item,0);
		}
		return PPA_SUCCESS;
}
EXPORT_SYMBOL(ppa_update_ewma);
uint32_t ppa_session_update_collisions(uint32_t flags)
{
    g_ppa_lan_current_collisions =  ppa_num_collision_sessions(0);
    g_ppa_wan_current_collisions =  ppa_num_collision_sessions(1);
	return PPA_SUCCESS;
}
EXPORT_SYMBOL(ppa_session_update_collisions);

uint32_t ppa_session_store_ewma( PPA_BUF *ppa_buf, struct session_list_item *p_item,uint32_t flags)
{
    p_item->ewma_session_bytes   += ppa_buf->len + PPA_ETH_HLEN + PPA_ETH_CRCLEN;
    p_item->ewma_num_adds ++;
	return PPA_SUCCESS;
}
EXPORT_SYMBOL(ppa_session_store_ewma);

uint32_t ppa_decide_collision(struct session_list_item *p_item) {
	if ( ! ppa_get_session_limit_enable(0) ) // Just return in case session limit is not enabled
		return PPA_SUCCESS;
	if ( p_item->flags & SESSION_LAN_ENTRY ) {
		PPA_MAX_ENTRY_INFO entry={0}; /*!< max entry info*/
		if (ppa_drv_get_max_entries(&entry, 0) != PPA_FAILURE ) {
		    	if ( p_item->session_priority <= g_ppa_def_prio_thresh  && ((entry.max_lan_collision_entries - g_ppa_lan_current_collisions) <= g_ppa_lan_reserve_collisions)) 
				p_item->collision_flag = 2; // This session should not be added to collision table in case hash table is already full
        			ppa_debug(DBG_ENABLE_MASK_DEBUG2_PRINT,"set collision to 2, p_item = 0x%08x\n", (u32)p_item);
				return PPA_SUCCESS;
		} else {
			ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"could not get max entries for lan entry!!\n");
			return PPA_FAILURE;
		}
	} else {
		PPA_MAX_ENTRY_INFO entry={0}; /*!< max entry info*/
		if (ppa_drv_get_max_entries(&entry, 0) != PPA_FAILURE ) {
		    	if ( p_item->session_priority <= g_ppa_def_prio_thresh && ((entry.max_wan_collision_entries - g_ppa_wan_current_collisions) <= g_ppa_wan_reserve_collisions)) 
				p_item->collision_flag = 2; // This session should not be added to collision table in case hash table is already full
        			ppa_debug(DBG_ENABLE_MASK_DEBUG2_PRINT,"set collision to 2, p_item = 0x%08x\n", (u32)p_item);
				return PPA_SUCCESS;
		} else {
			ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"could not get max entries for wan entry!!\n");
			return PPA_FAILURE;
		}
	}
}
EXPORT_SYMBOL(ppa_decide_collision);

uint32_t ppa_session_limit_update_flags(struct session_list_item *p_item,uint32_t flags)
{
		/* Criteria for setting SESSION_NOT_ACCEL_FOR_MGM:
		(1) If session limit is not enabled
		(2) Extra info flags has already flag set ( coming from ppacmd modifysession )
		if we set this flag, packet will go through linux stack and hit ppa_speed_handle_frame and return at postrouting stage.
		Note: The ppasessmgmtd will never set this flag though extra info flags*/
		if ( ! ppa_get_session_limit_enable(0) || ( flags & SESSION_NOT_ACCEL_FOR_MGM) ) 
	            p_item->flags |= SESSION_NOT_ACCEL_FOR_MGM;
		return PPA_SUCCESS;
}
EXPORT_SYMBOL(ppa_session_limit_update_flags);

uint32_t  ppa_session_record_time(PPA_BUF *ppa_buf,struct session_list_item *p_item,uint32_t flags)
{
	PPA_TIMESPEC before;
	if ( ppa_get_session_limit_enable(0) ) {
		if ( p_item->ewma_num_adds == 0 ) {
		    ppa_get_monotonic(&before);
		    p_item->timespent.tv_nsec = before.tv_nsec;
		    p_item->timespent.tv_sec = before.tv_sec;
		}
        	p_item->ewma_num_adds++;
	        p_item->ewma_session_bytes += ppa_buf->len + PPA_ETH_HLEN + PPA_ETH_CRCLEN;
	} else { // reset all ewma parameters
		    p_item->timespent.tv_nsec = 0;
		    p_item->timespent.tv_sec = 0;
        	    p_item->ewma_num_adds = 0;
	            p_item->ewma_session_bytes = 0;
	}
	return PPA_SUCCESS;
}
EXPORT_SYMBOL(ppa_session_record_time);

uint32_t ppa_init_session_limit_params(PPA_INIT_INFO *p_info,uint32_t flags)
{
    if( p_info->add_requires_lan_collisions ) 
        g_ppa_lan_reserve_collisions = p_info->add_requires_lan_collisions;

    if( p_info->add_requires_wan_collisions )
        g_ppa_wan_reserve_collisions = p_info->add_requires_wan_collisions;
    
    g_ppa_low_prio_data_rate = ppa_get_low_prio_data_rate(0);
    g_ppa_def_prio_data_rate = ppa_get_def_prio_data_rate(0);
    g_ppa_low_prio_thresh = ppa_get_low_prio_thresh(0);
    g_ppa_def_prio_thresh = ppa_get_def_prio_thresh(0);
	return PPA_SUCCESS;
}
EXPORT_SYMBOL(ppa_init_session_limit_params);

uint32_t ppa_num_collision_sessions(uint32_t flag)
{
    struct session_list_item *pp_item;
    uint32_t pos = 0;
    uint32_t num_collision_sessions = 0;

    if ( ppa_session_start_iteration(&pos, &pp_item) == PPA_SUCCESS )
    {
        do
        {   
	    if (pp_item->collision_flag == 1 && ( pp_item->flags & ( flag ? SESSION_WAN_ENTRY: SESSION_LAN_ENTRY) ) )
		num_collision_sessions++;
        } while ( ppa_session_iterate_next(&pos, &pp_item) == PPA_SUCCESS  );
    }
    
    ppa_session_stop_iteration();

    return (num_collision_sessions);
}
EXPORT_SYMBOL(ppa_num_collision_sessions);
