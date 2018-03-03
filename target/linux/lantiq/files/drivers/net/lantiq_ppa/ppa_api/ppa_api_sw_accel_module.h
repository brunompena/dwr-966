
/*******************************************************************************
**
** FILE NAME    : ppa_api_sw_accel_module.h
** PROJECT      : PPA
** MODULES      : PPA API (Wrapper for Software Fastpath Implementation)
**
** DATE         : 13 Mar 2014
** AUTHOR       : Lantiq
** DESCRIPTION  : Function to bypass the linux stack for packets belonging to the PPA sessions which are not in PPE firmware.
** COPYRIGHT    :              Copyright (c) 2013
**                          Lantiq Deutschland GmbH
**                   Am Campeon 3; 85579 Neubiberg, Germany
**
**   For licensing information, see the file 'LICENSE' in the root folder of
**   this software module.
**
** HISTORY
** $Date        $Author                $Comment
*******************************************************************************/

/*! \file ppa_api_sw_accel_module.h
    \brief This file contains es.
           software fastpath wrapper function declarations
*/

extern signed long sw_fastpath_send(void *skb);
extern signed long get_sw_fastpath_status(unsigned long *f_enable, unsigned long flags);
extern signed long sw_fastpath_enable(unsigned long f_enable, unsigned long flags);

extern void sw_del_session(void *pitem);
extern signed long sw_add_session(void *skb, void *pitem);
extern signed long sw_update_session(void *skb, void *pitem, void *tx_ifinfo);

extern int32_t (*ppa_sw_fastpath_enable_hook)(uint32_t, uint32_t);
extern int32_t (*ppa_get_sw_fastpath_status_hook)(uint32_t *, uint32_t);
extern int32_t (*ppa_sw_fastpath_send_hook)(void *);

extern int32_t (*ppa_sw_add_session_hook)(PPA_BUF *skb, struct session_list_item *p_item);
extern int32_t (*ppa_sw_update_session_hook)(PPA_BUF *skb, struct session_list_item *p_item,struct netif_info *tx_ifinfo);
extern void (*ppa_sw_del_session_hook)(struct session_list_item *p_item);

