/*
 * cmd_mfg.c - for manufacture testing
 *
 * Copyright (c) 2011 WNC inc.
 *
 * Licensed under the GPL-2 or later.
 */

#include <config.h>
#include <common.h>
#include <command.h>

#define UBOOT_RESET_GPIO			9
#define UBOOT_ANT_SWBTN_GPIO		11
#define UBOOT_ANT_SW_GPIO			19
#define UBOOT_WPS_GPIO				14
#define UBOOT_SIM_DETECT_GPIO		61

#define UBOOT_LED_RSSI_01_GPIO	10
#define UBOOT_LED_RSSI_02_GPIO	15
#define UBOOT_LED_RSSI_03_GPIO	16
#define UBOOT_LED_RSSI_04_GPIO	17
#define UBOOT_LED_RSSI_05_GPIO	18

int do_mfg_reset_button(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{

	u32 i = 0;
	u32 input_value = 0;

	if (argc != 2) {
		printf("mfg_rest_button [timer in sec]\n" );
		return 0;
	}

	int times = (int) simple_strtoul (argv[1], NULL, 10);

	 // GPIO 11  LTE_ANT switch
	 *AR10_GPIO_P0_DIR     &= ~(0x1 << UBOOT_ANT_SWBTN_GPIO);
	 *AR10_GPIO_P0_ALTSEL0 &= ~(0x1 << UBOOT_ANT_SWBTN_GPIO);
	 *AR10_GPIO_P0_ALTSEL1 &= ~(0x1 << UBOOT_ANT_SWBTN_GPIO);
	 *AR10_GPIO_P0_OD      |= (0x1 << UBOOT_ANT_SWBTN_GPIO);

	 //  GPIO 9 reset button
	 *AR10_GPIO_P0_DIR     &= ~(0x1 << UBOOT_RESET_GPIO);
	 *AR10_GPIO_P0_ALTSEL0 &= ~(0x1 << UBOOT_RESET_GPIO);
	 *AR10_GPIO_P0_ALTSEL1 &= ~(0x1 << UBOOT_RESET_GPIO);
	 *AR10_GPIO_P0_OD      |= (0x1 << UBOOT_RESET_GPIO);

	 // GPIO 14  , wps
	 *AR10_GPIO_P0_DIR     &= ~(0x1 << UBOOT_WPS_GPIO);
	 *AR10_GPIO_P0_ALTSEL0 &= ~(0x1 << UBOOT_WPS_GPIO);
	 *AR10_GPIO_P0_ALTSEL1 &= ~(0x1 << UBOOT_WPS_GPIO) ;
	 *AR10_GPIO_P0_OD      |= 0x1 << UBOOT_WPS_GPIO;
	
	input_value = *(AR10_GPIO_P0_IN) ;
	printf("Please press reset button --> " );

	while( (input_value & (0x1 << UBOOT_RESET_GPIO))  && i < 0x1000000 * times )
	{
		input_value = *(AR10_GPIO_P0_IN) ;
		i++;
	}
		
	if( i < 0x1000000 * times )
		printf("PASS \n");
	else
		printf("Fail \n");


}


int do_mfg_lte_button(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{

	u32 i = 0;
	u32 input_value = 0;

	if (argc != 2) {
		printf("mfg_lte_button [timer in sec]\n" );
		return 0;
	}

	int times = (int) simple_strtoul (argv[1], NULL, 10);

	 // GPIO 11  LTE_ANT switch
	 *AR10_GPIO_P0_DIR     &= ~(0x01 << UBOOT_ANT_SWBTN_GPIO);
	 *AR10_GPIO_P0_ALTSEL0 &= ~(0x01 << UBOOT_ANT_SWBTN_GPIO);
	 *AR10_GPIO_P0_ALTSEL1 &= ~(0x01 << UBOOT_ANT_SWBTN_GPIO);
	 *AR10_GPIO_P0_OD      |= (0x01 << UBOOT_ANT_SWBTN_GPIO);

	input_value = *(AR10_GPIO_P0_IN) ;
	printf("Please press LTE Antenna switch button --> " );	

	while( !( input_value & (0x1 << UBOOT_ANT_SWBTN_GPIO) )  && i < 0x1000000 *times )
	{
		input_value = *(AR10_GPIO_P0_IN) ;
		i++;
	}


	if( i < 0x1000000 * times )
		printf("PASS \n");
	else
		printf("Fail \n");


}


int do_mfg_wps_button(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	u32 i = 0;
	u32 input_value = 0;

	if (argc != 2) {
		printf("mfg_wps_button [timer in sec]\n" );
		return 0;
	}

	int times = (int) simple_strtoul (argv[1], NULL, 10);	

	//GPIO 14  , wps
	*AR10_GPIO_P0_DIR     &= ~(0x1 << UBOOT_WPS_GPIO);
	*AR10_GPIO_P0_ALTSEL0 &= ~(0x1 << UBOOT_WPS_GPIO);
	*AR10_GPIO_P0_ALTSEL1 &= ~(0x1 << UBOOT_WPS_GPIO);
	*AR10_GPIO_P0_OD      |= 0x1 << UBOOT_WPS_GPIO;

	input_value = *(AR10_GPIO_P0_IN) ;
	printf("Please press WPS button --> " );

	while( (input_value & (0x1 << UBOOT_WPS_GPIO))  && i < 0x1000000 * times)
	{
		input_value = *(AR10_GPIO_P0_IN);
		i++;
	}
		
	if( i < 0x1000000 * times)
		printf("PASS \n");
	else
		printf("Fail \n");

	return 0;
}


int do_mfg_sim_detect(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	u32 i = 0;
	volatile u32 input_value = 0;

	if (argc != 2) {
		printf("mfg_sim_detect [timer in sec]\n" );
		return 0;
	}

	int times = (int) simple_strtoul (argv[1], NULL, 10);	

	//GPIO 61, sim detect
	*AR10_GPIO_P3_DIR     &= ~(0x1 << (UBOOT_SIM_DETECT_GPIO%16));
	*AR10_GPIO_P3_PUDEN   |= (0x1 << (UBOOT_SIM_DETECT_GPIO%16));
	*AR10_GPIO_P3_PUDSEL  |= (0x1 << (UBOOT_SIM_DETECT_GPIO%16));
	*AR10_GPIO_P3_ALTSEL0 &= ~(0x1 << (UBOOT_SIM_DETECT_GPIO%16));
	*AR10_GPIO_P3_ALTSEL1 &= ~(0x1 << (UBOOT_SIM_DETECT_GPIO%16));
	*AR10_GPIO_P3_OD      |= (0x1 << (UBOOT_SIM_DETECT_GPIO%16));

	udelay(5000);

	input_value = *(AR10_GPIO_P3_IN) ;
	printf("Please insert SIM card --> " );

	while( (input_value & (0x1 << (UBOOT_SIM_DETECT_GPIO%16)))  && i < 0x1000000 * times)
	{
		input_value = *(AR10_GPIO_P3_IN);
		i++;
	}
		
	if( i < 0x1000000 * times)
		printf("PASS \n");
	else
		printf("Fail \n");

	return 0;
}

int do_boot_led(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{

	run_command("run dark_led",0);
	run_command("run red_led",0);
	udelay(500000);
	
	run_command("run dark_led",0);
	udelay(500000);
	
	run_command("run green_led",0);
	udelay(500000);

	//GPIO_18
	*AR10_GPIO_P1_DIR |= (0x1 << (UBOOT_LED_RSSI_05_GPIO%16));
	*AR10_GPIO_P1_ALTSEL0 &= ~(0x1 << (UBOOT_LED_RSSI_05_GPIO%16));
	*AR10_GPIO_P1_ALTSEL1 &= ~(0x1 << (UBOOT_LED_RSSI_05_GPIO%16)) ;
	*AR10_GPIO_P1_OD |= (0x1 << (UBOOT_LED_RSSI_05_GPIO%16));
	*AR10_GPIO_P1_OUT &= ~(0x1 << (UBOOT_LED_RSSI_05_GPIO%16));

	//GPIO_17
	*AR10_GPIO_P1_DIR |= (0x1 << (UBOOT_LED_RSSI_04_GPIO%16));
	*AR10_GPIO_P1_ALTSEL0 &= ~(0x1 << (UBOOT_LED_RSSI_04_GPIO%16));
	*AR10_GPIO_P1_ALTSEL1 &= ~(0x1 << (UBOOT_LED_RSSI_04_GPIO%16)) ;
	*AR10_GPIO_P1_OD |= (0x1 << (UBOOT_LED_RSSI_04_GPIO%16));
	*AR10_GPIO_P1_OUT &= ~(0x1 << (UBOOT_LED_RSSI_04_GPIO%16));

	//GPIO_16
	*AR10_GPIO_P1_DIR |= (0x1 << (UBOOT_LED_RSSI_03_GPIO%16));
	*AR10_GPIO_P1_ALTSEL0 &= ~(0x1 << (UBOOT_LED_RSSI_03_GPIO%16));
	*AR10_GPIO_P1_ALTSEL1 &= ~(0x1 << (UBOOT_LED_RSSI_03_GPIO%16)) ;
	*AR10_GPIO_P1_OD |= (0x1 << (UBOOT_LED_RSSI_03_GPIO%16));
	*AR10_GPIO_P1_OUT &= ~(0x1 << (UBOOT_LED_RSSI_03_GPIO%16));

	//GPIO_15
	*AR10_GPIO_P0_DIR |= (0x1 << (UBOOT_LED_RSSI_02_GPIO%16));
	*AR10_GPIO_P0_ALTSEL0 &= ~(0x1 << (UBOOT_LED_RSSI_02_GPIO%16));
	*AR10_GPIO_P0_ALTSEL1 &= ~(0x1 << (UBOOT_LED_RSSI_02_GPIO%16)) ;
	*AR10_GPIO_P0_OD |= (0x1 << (UBOOT_LED_RSSI_02_GPIO%16));
	*AR10_GPIO_P0_OUT &= ~(0x1 << (UBOOT_LED_RSSI_02_GPIO%16));

	//GPIO_10
	*AR10_GPIO_P0_DIR |= (0x1 << (UBOOT_LED_RSSI_01_GPIO%16));
	*AR10_GPIO_P0_ALTSEL0 &= ~(0x1 << (UBOOT_LED_RSSI_01_GPIO%16));
	*AR10_GPIO_P0_ALTSEL1 &= ~(0x1 << (UBOOT_LED_RSSI_01_GPIO%16)) ;
	*AR10_GPIO_P0_OD |= (0x1 << (UBOOT_LED_RSSI_01_GPIO%16));
	*AR10_GPIO_P0_OUT &= ~(0x1 << (UBOOT_LED_RSSI_01_GPIO%16));

	return 0;
}


int do_mfg_led(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	run_command("run dark_led",0);
	run_command("run green_led",0);
	udelay(1000000);
	
	run_command("run dark_led",0);
	udelay(1000000);
	
	run_command("run red_led",0);
	udelay(1000000);
	
	run_command("run dark_led",0);	

	return 0;
}

int do_ant_sw(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int tmp = (int) simple_strtoul (argv[1], NULL, 10);

	if (argc != 2) {
		printf("do_ant_sw [0~1]\n" );		
		return 0;
	}

	switch (tmp)
	{
		case 0:	//GPIO_19 Int Ant
			*AR10_GPIO_P1_DIR |= (0x1 << (UBOOT_ANT_SW_GPIO%16));
			*AR10_GPIO_P1_ALTSEL0 &= ~(0x1 << (UBOOT_ANT_SW_GPIO%16));
			*AR10_GPIO_P1_ALTSEL1 &= ~(0x1 << (UBOOT_ANT_SW_GPIO%16));
			*AR10_GPIO_P1_OD |= (0x1 << (UBOOT_ANT_SW_GPIO%16));
			*AR10_GPIO_P1_OUT &= ~(0x1 << (UBOOT_ANT_SW_GPIO%16));	
			printf("Switch to Internal Antenna\n" );			
			break;
			
		case 1: //GPIO_19 Ext  Ant
			*AR10_GPIO_P1_DIR |= (0x1 << (UBOOT_ANT_SW_GPIO%16));
			*AR10_GPIO_P1_ALTSEL0 &= ~(0x1 << (UBOOT_ANT_SW_GPIO%16));
			*AR10_GPIO_P1_ALTSEL1 &= ~(0x1 << (UBOOT_ANT_SW_GPIO%16));
			*AR10_GPIO_P1_OD |= (0x1 << (UBOOT_ANT_SW_GPIO%16));
			*AR10_GPIO_P1_OUT |= (0x1 << (UBOOT_ANT_SW_GPIO%16));			
			printf("Switch to External Antenna\n" );
			break;

		default:
			printf("Error!! Unknow parameter!!\n" );

	}
	
	return 0;
}

int do_mfg_5_bar_led_off(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int nth_led = (int) simple_strtoul (argv[1], NULL, 10);

	if (argc != 2) {
		printf("mfg_5_bar_led_off [1~5]\n" );
		
		return 0;
	}

	switch (nth_led)
	{
		case 1:	//GPIO_18
			*AR10_GPIO_P1_DIR |= (0x1 << (UBOOT_LED_RSSI_05_GPIO%16));
			*AR10_GPIO_P1_ALTSEL0 &= ~(0x1 << (UBOOT_LED_RSSI_05_GPIO%16));
			*AR10_GPIO_P1_ALTSEL1 &= ~(0x1 << (UBOOT_LED_RSSI_05_GPIO%16));
			*AR10_GPIO_P1_OD |= (0x1 << (UBOOT_LED_RSSI_05_GPIO%16));
			*AR10_GPIO_P1_OUT |= (0x1 << (UBOOT_LED_RSSI_05_GPIO%16));			
			break;
			
		case 2: //GPIO_17
			*AR10_GPIO_P1_DIR |= (0x1 << (UBOOT_LED_RSSI_04_GPIO%16));
			*AR10_GPIO_P1_ALTSEL0 &= ~(0x1 << (UBOOT_LED_RSSI_04_GPIO%16));
			*AR10_GPIO_P1_ALTSEL1 &= ~(0x1 << (UBOOT_LED_RSSI_04_GPIO%16));
			*AR10_GPIO_P1_OD |= (0x1 << (UBOOT_LED_RSSI_04_GPIO%16));
			*AR10_GPIO_P1_OUT |= (0x1 << (UBOOT_LED_RSSI_04_GPIO%16));			
			break;

		case 3: //GPIO_16
			*AR10_GPIO_P1_DIR |= (0x1 << (UBOOT_LED_RSSI_03_GPIO%16));
			*AR10_GPIO_P1_ALTSEL0 &= ~(0x1 << (UBOOT_LED_RSSI_03_GPIO%16));
			*AR10_GPIO_P1_ALTSEL1 &= ~(0x1 << (UBOOT_LED_RSSI_03_GPIO%16));
			*AR10_GPIO_P1_OD |= (0x1 << (UBOOT_LED_RSSI_03_GPIO%16));
			*AR10_GPIO_P1_OUT |= (0x1 << (UBOOT_LED_RSSI_03_GPIO%16));			
			break;

		case 4: //GPIO_15
			*AR10_GPIO_P0_DIR |= (0x1 << (UBOOT_LED_RSSI_02_GPIO%16));
			*AR10_GPIO_P0_ALTSEL0 &= ~(0x1 << (UBOOT_LED_RSSI_02_GPIO%16));
			*AR10_GPIO_P0_ALTSEL1 &= ~(0x1 << (UBOOT_LED_RSSI_02_GPIO%16));
			*AR10_GPIO_P0_OD |= (0x1 << (UBOOT_LED_RSSI_02_GPIO%16));
			*AR10_GPIO_P0_OUT |= (0x1 << (UBOOT_LED_RSSI_02_GPIO%16));			
			break;

		case 5: //GPIO_10
			*AR10_GPIO_P0_DIR |= (0x1 << (UBOOT_LED_RSSI_01_GPIO%16));
			*AR10_GPIO_P0_ALTSEL0 &= ~(0x1 << (UBOOT_LED_RSSI_01_GPIO%16));
			*AR10_GPIO_P0_ALTSEL1 &= ~(0x1 << (UBOOT_LED_RSSI_01_GPIO%16));
			*AR10_GPIO_P0_OD |= (0x1 << (UBOOT_LED_RSSI_01_GPIO%16));
			*AR10_GPIO_P0_OUT |= (0x1 << (UBOOT_LED_RSSI_01_GPIO%16));			
			break;

		default:
			printf("Error!! Led number incorrect!!\n" );

	}
	
	return 0;
}

int do_mfg_5_bar_led_on(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int nth_led = (int) simple_strtoul (argv[1], NULL, 10);

	if (argc != 2) {
		printf("mfg_5_bar_led_on [1~5]\n" );

		return 0;
	}

	switch (nth_led)
	{
		case 1:	//GPIO_18
			*AR10_GPIO_P1_DIR |= (0x1 << (UBOOT_LED_RSSI_05_GPIO%16));
			*AR10_GPIO_P1_ALTSEL0 &= ~(0x1 << (UBOOT_LED_RSSI_05_GPIO%16));
			*AR10_GPIO_P1_ALTSEL1 &= ~(0x1 << (UBOOT_LED_RSSI_05_GPIO%16));
			*AR10_GPIO_P1_OD |= (0x1 << (UBOOT_LED_RSSI_05_GPIO%16));
			*AR10_GPIO_P1_OUT &= ~(0x1 << (UBOOT_LED_RSSI_05_GPIO%16));			
			break;
			
		case 2: //GPIO_17
			*AR10_GPIO_P1_DIR |= (0x1 << (UBOOT_LED_RSSI_04_GPIO%16));
			*AR10_GPIO_P1_ALTSEL0 &= ~(0x1 << (UBOOT_LED_RSSI_04_GPIO%16));
			*AR10_GPIO_P1_ALTSEL1 &= ~(0x1 << (UBOOT_LED_RSSI_04_GPIO%16));
			*AR10_GPIO_P1_OD |= (0x1 << (UBOOT_LED_RSSI_04_GPIO%16));
			*AR10_GPIO_P1_OUT &= ~(0x1 << (UBOOT_LED_RSSI_04_GPIO%16));			
			break;

		case 3: //GPIO_16
			*AR10_GPIO_P1_DIR |= (0x1 << (UBOOT_LED_RSSI_03_GPIO%16));
			*AR10_GPIO_P1_ALTSEL0 &= ~(0x1 << (UBOOT_LED_RSSI_03_GPIO%16));
			*AR10_GPIO_P1_ALTSEL1 &= ~(0x1 << (UBOOT_LED_RSSI_03_GPIO%16));
			*AR10_GPIO_P1_OD |= (0x1 << (UBOOT_LED_RSSI_03_GPIO%16));
			*AR10_GPIO_P1_OUT &= ~(0x1 << (UBOOT_LED_RSSI_03_GPIO%16));			
			break;

		case 4: //GPIO_15
			*AR10_GPIO_P0_DIR |= (0x1 << (UBOOT_LED_RSSI_02_GPIO%16));
			*AR10_GPIO_P0_ALTSEL0 &= ~(0x1 << (UBOOT_LED_RSSI_02_GPIO%16));
			*AR10_GPIO_P0_ALTSEL1 &= ~(0x1 << (UBOOT_LED_RSSI_02_GPIO%16));
			*AR10_GPIO_P0_OD |= (0x1 << (UBOOT_LED_RSSI_02_GPIO%16));
			*AR10_GPIO_P0_OUT &= ~(0x1 << (UBOOT_LED_RSSI_02_GPIO%16));			
			break;

		case 5: //GPIO_10
			*AR10_GPIO_P0_DIR |= (0x1 << (UBOOT_LED_RSSI_01_GPIO%16));
			*AR10_GPIO_P0_ALTSEL0 &= ~(0x1 << (UBOOT_LED_RSSI_01_GPIO%16));
			*AR10_GPIO_P0_ALTSEL1 &= ~(0x1 << (UBOOT_LED_RSSI_01_GPIO%16));
			*AR10_GPIO_P0_OD |= (0x1 << (UBOOT_LED_RSSI_01_GPIO%16));
			*AR10_GPIO_P0_OUT &= ~(0x1 << (UBOOT_LED_RSSI_01_GPIO%16));			
			break;

		default:
			printf("Error!! Led number incorrect!!\n" );

	}
	
	return 0;
}

U_BOOT_CMD(
	mfg_sim_detect,	2,	1,	do_mfg_sim_detect,
	"mfg_sim_detect - sim detect testing\n",
	"mfg_sim_detect [timer in sec]\n"
);

U_BOOT_CMD(
	ant_sw, 2, 1, do_ant_sw,
	"ant_sw testing\n",
	"ant_sw [0/1]\n"
);

U_BOOT_CMD(
	mfg_5_bar_led_on, 2, 1, do_mfg_5_bar_led_on,
	"mfg_5_bar_led_on - 5 bar led(on) testing\n",
	"mfg_5_bar_led_on [n-th led number]\n"
);

U_BOOT_CMD(
	mfg_5_bar_led_off, 2, 1, do_mfg_5_bar_led_off,
	"mfg_5_bar_led_off - 5 bar led(off) testing\n",
	"mfg_5_bar_led_off [n-th led number]\n"
);

U_BOOT_CMD(
	boot_led,	1,	1,	do_boot_led,
	"boot_led - Led testing\n",
	"Tesing all leds\n"
);

U_BOOT_CMD(
	mfg_wps_button,	2,	1,	do_mfg_wps_button,
	"mfg_wps_button - wps button testing\n",
	"mfg_wps_button [timer in sec]\n"
);

U_BOOT_CMD(
	mfg_led,	1,	1,	do_mfg_led,
	"mfg_led - Led testing\n",
	"Tesing all leds\n"
);

U_BOOT_CMD(
	mfg_reset_button,	2,	1,	do_mfg_reset_button,
	"mfg_reset_button - rest Button testing\n",
	"mfg_reset_button [timer in sec]\n"
);

U_BOOT_CMD(
	mfg_lte_button,	2,	1,	do_mfg_lte_button,
	"mfg_lte_button - lte button testing\n",
	"mfg_lte_button [timer in sec]\n"
);
