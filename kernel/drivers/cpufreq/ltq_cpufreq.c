/*
 *
 *			   Copyright (c) 2012
 *			Lantiq Deutschland GmbH
 *		 Am Campeon 3; 85579 Neubiberg, Germany
 *
 *  For licensing information, see the file 'LICENSE' in the root folder of
 *  this software module.
 */

 #ifdef CONFIG_LTQ_CPUFREQ_DEBUG
	#define DEBUG
#endif

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/version.h>
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,33))
	#include <linux/utsrelease.h>
#else
	#include <generated/utsrelease.h>
#endif
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/ctype.h>
#include <linux/smp.h>
#include <linux/init.h>
#include <linux/spinlock.h>
#include <linux/interrupt.h>
#include <linux/cpu.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/sysfs.h>
#include <linux/mutex.h>
#include <linux/delay.h>
#include <linux/of.h>
#include <linux/clk.h>
#include <linux/cpufreq.h>
#include <cpufreq/ltq_cpufreq.h>
#include <../arch/mips/lantiq/clk.h>
#include <../arch/mips/include/asm/idle.h>

enum ltq_cpufreq_req_id{
	/** no pending powerState request pending in the ringBuffer */
	LTQ_CPUFREQ_NO_PENDING_REQ		= 0,
	/** powerState request pending in the ringBuffer */
	LTQ_CPUFREQ_PENDING_REQ			= 1,
	/** unknown powerState request in buffer or buffer overflow */
	LTQ_CPUFREQ_PENDING_REQ_ERROR	= -1,
};

/**
	struct used in REQUEST ringBuffer to keep all relevant info's for one
	powerState request
*/
struct ltq_cpufreq_req_state{
	struct ltq_cpufreq_module_state	moduleState;
	enum ltq_cpufreq_req_id			reqId;
};


/*======================================================================*/
/* LOCAL FUNCTIONS PROTOTYPES						*/
/*======================================================================*/
unsigned int ltq_cpufreq_getfreq_khz(unsigned int cpu);
static int ltq_cpufreq_init (struct cpufreq_policy *policy);
static int ltq_cpufreq_verify (struct cpufreq_policy *policy);
static int ltq_cpufreq_target (struct cpufreq_policy *policy,
				unsigned int target_freq,
				unsigned int relation);
static int ltq_cpufreq_exit (struct cpufreq_policy *policy);
static int ltq_cpufreq_resume (struct cpufreq_policy *policy);
static struct ltq_cpufreq_req_state ltq_cpufreq_get_req( void );
static int ltq_cpufreq_put_req( struct ltq_cpufreq_req_state req );
static void ltq_cpufreq_process_req(struct work_struct *work);
static int ltq_cpucg_pwrFeatureSwitch(int pwrStateEna);
static int ltq_cpufreq_state_get(enum ltq_cpufreq_state *pmcuState);

/*======================================================================*/
/* WORKQUEUE DECLARATION						*/
/*======================================================================*/
static DECLARE_WORK(work_obj, ltq_cpufreq_process_req);

/*======================================================================*/
/* module DEFINES							*/
/*======================================================================*/
/* size of request ringbuffer */
#define LTQ_REQ_BUFFER_SIZE 5

/** \ingroup LTQ_CPUFREQ
\brief  The ltq_cpufreq_lock is used to protect all data structures
	accessed from Interrupt AND Process-Context.
*/
static DEFINE_SPINLOCK(ltq_cpufreq_lock);

/*======================================================================*/
/* EXTERNAL variable DECLARATIONS					*/
/*======================================================================*/
extern struct cpufreq_governor cpufreq_gov_performance;

/*======================================================================*/
/* Internal module global variable DECLARATIONS				*/
/*======================================================================*/
#ifdef CONFIG_LTQ_CPUFREQ_DEBUG
struct ltq_cpufreq_latency ltq_cpufreq_latency_g[3]=
{	/* max,  min   , cur, freq, comment */
	{ -1, 999999999, -1,   0,   "Not Defined"},
	{ -1, 999999999, -1,   0,   "Not Defined"},
	{ -1, 999999999, -1,   0,   "Not Defined"},
};
#endif

struct ltq_cpufreq_module_info cpuddr_feature_fss = {
	.name				= "CPU/DDR frequency scaling support",
	.pmcuModule			= LTQ_CPUFREQ_MODULE_CPU,
	.pmcuModuleNr			= 0,
	.powerFeatureStat		= 1,
	.ltq_cpufreq_state_get		= ltq_cpufreq_state_get,
	.ltq_cpufreq_pwr_feature_switch	= NULL,
};

struct ltq_cpufreq_module_info cpuwait_feature_cgs = {
	.name			="CPU clock gating support (sleep opcode)",
	.pmcuModule		= LTQ_CPUFREQ_MODULE_CPU,
	.pmcuModuleNr		= 0,
	.powerFeatureStat	= 1,
	.ltq_cpufreq_state_get	= NULL,
	.ltq_cpufreq_pwr_feature_switch	= ltq_cpucg_pwrFeatureSwitch,
};

/** \ingroup LQ_CPUFREQ
\brief  Container to save cpu_wait pointer
	Used to enable/disable the mips_core clock gating feature
*/
static void (*cpu_wait_sav)(void ); /* container to save mips_wait */

static struct ltq_cpufreq ltq_cpufreq_obj = {
	.list_head_module = LIST_HEAD_INIT(ltq_cpufreq_obj.list_head_module),
	.cpufreq_cur_state  = LTQ_CPUFREQ_PS_D0,
	.cpufreq_new_state  = LTQ_CPUFREQ_PS_D0,
	.cpu_scaling_rates = {	500000000, /* D0 */
				500000000, /* D1 */
				500000000, /* D2 */
				500000000, /* D3 */
				0xA}, /* eol */
	.ddr_scaling_rates = {	250000000, /* D0 */
				250000000, /* D1 */
				250000000, /* D2 */
				250000000, /* D3 */
				0xA}, /* eol */
	.ltq_state_change_control = 0,
};

static int ltq_state_change_enable = 1;
unsigned int transition_latency = 3000000; /* ns */

/** \ingroup LTQ_CPUFREQ
\brief  The powerState request ringBuffer collect the incoming power state
	requests. The ringBuffer has a size of LTQ_REQ_BUFFER_SIZE,
	and will continuously start from the beginning with each overflow.
*/
static struct ltq_cpufreq_req_state ltq_cpufreq_reqBuffer[LTQ_REQ_BUFFER_SIZE];

/** \ingroup LTQ_CPUFREQ
\brief  The reqGetIndex points to the next request entry to be processed.
*/
static unsigned int ltq_cpufreq_reqGetIndex = 0;

/** \ingroup LTQ_CPUFREQ
\brief  The reqPutIndex points to the next free entry inside the ringBuffer
			to place the next incoming request.
*/
static unsigned int ltq_cpufreq_reqPutIndex = 0;

/** \ingroup LTQ_CPUFREQ
\brief  The reqBufferSize is the ringBuffer watchdog to signalize a real
	overflow of the ringBuffer. A real overflow means that the
	reqbuffer contains already LTQ_REQ_BUFFER_SIZE request entries
	and a new incoming request can not be placed and will be discarded.
*/
static unsigned int ltq_cpufreq_reqBufferSize = 0;


/* module names; array correspond to enum enum ltq_cpufreq_module */
char* ltq_cpufreq_mod[LTQ_CPUFREQ_MODULE_ID_MAX+1] = {
	"CPU" ,
	"WLAN" ,
	"VE" ,
	"PPE" ,
	"SWITCH" ,
	"UART" ,
	"GPTC" ,
	"PCIE" ,
	"USB" ,
	"INVALID" ,
};

/* CPUFREQ specific power state translation */
const char* ltq_cpufreq_pst[15][15] = {
	{"UNDEF", "UNDEF"},
	{"D0", "performance"},
	{"D1", "ondemand"},
	{"D2", "ondemand"},
	{"D3", "ondemand"},
	{"D0D3", "ondemand"},
	{"BOOST", "ondemand"}
};

/** \ingroup LTQ_CPUFREQ
\brief  hold the current and previous frequency of the CPU
	- cpu  = 0 only one CPU is supported
	- old  = previous frequency
	- new  = current CPU frequency
	- flags= None
*/
static struct cpufreq_freqs ltq_freqs={0,0,0,0};

struct cpufreq_frequency_table ltq_freq_tab[]={
	{.index = LTQ_CPUFREQ_PS_D0, .frequency = 0,},/*D0*/
	{.index = LTQ_CPUFREQ_PS_D1, .frequency = 0,},/*D1*/
	{.index = LTQ_CPUFREQ_PS_D2, .frequency = 0,},/*D2*/
	{.index = LTQ_CPUFREQ_PS_D3, .frequency = 0,},/*D3*/
	{.index = 0, .frequency = CPUFREQ_TABLE_END,}
};


static struct freq_attr *ltq_freq_attr[] = {
			&cpufreq_freq_attr_scaling_available_freqs,
			NULL,
};

struct cpufreq_driver ltq_cpufreq_driver={

	.name		= "ltq_cpufreq_drv\n",
	.owner		= THIS_MODULE,
	.init		= ltq_cpufreq_init,
	.flags		= 0,
	.verify		= ltq_cpufreq_verify,
	.setpolicy	= NULL,
	.target		= ltq_cpufreq_target,
	.get		= ltq_cpufreq_getfreq_khz,
	.getavg		= NULL,
	.exit		= ltq_cpufreq_exit,
	.suspend	= NULL,
	.resume		= ltq_cpufreq_resume,
	.attr		= ltq_freq_attr,
};

static int ltq_cpufreq_state_set(enum ltq_cpufreq_state ps)
{
	int retval = 0;
	unsigned long* cpu_freq;
	unsigned long freq;
	struct clk *clk = clk_get_sys("cpu", "cpu");

	if(clk == NULL){
		pr_err("CPU clock structure not found\n");
		return -1;
	}
	cpu_freq = ltq_cpufreq_obj.cpu_scaling_rates;
	if(cpu_freq == NULL){
		return -1;
	}
	switch(ps){
		case LTQ_CPUFREQ_PS_D0:
		{
			freq = *(cpu_freq +
				(LTQ_CPUFREQ_PS_D0 - LTQ_CPUFREQ_PS_D0));
			ltq_cpufreq_obj.cpufreq_new_state = LTQ_CPUFREQ_PS_D0;
			retval = clk_set_rate(clk,freq);
			if(retval == 0){
				ltq_cpufreq_obj.cpufreq_cur_state = 
					ltq_cpufreq_obj.cpufreq_new_state;
			}
			break ;
		}
		case LTQ_CPUFREQ_PS_D1:
		{
			freq = *(cpu_freq +
				(LTQ_CPUFREQ_PS_D1 - LTQ_CPUFREQ_PS_D0));
			ltq_cpufreq_obj.cpufreq_new_state = LTQ_CPUFREQ_PS_D1;
			retval = clk_set_rate(clk,freq);
			if(retval == 0){
				ltq_cpufreq_obj.cpufreq_cur_state = 
					ltq_cpufreq_obj.cpufreq_new_state;
			}
			break;
		}
		case LTQ_CPUFREQ_PS_D2:
		{
			freq = *(cpu_freq +
				(LTQ_CPUFREQ_PS_D2 - LTQ_CPUFREQ_PS_D0));
			ltq_cpufreq_obj.cpufreq_new_state = LTQ_CPUFREQ_PS_D2;
			retval = clk_set_rate(clk,freq);
			if(retval == 0){
				ltq_cpufreq_obj.cpufreq_cur_state = 
					ltq_cpufreq_obj.cpufreq_new_state;
			}
			break;
		}
		case LTQ_CPUFREQ_PS_D3:
		{
			freq = *(cpu_freq +
				(LTQ_CPUFREQ_PS_D3 - LTQ_CPUFREQ_PS_D0));
			ltq_cpufreq_obj.cpufreq_new_state = LTQ_CPUFREQ_PS_D3;
			retval = clk_set_rate(clk,freq);
			if(retval == 0){
				ltq_cpufreq_obj.cpufreq_cur_state = 
					ltq_cpufreq_obj.cpufreq_new_state;
			}
			break;
		}
		default:
		{
			break;
		}
	}
	return retval;
}


/**
    Callback function registered at the CPUFREQ.
    Enable/Disable of the mips_core clock gating (mips wait instruction).

	\param[in]  pwrStateEna
		- 1 enabled
		- 0 disabled

	\return Returns value as follows:
		- 0: if successful
		- < 0: in case of an error
*/
static int ltq_cpucg_pwrFeatureSwitch(int pwrStateEna)
{
	if (pwrStateEna == 1) {
		cpu_wait = cpu_wait_sav;
		cpuwait_feature_cgs.powerFeatureStat = 1;
		return 0;
	}
	if (pwrStateEna == 0) {
		cpu_wait = 0;
		cpuwait_feature_cgs.powerFeatureStat = 0;
		return 0;
	}
	return -1;
}

/**
	Callback function registered at the PMCU.
	This function is called by the PMCU to get the current power state
	status of the cpu frequency driver.

	\param[out]   pmcuState  current power state

	\return Returns value as follows:
		- IFX_PMCU_RETURN_SUCCESS: if successful
		- IFX_PMCU_RETURN_ERROR: in case of an error
*/
static int ltq_cpufreq_state_get(enum ltq_cpufreq_state *pmcuState)
{
	*pmcuState = ltq_cpufreq_obj.cpufreq_cur_state;
	return 0;
}

struct ltq_cpufreq* ltq_cpufreq_get(void){
	return &ltq_cpufreq_obj;
}
EXPORT_SYMBOL(ltq_cpufreq_get);

int ltq_cpufreq_state_change_disable(void) {

	ltq_state_change_enable = 0;
	ltq_cpufreq_state_req(LTQ_CPUFREQ_MODULE_CPU, 0, LTQ_CPUFREQ_PS_D0);
	return 0;
}

int ltq_cpufreq_state_change_enable(void) {

	ltq_state_change_enable = 1;
	ltq_cpufreq_state_req(LTQ_CPUFREQ_MODULE_CPU, 0, LTQ_CPUFREQ_PS_D3);
	return 0;
}

static void ltq_cpufreq_process_req(struct work_struct *work)
{
	struct cpufreq_policy policy_struct;
	struct cpufreq_policy *policy = &policy_struct;
	struct ltq_cpufreq_req_state reqState;

	pr_debug("%s is called\n",__FUNCTION__);

	while (ltq_cpufreq_reqBufferSize > 0) {
		reqState = ltq_cpufreq_get_req();
		if (reqState.reqId == LTQ_CPUFREQ_PENDING_REQ_ERROR) {
			pr_err("CPUFREQ Request Buffer underflow\n");
			return;
		}
		if (reqState.reqId == LTQ_CPUFREQ_NO_PENDING_REQ) {
			pr_err("No valid CPUFREQ Request in buffer, "
					"but workqueue is scheduled!!!!!\n");
			return;
		}

	if (!ltq_state_change_enable){
		pr_debug("Frequency down scaling disabled. Force "
						"PERFORMANCE governor\n");
		reqState.moduleState.pmcuState = LTQ_CPUFREQ_PS_D0;
	}
		policy->cpu = 0; /* we have only one CPU */
		policy = cpufreq_cpu_get(policy->cpu);
		if (policy){
			if (lock_policy_rwsem_write(policy->cpu) < 0){
				goto fail;
			}
			policy->cpuinfo.transition_latency =transition_latency;
			store_scaling_governor(policy,
			ltq_cpufreq_pst[reqState.moduleState.pmcuState][1],
		sizeof(ltq_cpufreq_pst[reqState.moduleState.pmcuState][1]));
			unlock_policy_rwsem_write(policy->cpu);
		fail:
			cpufreq_cpu_put(policy);
		}
	}
	return;
}


int ltq_cpufreq_state_req (enum ltq_cpufreq_module module,
				unsigned char moduleNr,
				enum ltq_cpufreq_state newState)
{
	struct cpufreq_policy policy_struct;
	struct cpufreq_policy *policy = &policy_struct;
	struct ltq_cpufreq_req_state reqState;

	pr_debug("%s is called: Module=%s, ModuleNr=%d, State=%s\n",
			__FUNCTION__,ltq_cpufreq_mod[module],moduleNr,
				ltq_cpufreq_pst[newState][0]);
	pr_debug("-%s- module request new governor -%s-\n",
			ltq_cpufreq_mod[module],ltq_cpufreq_pst[newState][1]);
	/* check if the acception of the powerstate request is enabled */
	if (ltq_cpufreq_obj.ltq_state_change_control == 0) {
		pr_debug("The PowerStateChange from driver level is "
				  "disabled. Each request will be rejected\n");
		return LTQ_CPUFREQ_RETURN_DENIED;
	}

	/*check if the new request is already active*/
	policy->cpu = 0; /* we have only one CPU */
	policy = cpufreq_cpu_get(policy->cpu);
	if (policy){
		if(strcmp(policy->governor->name,
					ltq_cpufreq_pst[newState][1]) == 0){
			return LTQ_CPUFREQ_RETURN_DENIED;
		}
	}

	reqState.moduleState.pmcuModule = module;
	reqState.moduleState.pmcuModuleNr = moduleNr;
	reqState.moduleState.pmcuState = newState;
	reqState.reqId = LTQ_CPUFREQ_NO_PENDING_REQ;

	/* put new powerState request into request buffer */
	if (ltq_cpufreq_put_req(reqState) < 0) {
		pr_err("CPUFREQ RequestBuffer overflow !!!\n");
	}

#ifdef CONFIG_CPU_FREQ_GOV_ONDEMAND
	if (system_wq == NULL) {
		pr_debug("No workqueue available\n");
		return -EFAULT;
	}
	/* feed the kondemand_wq workqueue.
	   fetch powerState request from requestBuffer by the wq */
	if ( !queue_work(system_wq, &work_obj) ){
		pr_debug("workqueue successfully loaded\n");
	}
#endif

	return 0;
}
EXPORT_SYMBOL(ltq_cpufreq_state_req);

static int ltq_cpufreq_put_req( struct ltq_cpufreq_req_state req )
{
	int i;
	unsigned long iflags;

	pr_debug("%s is called\n",__FUNCTION__);
	if ( ltq_cpufreq_reqBufferSize >= LTQ_REQ_BUFFER_SIZE ) {
		return -1;
	}

	spin_lock_irqsave(&ltq_cpufreq_lock, iflags);

	/* first check if there is already a request for this module pending.
	   If this is the case, reject the actual one. */
	for (i=0;i<LTQ_REQ_BUFFER_SIZE;i++) {
		if (ltq_cpufreq_reqBuffer[i].reqId ==
						LTQ_CPUFREQ_NO_PENDING_REQ) {
			continue;
		}
		if ((ltq_cpufreq_reqBuffer[i].moduleState.pmcuModule ==
						req.moduleState.pmcuModule) &&
			(ltq_cpufreq_reqBuffer[i].moduleState.pmcuModuleNr ==
					req.moduleState.pmcuModuleNr) &&
			(ltq_cpufreq_reqBuffer[i].moduleState.pmcuState ==
						req.moduleState.pmcuState) )
		{
			spin_unlock_irqrestore(&ltq_cpufreq_lock, iflags);
			pr_debug("%s detect's multiple requests from same "
				 "module. Discard request.\n",__FUNCTION__);
			return 0;
		}
	}

	req.reqId = LTQ_CPUFREQ_PENDING_REQ;
	memcpy(&ltq_cpufreq_reqBuffer[ltq_cpufreq_reqPutIndex],
							&req, sizeof(req));
	ltq_cpufreq_reqPutIndex++;
	if ( ltq_cpufreq_reqPutIndex >= LTQ_REQ_BUFFER_SIZE ) {
		ltq_cpufreq_reqPutIndex = 0;
	}
	ltq_cpufreq_reqBufferSize++;
	spin_unlock_irqrestore(&ltq_cpufreq_lock, iflags);
	return 0;
}

static struct ltq_cpufreq_req_state ltq_cpufreq_get_req( void )
{
	struct ltq_cpufreq_req_state req;

	pr_debug("%s is called\n",__FUNCTION__);
	req.reqId = LTQ_CPUFREQ_PENDING_REQ_ERROR;

	if ( !ltq_cpufreq_reqBufferSize ) {
		return req;
	}
	ltq_cpufreq_reqBufferSize--;
	memcpy(&req, &ltq_cpufreq_reqBuffer[ltq_cpufreq_reqGetIndex],
								sizeof(req));
	ltq_cpufreq_reqBuffer[ ltq_cpufreq_reqGetIndex ].reqId =
						LTQ_CPUFREQ_NO_PENDING_REQ;
	ltq_cpufreq_reqGetIndex++;
	if ( ltq_cpufreq_reqGetIndex >= LTQ_REQ_BUFFER_SIZE ) {
		ltq_cpufreq_reqGetIndex = 0;
	}
	return req;
}

/* convert given frequency in kHz to corresponding power state */
enum ltq_cpufreq_state ltq_cpufreq_get_ps_from_khz(unsigned int freqkhz)
{
	struct cpufreq_frequency_table* ltq_freq_tab_p;
	ltq_freq_tab_p = ltq_freq_tab;

	while(ltq_freq_tab_p->frequency != CPUFREQ_TABLE_END) {
		if(ltq_freq_tab_p->frequency == freqkhz) {
			return (enum ltq_cpufreq_state)ltq_freq_tab_p->index;
		}
		ltq_freq_tab_p++;
	}
	return (enum ltq_cpufreq_state)ltq_freq_tab_p->index;
}
EXPORT_SYMBOL(ltq_cpufreq_get_ps_from_khz);


unsigned int ltq_cpufreq_getfreq_khz(unsigned int cpu)
{
	struct clk *clk = clk_get_sys("cpu", "cpu");

	if(clk == NULL){
		pr_err("CPU clock structure not found\n");
		return -1;
	}

	return (clk_get_rate(clk) / 1000);
}
EXPORT_SYMBOL(ltq_cpufreq_getfreq_khz);

struct cpufreq_policy *policy_sav;

static int ltq_cpufreq_init (struct cpufreq_policy *policy)
{
	struct clk *clk = clk_get_sys("cpu", "cpu");

	pr_debug("%s is called\n",__FUNCTION__);
	if(clk == NULL){
		pr_err("CPU clock structure not found\n");
		return -EINVAL;
	}
	policy_sav = policy;
	cpumask_copy(policy->cpus, cpu_possible_mask);
	policy->cpuinfo.min_freq = ltq_cpufreq_obj.cpu_scaling_rates[3]/1000;
	policy->cpuinfo.max_freq = ltq_cpufreq_obj.cpu_scaling_rates[0]/1000;
	policy->cpuinfo.transition_latency = transition_latency;
	policy->cur		 = (clk_get_rate(clk) / 1000);
	policy->min		 = ltq_cpufreq_obj.cpu_scaling_rates[3]/1000;
	policy->max		 = ltq_cpufreq_obj.cpu_scaling_rates[0]/1000;
	policy->policy		 = 0;
	policy->governor	 = &cpufreq_gov_performance;

	cpufreq_frequency_table_cpuinfo(policy,ltq_freq_tab);
	cpufreq_frequency_table_get_attr(ltq_freq_tab, policy->cpu);
	ltq_freqs.new = policy->cur;
	return 0;
}

static int ltq_cpufreq_verify (struct cpufreq_policy *policy)
{
	pr_debug("%s is called\n",__FUNCTION__);
	cpufreq_frequency_table_verify(policy,ltq_freq_tab);
	return 0;
}

static int ltq_cpufreq_target ( struct cpufreq_policy *policy,
				unsigned int target_freq,
				unsigned int relation)
{
	static struct cpufreq_policy *policy_sav;
	static unsigned int target_freq_sav;
	static unsigned int tab_index,ret=0;
	int freqOk = -1;
	struct cpufreq_freqs freqs;
#ifdef CONFIG_LTQ_CPUFREQ_DEBUG
	unsigned int count_start;
#endif
	struct clk *clk = clk_get_sys("cpu", "cpu");

	pr_debug("%s is called\n",__FUNCTION__);
	if(clk == NULL){
		pr_err("CPU clock structure not found\n");
		return -EINVAL;
	}
	policy_sav = policy;
	target_freq_sav = target_freq;

	if (!ltq_cpufreq_obj.ltq_state_change_control){
		pr_debug("Frequency down scaling disabled.\n");
		return -EINVAL;
	}
	if (cpufreq_frequency_table_target(policy, ltq_freq_tab,
		 target_freq, relation, &tab_index)){
		return -EINVAL;
	}
	freqs.new = ltq_freq_tab[tab_index].frequency;
	freqs.old = (clk_get_rate(clk) / 1000); /* get kHz */
	freqs.cpu = policy->cpu;
	/* interrupt the process here if frequency doesn't change */
	if (freqs.new == freqs.old){
		return 0; /* nothing to do */
	}

	pr_debug("cpufreq_notify_transition, CPUFREQ_PRECHANGE\n");
#ifdef CONFIG_LTQ_CPUFREQ_DEBUG
	count_start = ltq_count0_read();
#endif
	ret = cpufreq_notify_transition(policy, &freqs, CPUFREQ_PRECHANGE);
#ifdef CONFIG_LTQ_CPUFREQ_DEBUG
	ltq_count0_diff(count_start,ltq_count0_read(),
						"prechange notification ",0);
#endif
	if((ret & NOTIFY_STOP_MASK) == NOTIFY_STOP_MASK) {
		ret = (ret >> 4) & 0x1F; /*mask module id*/
		pr_debug("Frequency scaling was denied by module %s\n",
					ltq_cpufreq_mod[ret]);
	}else{

#ifdef CONFIG_LTQ_CPUFREQ_DEBUG
		count_start = ltq_count0_read();
#endif
		if (ltq_cpufreq_state_set(ltq_freq_tab[tab_index].index)){
			freqOk = -1;
		}else{
			freqOk = 1;
		}
#ifdef CONFIG_LTQ_CPUFREQ_DEBUG
		ltq_count0_diff(count_start, ltq_count0_read(),
						"change clock rate      ",1);
#endif
	}

	if(freqOk < 0){
	/* if frequency change is denied, call post processing with new=old */
		freqs.new = freqs.old;
	}
	pr_debug("cpufreq_notify_transition, CPUFREQ_POSTCHANGE\n");
#ifdef CONFIG_LTQ_CPUFREQ_DEBUG
	count_start = ltq_count0_read();
#endif
	cpufreq_notify_transition(policy, &freqs, CPUFREQ_POSTCHANGE);
#ifdef CONFIG_LTQ_CPUFREQ_DEBUG
	ltq_count0_diff(count_start,ltq_count0_read(),
						"postchange notification",2);
#endif
	/* copy loops_per_jiffy to cpu_data */
	//cpu_data[smp_processor_id()].udelay_val = loops_per_jiffy;
	return 0;
}

static int ltq_cpufreq_exit (struct cpufreq_policy *policy)
{
	pr_debug("%s is called\n",__FUNCTION__);
	return 0;
}

static int ltq_cpufreq_resume (struct cpufreq_policy *policy)
{
	pr_debug("%s is called\n",__FUNCTION__);
	return 0;
}


#ifdef CONFIG_LTQ_CPUFREQ_DEBUG
/* time measurement routines for debug purpose only */
#define COUNT0_MAX 0xFFFFFFFF

static unsigned int cpu_freq_MHz;

int ltq_count0_diff(u32 count_start, u32 count_end, char* ident, int index)
{
	u64 diff,time64;
	int time;

	if (count_start >= count_end){
		 diff = (COUNT0_MAX - count_start) + count_end;
	}else{
		diff = count_end - count_start;
	}
	time64 = 2 * diff; /* count runs with cpu/2 */
	do_div(time64, cpu_freq_MHz);

	/* check for realistic time */
	if (time64 > 0x7FFFFFFF){
		pr_debug("timer overflow in freq change measurement\n");
		time = 0x7FFFFFFF;
	}
	time = (int)time64;

	switch(index){
		case 0:/*PRE*/
		{
			ltq_cpufreq_latency_g[0].comment = ident;
			ltq_cpufreq_latency_g[0].cpu_frequency = cpu_freq_MHz;
			if(time < ltq_cpufreq_latency_g[0].min_time){
				ltq_cpufreq_latency_g[0].min_time = time;
			}
			ltq_cpufreq_latency_g[0].cur_time = time;
			if(time > ltq_cpufreq_latency_g[0].max_time){
				ltq_cpufreq_latency_g[0].max_time = time;
			}
			break;
		}
		case 1:/*STATE CHANGE*/
		{
			ltq_cpufreq_latency_g[1].comment = ident;
			ltq_cpufreq_latency_g[1].cpu_frequency = cpu_freq_MHz;
			if(time < ltq_cpufreq_latency_g[1].min_time){
				ltq_cpufreq_latency_g[1].min_time = time;
			}
			ltq_cpufreq_latency_g[1].cur_time = time;
			if(time > ltq_cpufreq_latency_g[1].max_time){
				ltq_cpufreq_latency_g[1].max_time = time;
			}
			break;
		}
		case 2:/*POST*/
		{
			ltq_cpufreq_latency_g[2].comment = ident;
			ltq_cpufreq_latency_g[2].cpu_frequency = cpu_freq_MHz;
			if(time < ltq_cpufreq_latency_g[2].min_time){
				ltq_cpufreq_latency_g[2].min_time = time;
			}
			ltq_cpufreq_latency_g[2].cur_time = time;
			if(time > ltq_cpufreq_latency_g[2].max_time){
				ltq_cpufreq_latency_g[2].max_time = time;
			}
			break;
		}
		default: break;
	}

	return time;
}
EXPORT_SYMBOL(ltq_count0_diff);

u32 ltq_count0_read(void)
{
	struct clk *clk = clk_get_sys("cpu", "cpu");

	if(clk == NULL){
		pr_err("CPU clock structure not found\n");
		return -1;
	}
	cpu_freq_MHz = clk_get_rate(clk)/1000000;
	return read_c0_count();
}
EXPORT_SYMBOL(ltq_count0_read);

#endif /* CONFIG_LTQ_CPUFREQ_DEBUG */

/*======================================================================*/
/* START: SYSFS HELPER FUNCTION CALLED FROM LINUX CPUFREQ		*/
/*======================================================================*/
static int ltq_cpufreq_read_control(char *buf)
{
	int len = 0;

	len += sprintf(buf+len, "  ltq_state_change_control = %s\n\n",
		(ltq_cpufreq_obj.ltq_state_change_control==1)?"ON":"OFF");
	return len;
}

static int ltq_cpufreq_write_control(const char *buf, size_t count)
{
	buf=strsep((char **)&buf,"/");
	if(strcmp(buf, "on") == 0) {
		ltq_cpufreq_obj.ltq_state_change_control = 1;
	}else{
		ltq_cpufreq_obj.ltq_state_change_control = 0;
	}
	return count;
}

static int ltq_cpufreq_read_module_info(char *buf)
{
	int len = 0;
	struct ltq_cpufreq_module_info* cur_list_pos;
	enum ltq_cpufreq_state pState;

	len += sprintf(buf+len, "\n\n");
	len += sprintf(buf+len, "  Example for WRITE:\n");
	len += sprintf(buf+len, "    echo on/ > control\n");
	len += sprintf(buf+len, "    echo CPU clock gating support/off >" 
							"ltq_module_info\n\n");
	len += sprintf(buf+len, "  ltq_state_change_control = %s\n\n",
		(ltq_cpufreq_obj.ltq_state_change_control==1)?"ON":"OFF");
	len += sprintf(buf+len, "  Legend:\n");
	len += sprintf(buf+len, "  D0 = OnState, D3 = IdleState, D1,D2 = "
							"intermediate.\n");
	len += sprintf(buf+len, "  PS = current Power State of the module\n");
	len += sprintf(buf+len, "  SubNo = Module instance\n");
	len += sprintf(buf+len, "  Power Feature On/Off example:\n");
	len += sprintf(buf+len, "  echo PPE clock gating support/off > "
							"ltq_module_info\n\n");
	list_for_each_entry(cur_list_pos,
				&ltq_cpufreq_obj.list_head_module,list) {
		len += sprintf(buf+len, "  Comment = %s\n",cur_list_pos->name);
		if (cur_list_pos->pmcuModule < LTQ_CPUFREQ_MODULE_ID_MAX){
			len += sprintf(buf+len, "  Mod = %s\t",
				   ltq_cpufreq_mod[cur_list_pos->pmcuModule]);
			if(cur_list_pos->pmcuModuleNr != 0){
				len += sprintf(buf+len, "  SubNo = %d",
				cur_list_pos->pmcuModuleNr);
			}
		}
		if(cur_list_pos->ltq_cpufreq_state_get != NULL){
			cur_list_pos->ltq_cpufreq_state_get(&pState);
			len += sprintf(buf+len, "\tPS = %s\t",
						ltq_cpufreq_pst[pState][0]);
		}
		if ((cur_list_pos->powerFeatureStat == 1)
			|| (cur_list_pos->powerFeatureStat == 0)){
			len += sprintf(buf+len, "\tPowerFeature = %s\n\n",
			(cur_list_pos->powerFeatureStat == 1 ? "ENABLED" :
								"DISABLED"));
		}else{
			len += sprintf(buf+len, "\n\n");
		}
	}
	len += sprintf(buf+len, "\n\n");
	return len;
}

int ltq_cpufreq_write_module_info(const char *buf, size_t count)
{
	int par1 = 0;
	char* s = (char*)buf;
	char* p0;
	char* p1;
	struct ltq_cpufreq_module_info* cur_list_pos;

	p0=strsep(&s,"/");
	p1=strsep(&s,"/");
	*(p1 + strlen(p1) - 1) = '\0'; /* remove LF */
	if(strcmp(p1, "on") == 0) {
		par1 = 1;
	}

	list_for_each_entry(cur_list_pos,
				&ltq_cpufreq_obj.list_head_module,list) {
		if(strcmp(cur_list_pos->name, p0) == 0) {
			//pr_debug("power feature found, %s\n",p0);
			if(cur_list_pos->ltq_cpufreq_pwr_feature_switch!=NULL){
				if(cur_list_pos->
				ltq_cpufreq_pwr_feature_switch(par1) >= 0){
				cur_list_pos->powerFeatureStat = par1;
				pr_debug("power feature set, %s, to %d\n",
				p0,par1);
				}
			}
		}
	}
	return count;
}

#ifdef CONFIG_LTQ_CPUFREQ_DEBUG
static int ltq_cpufreq_read_freqchangelatency(char *buf)
{
	int len = 0;

	len += sprintf(buf+len, "\n\n");
	len += sprintf(buf+len, "%s: cur_time measured = %d[us], @%dHz,\n",
				ltq_cpufreq_latency_g[0].comment,
				ltq_cpufreq_latency_g[0].cur_time,
				ltq_cpufreq_latency_g[0].cpu_frequency);
	len += sprintf(buf+len, "%s: min_time measured = %d[us]\n",
				ltq_cpufreq_latency_g[0].comment,
				ltq_cpufreq_latency_g[0].min_time);
	len += sprintf(buf+len, "%s: max_time measured = %d[us]\n",
				ltq_cpufreq_latency_g[0].comment,
				ltq_cpufreq_latency_g[0].max_time);
	len += sprintf(buf+len, "%s: cur_time measured = %d[us], @%dHz,\n",
				ltq_cpufreq_latency_g[1].comment,
				ltq_cpufreq_latency_g[1].cur_time,
				ltq_cpufreq_latency_g[1].cpu_frequency);
	len += sprintf(buf+len, "%s: min_time measured = %d[us]\n",
				ltq_cpufreq_latency_g[1].comment,
				ltq_cpufreq_latency_g[1].min_time);
	len += sprintf(buf+len, "%s: max_time measured = %d[us]\n",
				ltq_cpufreq_latency_g[1].comment,
				ltq_cpufreq_latency_g[1].max_time);
	len += sprintf(buf+len, "%s: cur_time measured = %d[us], @%dHz,\n",
				ltq_cpufreq_latency_g[2].comment,
				ltq_cpufreq_latency_g[2].cur_time,
				ltq_cpufreq_latency_g[2].cpu_frequency);
	len += sprintf(buf+len, "%s: min_time measured = %d[us]\n",
				ltq_cpufreq_latency_g[2].comment,
				ltq_cpufreq_latency_g[2].min_time);
	len += sprintf(buf+len, "%s: max_time measured = %d[us]\n",
				ltq_cpufreq_latency_g[2].comment,
				ltq_cpufreq_latency_g[2].max_time);

	len += sprintf(buf+len, "\n\n");
	return len;
}

static int ltq_cpufreq_write_freqchangelatency(const char *buf,
						unsigned long count)
{
	int i;
	/* just reset the freq change latency buffer */
	for(i=0;i<3;i++){
		ltq_cpufreq_latency_g[i].max_time = -1;
		ltq_cpufreq_latency_g[i].min_time = 999999999;
		ltq_cpufreq_latency_g[i].cur_time = -1;
		ltq_cpufreq_latency_g[i].cpu_frequency = 0;
		ltq_cpufreq_latency_g[i].comment  = "Not Defined";
	}
	return count;
}

/* test function to check the correct setting for loops_per_jiffy */
static int ltq_cpufreq_read_udelay1000us(char *buf)
{
	int len = 0;
	u32 count_start;
	int time;

	count_start = ltq_count0_read();
	udelay(1000); /* 1000us */
	time = ltq_count0_diff(count_start, ltq_count0_read(), NULL, 0xFF);
	len += sprintf(buf+len, "\n\n");
	len += sprintf(buf+len, "This is a udelay/jiffies test to prove that"
				"the kernel clock settings are correct\n\n");
	len += sprintf(buf+len, "udelay measured = %d[us] for udelay"
						"set to 1000[us]\n", time);
	len += sprintf(buf+len, "\n\n");
	return len;
}

static ssize_t show_ltq_freqchangelatency(struct cpufreq_policy *policy,
						char *buf)
{
	return ltq_cpufreq_read_freqchangelatency(buf);
}

static ssize_t store_ltq_freqchangelatency(struct cpufreq_policy *policy,
					const char *buf, size_t count)
{
	return ltq_cpufreq_write_freqchangelatency(buf, count);
}

static ssize_t show_ltq_udelay1000us(struct cpufreq_policy *policy, char *buf)
{
	return ltq_cpufreq_read_udelay1000us(buf);
}

cpufreq_freq_attr_rw(ltq_freqchangelatency);
cpufreq_freq_attr_ro(ltq_udelay1000us);
#endif /* CONFIG_LTQ_CPUFREQ_DEBUG */

static ssize_t show_ltq_moduleinfo(struct cpufreq_policy *policy, char *buf)
{
	return ltq_cpufreq_read_module_info(buf);
}

static ssize_t store_ltq_moduleinfo(struct cpufreq_policy *policy,
					const char *buf, size_t count)
{
	return ltq_cpufreq_write_module_info(buf, count);
}

static ssize_t show_ltq_control(struct cpufreq_policy *policy, char *buf)
{
	return ltq_cpufreq_read_control(buf);
}

static ssize_t store_ltq_control(struct cpufreq_policy *policy,
					const char *buf, size_t count)
{
	return ltq_cpufreq_write_control(buf, count);
}

cpufreq_freq_attr_rw(ltq_moduleinfo);
cpufreq_freq_attr_rw(ltq_control);

static struct attribute *default_attrs[] = {
	&ltq_moduleinfo.attr,
	&ltq_control.attr,
#ifdef CONFIG_LTQ_CPUFREQ_DEBUG
	&ltq_freqchangelatency.attr,
	&ltq_udelay1000us.attr,
#endif
	NULL
};
static struct attribute_group ltq_stats_attr_group = {
	.attrs = default_attrs,
	.name = "ltq_stats"
};

static int ltq_cpufreq_stats_create(struct cpufreq_policy *policy)
{
	unsigned int ret = 0;
	struct cpufreq_policy *data;

	data = cpufreq_cpu_get(policy->cpu);
	if (data == NULL) {
		ret = -EINVAL;
		return ret;
	}
	ret = sysfs_create_group(&data->kobj, &ltq_stats_attr_group);
	cpufreq_cpu_put(data);
	return ret;
}
/*======================================================================*/
/* END: SYSFS HELPER FUNCTION CALLED FROM LANTIQ CPUFREQ		*/
/*======================================================================*/


static void get_frequencies(const u32 *addr, unsigned long *freqs, int len)
{
	int i;

	if(len > 4*sizeof(u32)){
		len = 4*sizeof(u32); /* we allow currently only 4 u32 values */
	}
	for(i=0; i<(len/4);i++){
		*(freqs+i) = (unsigned long)*(addr+i);
	}
}

static void of_get_cpuddr_scaling_rates(void)
{
	struct device_node *np;
	int len;
	const u32 *addr;

	np = of_find_compatible_node(NULL, NULL, "lantiq,scaling-frequencies");
	if (!np){
		pr_debug("No Scaling frequencies found");
		return;
	}
	addr = of_get_property(np, "lantiq,cpuclocks", &len);
	if(addr != NULL){
		get_frequencies(addr,
				&ltq_cpufreq_obj.cpu_scaling_rates[0], len);
	}
	addr = of_get_property(np, "lantiq,ddrclocks", &len);
	if(addr != NULL){
		get_frequencies(addr,
				&ltq_cpufreq_obj.ddr_scaling_rates[0], len);
	}
	return;
}

static int __init ltq_cpufreq_modinit(void)
{
	int ret = 0;
	struct cpufreq_policy ltq_cpufreq_policy;
	struct cpufreq_policy *policy = &ltq_cpufreq_policy;

	pr_debug("%s is called\n",__FUNCTION__);
	of_get_cpuddr_scaling_rates();
	ltq_freq_tab[0].frequency = ltq_cpufreq_obj.cpu_scaling_rates[0]/1000;
	ltq_freq_tab[1].frequency = ltq_cpufreq_obj.cpu_scaling_rates[1]/1000;
	ltq_freq_tab[2].frequency = ltq_cpufreq_obj.cpu_scaling_rates[2]/1000;
	ltq_freq_tab[3].frequency = ltq_cpufreq_obj.cpu_scaling_rates[3]/1000;

	/* register driver to the linux cpufreq driver */
	ret = cpufreq_register_driver(&ltq_cpufreq_driver);
	if(ret<0){
		pr_err("Error in %s, cpufreq_register_driver\n",__FUNCTION__);
		return ret;
	}
	/* define the usage of mips_wait instruction inside linux idle loop */
	cpu_wait_sav = cpu_wait; /* save cpu_wait pointer */
	list_add_tail(&cpuddr_feature_fss.list,
			&ltq_cpufreq_obj.list_head_module);
	list_add_tail(&cpuwait_feature_cgs.list,
			&ltq_cpufreq_obj.list_head_module);

	policy->cpu = 0; /* we have only one CPU */
	policy = cpufreq_cpu_get(policy->cpu);
	if (policy){
		ret = ltq_cpufreq_stats_create(policy);
		if(ret<0){
			pr_err("Error in %s,ltq_cpufreq_stats_create_table\n",
								__FUNCTION__);
			return ret;
		}
	}
	return ret;
}

static void __exit ltq_cpufreq_modexit(void)
{
	struct cpufreq_policy ltq_cpufreq_policy;
	struct cpufreq_policy *policy = &ltq_cpufreq_policy;
	pr_debug("%s is called\n",__FUNCTION__);

	list_del(&cpuddr_feature_fss.list);
	list_del(&cpuwait_feature_cgs.list);
	policy->cpu = 0; /* we have only one CPU */
	policy = cpufreq_cpu_get(policy->cpu);
	if (!policy)
		return;

		if (!policy_is_shared(policy)) {
		pr_debug("%s: Free sysfs ltq_stat\n", __func__);
		sysfs_remove_group(&policy->kobj, &ltq_stats_attr_group);
	}
	cpufreq_cpu_put(policy);
	cpufreq_unregister_driver(&ltq_cpufreq_driver);
}


module_init(ltq_cpufreq_modinit);
module_exit(ltq_cpufreq_modexit);

module_param(transition_latency, uint, 0644);
MODULE_PARM_DESC(transition_latency, "used from the ONDEMAND governor to "
		"calculate the sampling_rate. This is how often you want the"
		"kernel to look the CPU usage and make decisions on what to "
		"do with the frequency. Value must be given in ns. The"
		"sampling_rate is calculated by transition_latency * 1000."
		"E.g.: sampling_rate should be 5sec -> set transition_latency"
		"to 5000000.");

MODULE_AUTHOR("<t.bartholomae@lantiq.com>");
MODULE_DESCRIPTION("cpufreq driver for Lantiq XWAY");
MODULE_LICENSE("GPL");
