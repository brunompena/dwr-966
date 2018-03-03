/*******************************************************************************
**
** FILE NAME    : ppa_api_directpath.c
** PROJECT      : PPA
** MODULES      : PPA API (Routing/Bridging Acceleration APIs)
**
** DATE         : 19 NOV 2008
** AUTHOR       : Xu Liang
** DESCRIPTION  : PPA Protocol Stack Hook API Directpath Functions
**                File
** COPYRIGHT    :              Copyright (c) 2009
**                          Lantiq Deutschland GmbH
**                   Am Campeon 3; 85579 Neubiberg, Germany
**
**   For licensing information, see the file 'LICENSE' in the root folder of
**   this software module.
**
** HISTORY
** $Date        $Author         $Comment
** 19 NOV 2008  Xu Liang        Initiate Version
*******************************************************************************/



/*
 * ####################################
 *              Head File
 * ####################################
 */

/*
 *  Common Head File
 */
//#include <linux/autoconf.h>
#include <linux/version.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 33)
#include <linux/autoconf.h>
#else
#include <generated/autoconf.h>
#endif


//#include <linux/kernel.h>
//#include <linux/module.h>
//#include <linux/version.h>
//#include <linux/types.h>
//#include <linux/init.h>
//#include <linux/slab.h>
//#if defined(CONFIG_LTQ_PPA_API_PROC)
//#include <linux/proc_fs.h>
//#endif
//#include <linux/netdevice.h>
//#include <linux/in.h>
//#include <net/sock.h>
//#include <net/ip_vs.h>
//#include <asm/time.h>

/*
 *  PPA Specific Head File
 */
#include <net/ppa_api.h>
#include <net/ppa_api_directpath.h>
#include <net/ppa_ppe_hal.h>
#include "ppa_api_misc.h"
#include "ppa_api_netif.h"
#include "ppe_drv_wrapper.h"
#include "ppa_api_misc.h"
#include "ppa_api_tools.h"


/*
 * ####################################
 *              Definition
 * ####################################
 */



/*
 * ####################################
 *              Data Type
 * ####################################
 */



/*
 * ####################################
 *             Declaration
 * ####################################
 */

static void update_itf_info(void);



/*
 * ####################################
 *           Global Variable
 * ####################################
 */

static PPA_LOCK g_directpath_port_lock;
static uint32_t g_start_ifid = ~0, g_end_ifid = ~0;



/*
 * ####################################
 *           Extern Variable
 * ####################################
 */



/*
 * ####################################
 *            Local Function
 * ####################################
 */

static void update_itf_info(void)
{
    uint32_t i;
    PPE_IFINFO if_info;
    uint32_t tmp_flags;

    ppa_memset(&if_info, 0, sizeof(if_info));

    for ( i = g_start_ifid; i < g_end_ifid; i++ )
    {
        if_info.port = i;
        if ( ppa_drv_get_phys_port_info(&if_info, 0) == PPA_SUCCESS
            && (if_info.if_flags & (PPA_PHYS_PORT_FLAGS_VALID | PPA_PHYS_PORT_FLAGS_TYPE_MASK)) == (PPA_PHYS_PORT_FLAGS_VALID | PPA_PHYS_PORT_FLAGS_TYPE_EXT)
            && (if_info.if_flags & (PPA_PHYS_PORT_FLAGS_EXT_CPU0 | PPA_PHYS_PORT_FLAGS_EXT_CPU1)) != 0 )
        {
            tmp_flags = ppa_drv_g_ppe_directpath_data[i - g_start_ifid].flags & ~PPE_DIRECTPATH_LANWAN_MASK;
            if ( (if_info.if_flags & PPA_PHYS_PORT_FLAGS_MODE_LAN) )
                tmp_flags |= PPE_DIRECTPATH_LAN;
            if ( (if_info.if_flags & PPA_PHYS_PORT_FLAGS_MODE_WAN) )
                tmp_flags |= PPE_DIRECTPATH_WAN;
            ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT, "%s: if_info.if_flags = 0x%08x, direct_flag[%d]=0x%08x, tmp_flags=0x%08x\n", __FUNCTION__, if_info.if_flags, i - g_start_ifid, ppa_drv_g_ppe_directpath_data[i - g_start_ifid].flags, tmp_flags);
            ppa_drv_g_ppe_directpath_data[i - g_start_ifid].flags = tmp_flags;
        }
    }
}

static PPA_BUF *__remove_directpath_dev_from_datapath(int if_id)
{
    //unsigned long sys_flag;
    uint32_t tmp_flags;
    
#if defined(CONFIG_LTQ_PPA_DIRECTPATH_TX_QUEUE_SIZE) || defined(CONFIG_LTQ_PPA_DIRECTPATH_TX_QUEUE_PKTS)
    PPA_BUF *skb_list = NULL;
#endif
    
    if ( (ppa_drv_g_ppe_directpath_data[if_id].flags & PPE_DIRECTPATH_DATA_ENTRY_VALID) )
    {
        ppa_phys_port_remove(if_id + g_start_ifid);
        //sys_flag = ppa_disable_int();
        tmp_flags = ppa_drv_g_ppe_directpath_data[if_id].flags & PPE_DIRECTPATH_MASK;
#if defined(CONFIG_LTQ_PPA_DIRECTPATH_TX_QUEUE_SIZE) || defined(CONFIG_LTQ_PPA_DIRECTPATH_TX_QUEUE_PKTS)
        ppe_lock_get(&ppa_drv_g_ppe_directpath_data[if_id].txq_lock);
        skb_list = ppa_drv_g_ppe_directpath_data[if_id].skb_list;
        ppa_drv_g_ppe_directpath_data[if_id].skb_list = NULL;
        ppe_lock_release(&ppa_drv_g_ppe_directpath_data[if_id].txq_lock);
#endif  
        ppa_memset(&ppa_drv_g_ppe_directpath_data[if_id], 0, sizeof(ppa_drv_g_ppe_directpath_data[if_id]));
        ppa_drv_g_ppe_directpath_data[if_id].flags = tmp_flags;
        //ppa_enable_int(sys_flag);
        ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"directp unregiter ok and restore direct_flag[%d]=%x\n", if_id, ppa_drv_g_ppe_directpath_data[if_id].flags );
    }

#if defined(CONFIG_LTQ_PPA_DIRECTPATH_TX_QUEUE_SIZE) || defined(CONFIG_LTQ_PPA_DIRECTPATH_TX_QUEUE_PKTS)
    return skb_list;
#endif
    return NULL;
}

#if defined(CONFIG_LTQ_PPA_DIRECTPATH_TX_QUEUE_SIZE) || defined(CONFIG_LTQ_PPA_DIRECTPATH_TX_QUEUE_PKTS)
static int remove_directpath_queue(PPA_BUF *skb_list)
{
    PPA_BUF *skb, *skb_list_bak;

    if ( skb_list )
    {
      skb_list_bak = skb_list;
      do
      {
          skb = skb_list;
          skb_list = ppa_buf_get_next(skb_list);
          ppa_buf_free(skb);
      } while ( skb_list != NULL && skb_list != skb_list_bak );
    }
    return 0;
}
#endif

/*
 * ####################################
 *           Global Function
 * ####################################
 */

/*
 *  PPA DirectPath Functions
 */
#ifdef CONFIG_ACCL_11AC
int32_t ppa_directlink_register_dev(int32_t *p_if_id, PPA_DTLK_T *dtlk, PPA_DIRECTPATH_CB *pDirectpathCb,uint32_t flags)
{
   int32_t ret = PPA_SUCCESS;
   uint32_t if_id = ~0;

   if(!ppa_is_init()) 
      return PPA_EINVAL;
   
   if(!p_if_id || !dtlk || !dtlk->dev)
      return PPA_EINVAL;

   if(dtlk->flags & PPE_F_DTLK_REGISTER){//register directlink
       printk("%s: register directlink\n", __func__); 
      if((ret = datapath_dtlk_register(p_if_id, dtlk)) != PPA_SUCCESS){
		return ret;
	}
   }
   
   if(dtlk->flags & PPE_F_DTLK_DP_REGISTER){//register to directpath
       if((ret=ppa_directpath_register_dev(&if_id,dtlk->dev,pDirectpathCb,flags)) == PPA_SUCCESS){
            printk("%s: register directpath\n", __func__); 
            if((ret=datapath_dtlk_update(*p_if_id,dtlk, &if_id)) != PPA_SUCCESS){
		return ret;
		} 
       }else{
		return ret;
       }
   }else if(dtlk->flags & PPE_F_DTLK_DP_DEREGISTER){//deregister from directpath
        printk("%s: de-register directpath\n", __func__); 
        datapath_dtlk_update(*p_if_id,dtlk, &if_id);
        if((ret=ppa_directpath_register_dev(&if_id,dtlk->dev,pDirectpathCb,flags)) != PPA_SUCCESS){
		return ret;
	}
   }

   if(dtlk->flags & PPE_F_DTLK_DEREGISTER){//deregister from directlink
        printk("%s: de-register directlink\n", __func__); 
        ret = datapath_dtlk_register(p_if_id, dtlk);
   }
   
   return ret;      
}
#endif

int32_t ppa_directpath_register_dev(uint32_t *p_if_id, PPA_NETIF *netif, PPA_DIRECTPATH_CB *pDirectpathCb, uint32_t flags)
{
    int32_t ret;
    uint32_t if_id;
    uint32_t tmp_flags;
    PPA_BUF *skb_list = NULL;

    if( !ppa_is_init() ) return PPA_EINVAL;

    if ( p_if_id == NULL || !ppa_drv_g_ppe_directpath_data  )
        return PPA_EINVAL;

    if ( (flags & PPA_F_DIRECTPATH_REGISTER) )
    {
        if ( !(flags & PPA_F_DIRECTPATH_ETH_IF) )
            //  currently, only ethernet interface is supported
            return PPA_EINVAL;

        if ( !netif || !pDirectpathCb
            || !pDirectpathCb->rx_fn )
            return PPA_EINVAL;

        if ( ppa_get_netif_name(netif) == NULL )
            return PPA_EINVAL;

        tmp_flags = (flags & PPA_F_DIRECTPATH_WAN) ? PPE_DIRECTPATH_WAN : PPE_DIRECTPATH_LAN;
        if ( (flags & PPA_F_DIRECTPATH_CORE1) )
            tmp_flags |= PPE_DIRECTPATH_CORE1;
        else
            tmp_flags |= PPE_DIRECTPATH_CORE0;
        if ( (flags & PPA_F_DIRECTPATH_ETH_IF) )
            tmp_flags |= PPE_DIRECTPATH_ETH;

        //  update flags which could be changed on the fly, e.g. AR9/VR9/AR10 - LAN/WAN could be changed
        update_itf_info();

        ppa_lock_get(&g_directpath_port_lock);

        //first check whether the interface already added into PPA directpath or not
        for ( if_id = 0; if_id < g_end_ifid; if_id ++ )
        {
            if( ppa_drv_g_ppe_directpath_data[if_id].netif && ( ppa_drv_g_ppe_directpath_data[if_id].netif == netif ) )
            {
                ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT, "The interface already in PPA directpath already. Its netif pointer is :%p\n", netif);
                if(p_if_id ) *p_if_id = ppa_drv_g_ppe_directpath_data[if_id].ifid ;
                ppa_lock_release(&g_directpath_port_lock);
                return PPA_SUCCESS;
            }
        }

        for ( if_id = 0;
              if_id < g_end_ifid - g_start_ifid && (ppa_drv_g_ppe_directpath_data[if_id].flags & (PPE_DIRECTPATH_DATA_ENTRY_VALID | PPE_DIRECTPATH_ITF_TYPE_MASK | tmp_flags)) != tmp_flags;
              if_id++ )
        {
                ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT, "%s: direct_flag[%d]=0x%08x, tmp_flags=0x%08x\n", __FUNCTION__, if_id, ppa_drv_g_ppe_directpath_data[if_id].flags, tmp_flags);
        }
        if ( if_id < g_end_ifid - g_start_ifid
            && ppa_phys_port_add(ppa_get_netif_name(netif), if_id + g_start_ifid) == PPA_SUCCESS )
        {
            tmp_flags = (ppa_drv_g_ppe_directpath_data[if_id].flags & PPE_DIRECTPATH_MASK) | PPE_DIRECTPATH_DATA_ENTRY_VALID | PPE_DIRECTPATH_DATA_RX_ENABLE;
            ppa_memset(&ppa_drv_g_ppe_directpath_data[if_id], 0, sizeof(ppa_drv_g_ppe_directpath_data[if_id]));
#if defined(CONFIG_LTQ_PPA_DIRECTPATH_TX_QUEUE_SIZE) || defined(CONFIG_LTQ_PPA_DIRECTPATH_TX_QUEUE_PKTS)
            ppe_lock_init(&ppa_drv_g_ppe_directpath_data[if_id].txq_lock);
#endif
            ppa_drv_g_ppe_directpath_data[if_id].callback   = *pDirectpathCb;
            ppa_drv_g_ppe_directpath_data[if_id].netif      = netif;
            ppa_drv_g_ppe_directpath_data[if_id].ifid       = if_id + g_start_ifid;
            ppa_drv_g_ppe_directpath_data[if_id].flags      = tmp_flags;
            *p_if_id = if_id + g_start_ifid;
            ret = PPA_SUCCESS;
        }
        else
        {
            ret = PPA_FAILURE;
            ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT, "direct register fail\n" );
        }
        ppa_lock_release(&g_directpath_port_lock);

        return ret;
    }
    else
    {
        if ( *p_if_id < g_start_ifid || *p_if_id >= g_end_ifid )
        {
            ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"directp unregister wrong id: %d\n", *p_if_id);
            return PPA_EINVAL;
        }

        if_id = *p_if_id - g_start_ifid;
        ppa_lock_get(&g_directpath_port_lock);
        skb_list=__remove_directpath_dev_from_datapath(if_id);       
        ppa_lock_release(&g_directpath_port_lock);

#if defined(CONFIG_LTQ_PPA_DIRECTPATH_TX_QUEUE_SIZE) || defined(CONFIG_LTQ_PPA_DIRECTPATH_TX_QUEUE_PKTS)
        remove_directpath_queue(skb_list);
#endif
        ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"unregiter directpath ok\n");
        return PPA_SUCCESS;
    }
}

int32_t ppa_directpath_send(uint32_t rx_if_id, PPA_BUF *skb, int32_t len, uint32_t flags)
{
#if !defined(CONFIG_LTQ_PPA_API_DIRECTPATH_BRIDGING)
    uint8_t mac[PPA_ETH_ALEN] = {0};
    uint8_t netif_mac[PPA_ETH_ALEN] = {0};
#endif
    int32_t ret = PPA_SUCCESS;

#if defined(CONFIG_LTQ_PPA_DIRECTPATH_TX_IMQ)
    int32_t (*tmp_imq_queue)(PPA_BUF *skb, uint32_t portID) = ppa_hook_directpath_enqueue_to_imq_fn;
#endif        
    if( skb == NULL )
    {
        return PPA_EINVAL;
    }
    
    if( !ppa_is_init() ) 
    {
        PPA_SKB_FREE(skb);
        return PPA_EINVAL;
    }
    if(flags & 3) { //directlink function call
        return ppa_drv_directpath_send(rx_if_id, skb, len, flags);        
    }
    if( !ppa_drv_g_ppe_directpath_data ) 
    {
        PPA_SKB_FREE(skb);
        return PPA_EINVAL;
    }

    ppa_lock_get(&g_directpath_port_lock);
    
    if ( rx_if_id < g_start_ifid || rx_if_id >= g_end_ifid )
    {
        ret = PPA_EINVAL;
        goto __DIRETPATH_TX_EXIT;
    }

    if ( !(ppa_drv_g_ppe_directpath_data[rx_if_id - g_start_ifid].flags & PPE_DIRECTPATH_DATA_ENTRY_VALID) )
    {
        ret = PPA_EPERM;
        goto __DIRETPATH_TX_EXIT;
    }

#if !defined(CONFIG_LTQ_PPA_API_DIRECTPATH_BRIDGING)
    ppa_get_pkt_rx_dst_mac_addr(skb, mac);
    ppa_get_netif_hwaddr(ppa_drv_g_ppe_directpath_data[rx_if_id - g_start_ifid].netif, netif_mac, 0);
    if ( ppa_memcmp(mac, /* ppa_drv_g_ppe_directpath_data[rx_if_id - g_start_ifid].mac */ netif_mac, PPA_ETH_ALEN) != 0 )
    {
        //  bridge
        rx_if_id -= g_start_ifid;
        if ( (ppa_drv_g_ppe_directpath_data[rx_if_id].flags & PPE_DIRECTPATH_DATA_RX_ENABLE) )
        {
            ppa_drv_g_ppe_directpath_data[rx_if_id].callback.rx_fn(ppa_drv_g_ppe_directpath_data[rx_if_id].netif, NULL, skb, len);
            ret = LTQ_SUCCESS;
            goto __DIRETPATH_TX_EXIT;
        }
        else
        {
            ret = PPA_EINVAL;
            goto __DIRETPATH_TX_EXIT;
        }
    }
#endif


#if defined(CONFIG_LTQ_PPA_DIRECTPATH_TX_IMQ)
    if( ppa_directpath_imq_en_flag && tmp_imq_queue )
    {
         if( tmp_imq_queue(skb, rx_if_id) == 0 )
        { 
            ppa_lock_release(&g_directpath_port_lock);
            return 0;
        }
   }
#endif
   ret= ppa_drv_directpath_send(rx_if_id, skb, len, flags);
   ppa_lock_release(&g_directpath_port_lock);
   return ret;
   

__DIRETPATH_TX_EXIT:
   if(ret != PPA_SUCCESS && skb){
        PPA_SKB_FREE(skb);
   }

   ppa_lock_release(&g_directpath_port_lock);
   return ret;

}


#if defined(CONFIG_LTQ_PPA_DIRECTPATH_TX_IMQ)
int32_t ppa_directpath_reinject_from_imq(int32_t rx_if_id, PPA_BUF *skb, int32_t len, uint32_t flags)
{
#if !defined(CONFIG_LTQ_PPA_API_DIRECTPATH_BRIDGING)
    uint8_t mac[PPA_ETH_ALEN] = {0};
    uint8_t netif_mac[PPA_ETH_ALEN] = {0};
#endif
    int32_t ret = PPA_SUCCESS;

    if( skb == NULL )
    {
        return PPA_EINVAL;
    }
    
    if( !ppa_is_init() ) 
    {
        PPA_SKB_FREE(skb);
        return PPA_EINVAL;
    }
    if( !ppa_drv_g_ppe_directpath_data ) 
    {
        PPA_SKB_FREE(skb);
        return PPA_EINVAL;
    }

    ppa_lock_get(&g_directpath_port_lock);
    
    if ( rx_if_id < g_start_ifid || rx_if_id >= g_end_ifid )
    {
        ret = PPA_EINVAL;
        goto __DIRETPATH_TX_EXIT;
    }

    if ( !(ppa_drv_g_ppe_directpath_data[rx_if_id - g_start_ifid].flags & PPE_DIRECTPATH_DATA_ENTRY_VALID) )
    {
        ret = PPA_EPERM;
        goto __DIRETPATH_TX_EXIT;
    }

    ret = ppa_drv_directpath_send(rx_if_id, skb, len, flags);
    ppa_lock_release(&g_directpath_port_lock);
    return ret;
    

__DIRETPATH_TX_EXIT:
   if(ret != PPA_SUCCESS && skb){
        PPA_SKB_FREE(skb);
   }

   ppa_lock_release(&g_directpath_port_lock);
   return ret;

}
#endif

int32_t ppa_directpath_rx_stop(uint32_t if_id, uint32_t flags)
{
    int32_t ret;

    if( !ppa_is_init() ) return PPA_EINVAL;
    if( !ppa_drv_g_ppe_directpath_data ) return PPA_EINVAL;

    ppa_lock_get(&g_directpath_port_lock);
    if ( if_id >= g_start_ifid && if_id < g_end_ifid
        && (ppa_drv_g_ppe_directpath_data[if_id - g_start_ifid].flags & PPE_DIRECTPATH_DATA_ENTRY_VALID) )
    {
        ppa_drv_g_ppe_directpath_data[if_id - g_start_ifid].flags &= ~PPE_DIRECTPATH_DATA_RX_ENABLE;
       ret = ppa_drv_directpath_rx_stop(if_id, flags) == 0 ? PPA_SUCCESS : PPA_FAILURE;
    }
    else
        ret = PPA_EINVAL;
    ppa_lock_release(&g_directpath_port_lock);

    return ret;
}

int32_t ppa_directpath_rx_restart(uint32_t if_id, uint32_t flags)
{
    int32_t ret;

    if( !ppa_is_init() ) return PPA_EINVAL;
    if( !ppa_drv_g_ppe_directpath_data ) return PPA_EINVAL;

    ppa_lock_get(&g_directpath_port_lock);
    if ( if_id >= g_start_ifid && if_id < g_end_ifid
        && (ppa_drv_g_ppe_directpath_data[if_id - g_start_ifid].flags & PPE_DIRECTPATH_DATA_ENTRY_VALID) )
    {
        ppa_drv_g_ppe_directpath_data[if_id - g_start_ifid].flags |= PPE_DIRECTPATH_DATA_RX_ENABLE;
        ret = ppa_drv_directpath_rx_start(if_id, flags) == 0 ? PPA_SUCCESS : PPA_FAILURE;

    }
    else
        ret = PPA_EINVAL;
    ppa_lock_release(&g_directpath_port_lock);

    return ret;
}

PPA_NETIF *ppa_get_netif_for_ppa_ifid(uint32_t if_id)
{
    PPA_NETIF *ret = NULL;

    if( !ppa_is_init() ) return NULL;
    if( !ppa_drv_g_ppe_directpath_data ) return NULL;

    ppa_lock_get(&g_directpath_port_lock);
    if ( if_id >= g_start_ifid && if_id < g_end_ifid )
    {
        if_id -= g_start_ifid;
        if ( (ppa_drv_g_ppe_directpath_data[if_id].flags & PPE_DIRECTPATH_DATA_ENTRY_VALID) )
            ret = ppa_drv_g_ppe_directpath_data[if_id].netif;
    }
    ppa_lock_release(&g_directpath_port_lock);

    return ret;
}

int32_t ppa_get_ifid_for_netif(PPA_NETIF *netif)
{
    uint32_t if_id;

    if( !ppa_is_init() ) return PPA_EINVAL;
    if( !ppa_drv_g_ppe_directpath_data ) return PPA_EINVAL;

    if ( netif == NULL )
        return PPA_EINVAL;

    ppa_lock_get(&g_directpath_port_lock);
    for ( if_id = 0; if_id < g_end_ifid - g_start_ifid; if_id++ )
        if ( (ppa_drv_g_ppe_directpath_data[if_id].flags & PPE_DIRECTPATH_DATA_ENTRY_VALID)
            && ppa_is_netif_equal(netif, ppa_drv_g_ppe_directpath_data[if_id].netif) )
        {
            ppa_lock_release(&g_directpath_port_lock);
            return if_id + g_start_ifid;
        }
    ppa_lock_release(&g_directpath_port_lock);

    return PPA_FAILURE;
}

int32_t ppa_directpath_data_start_iteration(uint32_t *ppos, struct ppe_directpath_data **info)
{
    uint32_t l;

    ppa_lock_get(&g_directpath_port_lock);
	
    if( !ppa_is_init() ) return PPA_EINVAL;
    
    if ( g_start_ifid == ~0 )
    {
        *info = NULL;
        return PPA_FAILURE;
    }

    if( !ppa_drv_g_ppe_directpath_data ) return PPA_EINVAL;

    l = *ppos;
    if ( l + g_start_ifid < g_end_ifid )
    {
        ++*ppos;
        *info = ppa_drv_g_ppe_directpath_data + l;
        return PPA_SUCCESS;
    }
    else
    {
        *info = NULL;
        return PPA_FAILURE;
    }
}

int32_t ppa_directpath_data_iterate_next(uint32_t *ppos, struct ppe_directpath_data **info)
{
    if( !ppa_is_init() ) return PPA_EINVAL;
    if( !ppa_drv_g_ppe_directpath_data ) return PPA_EINVAL;

    if ( *ppos + g_start_ifid < g_end_ifid )
    {
        *info = ppa_drv_g_ppe_directpath_data + *ppos;
        ++*ppos;
        return PPA_SUCCESS;
    }
    else
    {
        *info = NULL;
        return PPA_FAILURE;
    }
}

void ppa_directpath_data_stop_iteration(void)
{
    ppa_lock_release(&g_directpath_port_lock);
}

void ppa_directpath_get_ifid_range(uint32_t *p_start_ifid, uint32_t *p_end_ifid)
{
    if ( p_start_ifid )
        *p_start_ifid = g_start_ifid;
    if ( p_end_ifid )
        *p_end_ifid = g_end_ifid;
}



/*
 * ####################################
 *           Init/Cleanup API
 * ####################################
 */

int32_t ppa_api_directpath_init(void)
{
    uint32_t i;
    uint32_t last_valid_ifid;
    uint32_t tmp_flags;
    PPE_IFINFO if_info;
    PPE_COUNT_CFG count={0};

    if ( !ppa_drv_g_ppe_directpath_data )
        return PPA_EINVAL;

    ppa_drv_get_number_of_phys_port(&count, 0);
    last_valid_ifid = count.num - 1;
    ppa_memset( &if_info, 0, sizeof(if_info) );
    for ( i = 0; i < count.num; i++ )
    {
        if_info.port = i;
        ppa_drv_get_phys_port_info(&if_info, 0);
        if ( (if_info.if_flags & (PPA_PHYS_PORT_FLAGS_VALID | PPA_PHYS_PORT_FLAGS_TYPE_MASK)) == (PPA_PHYS_PORT_FLAGS_VALID | PPA_PHYS_PORT_FLAGS_TYPE_EXT)
            && (if_info.if_flags  & (PPA_PHYS_PORT_FLAGS_EXT_CPU0 | PPA_PHYS_PORT_FLAGS_EXT_CPU1)) != 0 )
        {
            if ( g_start_ifid == ~0 )
                g_start_ifid = i;
            last_valid_ifid = i;
            tmp_flags = PPE_DIRECTPATH_ETH;
            if ( (if_info.if_flags  & PPA_PHYS_PORT_FLAGS_EXT_CPU0) )
                tmp_flags |= PPE_DIRECTPATH_CORE0;
            if ( (if_info.if_flags  & PPA_PHYS_PORT_FLAGS_EXT_CPU1) )
                tmp_flags |= PPE_DIRECTPATH_CORE1;
            if ( (if_info.if_flags & PPA_PHYS_PORT_FLAGS_MODE_LAN) )
                tmp_flags |= PPE_DIRECTPATH_LAN;
            if ( (if_info.if_flags & PPA_PHYS_PORT_FLAGS_MODE_WAN) )
                tmp_flags |= PPE_DIRECTPATH_WAN;
            ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT, "%s: if_info.if_flags = 0x%08x, direct_flag[%d]=0x%08x, tmp_flags=0x%08x\n", __FUNCTION__, if_info.if_flags, i - g_start_ifid, ppa_drv_g_ppe_directpath_data[i - g_start_ifid].flags, tmp_flags);
            ppa_drv_g_ppe_directpath_data[i - g_start_ifid].flags = tmp_flags;
        }
    }
    g_end_ifid = last_valid_ifid + 1;

    ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT, "directpath start id: %d, end id: %d\n", g_start_ifid, g_end_ifid);

    return PPA_SUCCESS;
}

void ppa_api_directpath_exit(void)
{
    uint32_t i;  
    PPA_BUF *skb_list = NULL;



    if ( g_end_ifid != ~0 )
    {
        if( ppa_drv_g_ppe_directpath_data )
        {
            ppa_lock_get(&g_directpath_port_lock);
            for ( i = 0; i < g_end_ifid - g_start_ifid; i++ )
            {
                if( ppa_drv_g_ppe_directpath_data[i].flags & PPE_DIRECTPATH_DATA_ENTRY_VALID) 
                {
                    skb_list=__remove_directpath_dev_from_datapath(i);      
#if defined(CONFIG_LTQ_PPA_DIRECTPATH_TX_QUEUE_SIZE) || defined(CONFIG_LTQ_PPA_DIRECTPATH_TX_QUEUE_PKTS)
                    remove_directpath_queue(skb_list);                    
#endif                    
                }
            }
            g_start_ifid = g_end_ifid = ~0;
            ppa_lock_release(&g_directpath_port_lock);
        }
    }

}

int32_t ppa_api_directpath_create(void)
{
    if ( ppa_lock_init(&g_directpath_port_lock) )
    {
        err("Failed in creating lock for directpath port.");
        return PPA_EIO;
    }

    return PPA_SUCCESS;
}

void ppa_api_directpath_destroy(void)
{
    ppa_lock_destroy(&g_directpath_port_lock);
}

EXPORT_SYMBOL(ppa_directpath_data_start_iteration);
EXPORT_SYMBOL(ppa_directpath_data_iterate_next);
EXPORT_SYMBOL(ppa_directpath_data_stop_iteration);
EXPORT_SYMBOL(ppa_directpath_get_ifid_range);
#if defined(LTQ_PPA_DIRECTPATH_TX_IMQ)
    EXPORT_SYMBOL(ppa_directpath_reinject_from_imq);
#endif
