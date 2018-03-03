/*
 *
 *			   Copyright (c) 2012
 *			Lantiq Deutschland GmbH
 *		 Am Campeon 3; 85579 Neubiberg, Germany
 *
 *  For licensing information, see the file 'LICENSE' in the root folder of
 *  this software module.
 */

 #ifndef _LTQ_CPUFREQ_H_
#define _LTQ_CPUFREQ_H_

/** LTQ_CPUFREQ_MODULE_t
	Definition of the modules identifier
*/
enum ltq_cpufreq_module{
	LTQ_CPUFREQ_MODULE_CPU	,
	LTQ_CPUFREQ_MODULE_WLAN	,
	LTQ_CPUFREQ_MODULE_VE	,
	LTQ_CPUFREQ_MODULE_PPE	,
	LTQ_CPUFREQ_MODULE_SWITCH,
	LTQ_CPUFREQ_MODULE_UART	,
	LTQ_CPUFREQ_MODULE_GPTC	,
	LTQ_CPUFREQ_MODULE_PCIE	,
	LTQ_CPUFREQ_MODULE_USB	,
	LTQ_CPUFREQ_MODULE_DEU	,
	LTQ_CPUFREQ_MODULE_ID_MAX,
};


/** LTQ_CPUFREQ_STATE_t
	Definition of power management state
*/
enum ltq_cpufreq_state{
	/** Power State Invalid. */
	LTQ_CPUFREQ_PS_UNDEF,
	/** Power State D0. normal operation freq */
	LTQ_CPUFREQ_PS_D0,
	/** Power State D1.  intermediate freq */
	LTQ_CPUFREQ_PS_D1,
	/** Power State D2.  intermediate freq */
	LTQ_CPUFREQ_PS_D2,
	/** Power State D3. lowest freq */
	LTQ_CPUFREQ_PS_D3,
	/** Power State don't care */
	LTQ_CPUFREQ_PS_D0D3,
	/** Power State BOOST highest freq, time limited because of
	 *  thermal aspects */
	LTQ_CPUFREQ_PS_BOOST,
};

/* return codes used in LTQ_CPUFREQ */
#define	LTQ_CPUFREQ_RETURN_SUCCESS	0
	/** Operation denied */
#define	LTQ_CPUFREQ_RETURN_DENIED	1
	/** called function just return without doing anything; used only
	 *  in callback functions */
#define	LTQ_CPUFREQ_RETURN_NOACTIVITY	3
	/** is used if callback function is not defined */
#define	LTQ_CPUFREQ_RETURN_NOTDEFINED	4


/** IFX_PMCU_MODULE_STATE_t
	Structure hold the module-ID, the moduleSub-ID and the
	PowerState of one module.
*/
struct ltq_cpufreq_module_state{
	enum ltq_cpufreq_module pmcuModule;/** Module identifier */
	unsigned int  pmcuModuleNr;/** Module instance */
	enum ltq_cpufreq_state  pmcuState;/** Module PowerState */
};

struct ltq_cpufreq_module_info {
	struct list_head		list;
	char*				name;
	enum ltq_cpufreq_module		pmcuModule;
	unsigned int			pmcuModuleNr;
	int				powerFeatureStat;
	int	(*ltq_cpufreq_state_get) ( enum ltq_cpufreq_state *pmcuState );
	int	(*ltq_cpufreq_pwr_feature_switch) ( int pmcuPwrStateEna );
};


struct ltq_cpufreq_latency {
	int	max_time;
	int	min_time;
	int	cur_time;
	int	cpu_frequency;
	char*	comment;
};

struct ltq_cpufreq {
	struct list_head list_head_module;
	enum ltq_cpufreq_state cpufreq_cur_state;
	enum ltq_cpufreq_state cpufreq_new_state;
	unsigned long cpu_scaling_rates[8];
	unsigned long ddr_scaling_rates[8];
	int ltq_state_change_control;
};


int ltq_cpufreq_state_req ( enum ltq_cpufreq_module module,
				unsigned char moduleNr,
				enum ltq_cpufreq_state newState);
enum ltq_cpufreq_state ltq_cpufreq_get_ps_from_khz(unsigned int freqkhz);
unsigned int ltq_cpufreq_getfreq_khz(unsigned int cpu);
int ltq_cpufreq_state_change_disable(void);
int ltq_cpufreq_state_change_enable(void);
int ltq_count0_diff(unsigned int count_start, unsigned int count_end,
						char* ident, int index);
unsigned int ltq_count0_read(void);
struct ltq_cpufreq* ltq_cpufreq_get(void);

#endif /* _LTQ_CPUFREQ_H_ */
