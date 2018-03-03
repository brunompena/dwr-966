obj-y += ppa_hook.o ppe_drv_wrapper.o ppa_stack_al.o ppa_api_tools.o
obj-$(CONFIG_LTQ_PPA_API) += ppa_api.o

ppa_api-objs = 


ppa_api-objs += ppa_api_mfe.o

ifeq ($(CONFIG_LTQ_PPA_QOS),y)
ppa_api-objs += ppa_api_qos.o
endif

ppa_api-objs += ppa_api_mib.o ppa_api_core.o ppa_api_misc.o ppa_api_netif.o 

ifeq ($(CONFIG_LTQ_PPA_HANDLE_CONNTRACK_SESSIONS),y)
ppa_api-objs += ppa_api_session_limit.o
endif

ppa_api-objs += ppa_api_session.o
ifeq ($(CONFIG_LTQ_PMCU),y)
   ppa_api-objs += ppa_api_pwm.o ppa_api_pwm_logic.o
endif

ifeq ($(CONFIG_LTQ_CPU_FREQ),y)
   ppa_api-objs += ppa_api_pwm.o ppa_api_pwm_logic.o
endif

ifeq ($(CONFIG_LTQ_PPA_API_DIRECTPATH),y)
   ppa_api-objs += ppa_api_directpath.o
endif

ifeq ($(CONFIG_LTQ_PPA_API_SW_FASTPATH),y)
   obj-m += ppa_api_sw_accel_mod.o
   ppa_api_sw_accel_mod-objs = swa_stack_al.o ppa_api_sw_accel.o ppa_api_sw_accel_module.o 
endif

ifeq ($(CONFIG_LTQ_PPA_API_PROC),y)
  ppa_api-objs += ppa_api_proc.o
endif
ifeq ($(CONFIG_LTQ_PPA_API_PROC),m)
  obj-$(CONFIG_LTQ_PPA_API_PROC) += ppa_api_proc.o
endif

