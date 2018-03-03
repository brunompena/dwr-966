/*
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *
 *  Copyright (C) 2013 Reddy Mallikarjuna <Reddy.Mallikarjun@lantiq.com>
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/moduleparam.h>
#include <linux/io.h>
#include <linux/ioport.h>
#include <linux/pm.h>
#include <linux/export.h>
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/of_address.h>
#include <linux/of_platform.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/vmalloc.h>
#include <linux/dma-mapping.h>
#include <linux/firmware.h>
#include <asm/uaccess.h>
#include <linux/mm.h>
#include <asm/reboot.h>
#include <linux/kthread.h>
#include <lantiq_soc.h>
//#include <ifx_ethsw_kernel_api.h>
#include <switch-api/lantiq_ethsw_api.h>
static struct proc_dir_entry *g_proc_gphy_dir;

static int dev_id; 
static struct device_node *gphy_node;
static struct device *gphy_dev;
const char *g_gphy_fw_mode = NULL;
static int g_no_phys, pw_save_mode;
static dma_addr_t g_dev_addr = 0;
static void *g_fw_addr;
#define XWAY_GPHY_FW_ALIGN	(16 * 1024)
#define XWAY_GPHY_FW_ALIGN_MASK (XWAY_GPHY_FW_ALIGN - 1)
static u8 *g_gphy_fw_dma;
#define	IH_MAGIC_NO	0x27051956	/* Image Magic Number		*/
#define IH_NMLEN		32	/* Image Name Length		*/
#define VERSIONLEN	16

/** Legacy format image header,
 * all data in network byte order (aka natural aka bigendian).*/
typedef struct gphy_image_header {
	uint32_t	ih_magic;	/* Image Header Magic Number	*/
	uint32_t	ih_hcrc;	/* Image Header CRC Checksum	*/
	uint32_t	ih_time;	/* Image Creation Timestamp	*/
	uint32_t	ih_size;	/* Image Data Size		*/
	uint32_t	ih_load;	/* Data	 Load  Address		*/
	uint32_t	ih_ep;		/* Entry Point Address		*/
	uint32_t	ih_dcrc;	/* Image Data CRC Checksum	*/
	uint8_t		ih_os;		/* Operating System		*/
	uint8_t		ih_arch;	/* CPU architecture		*/
	uint8_t		ih_type;	/* Image Type			*/
	uint8_t		ih_comp;	/* Compression Type		*/
	uint8_t		ih_name[IH_NMLEN];	/* Image Name		*/
#ifdef CONFIG_LANTIQ_IMAGE_EXTRA_CHECKS
	uint8_t     ih_vendor[IH_NMLEN]; /* 32 char Vendor Name String */
	uint8_t     ih_board[IH_NMLEN]; /* 32 char Board Name String */
	uint8_t     ih_boardVer[VERSIONLEN]; /* Board Version 16 char str */
	uint8_t     ih_chip[IH_NMLEN]; /* 32 char Chip Name String */
	uint8_t     ih_chipVer[VERSIONLEN]; /* Chip Version16 char string */
	uint8_t     ih_swVer[VERSIONLEN]; /* Software Version-16 char string*/
#endif /* CONFIG_LANTIQ_IMAGE_EXTRA_CHECKS */
} gphy_image_header_t;
LTQ_ETHSW_API_HANDLE swithc_api_fd;
struct task_struct *gphy_pw_id;
static int gphy_pw_save_thread (void *arg);
/* Signal Related */
wait_queue_head_t gphy_pw_wait;
static unsigned long on_interval = 3;
static unsigned long off_interval = 3;
#define NUM_OF_PORTS 6
static int gphy_link_status[NUM_OF_PORTS] = {0};
static int gphy_power_down[NUM_OF_PORTS] = {0};
static int power_down_cnt[NUM_OF_PORTS] = {0};

/* reset request register */
#define RCU_RST_REQ		0x0010

/* reboot bit */
#define RCU_RD_GPHY0_XRX200	BIT(31)
#define RCU_RD_SRST		BIT(30)
#define RCU_RD_GPHY1_XRX200	BIT(29)
/* vr9 gphy registers */
#define RCU_GFS_ADD0_XRX200	0x0020
#define RCU_GFS_ADD1_XRX200	0x0068

/* xRX300 gphy registers */
#define RCU_GFS_ADD0_XRX300	0x0020
#define RCU_GFS_ADD1_XRX300	0x0058
#define RCU_GFS_ADD2_XRX300	0x00AC

/* xRX330 gphy registers */
#define RCU_GFS_ADD0_XRX330	0x0020
#define RCU_GFS_ADD1_XRX330	0x0058
#define RCU_GFS_ADD2_XRX330	0x00AC
#define RCU_GFS_ADD3_XRX330	0x0264

/* xRX300 bits */
#define RCU_RD_GPHY0_XRX300	BIT(31)
#define RCU_RD_GPHY1_XRX300	BIT(29)
#define RCU_RD_GPHY2_XRX300	BIT(28)

/* xRX330 bits */
#define RCU_RD_GPHY0_XRX330	BIT(31)
#define RCU_RD_GPHY1_XRX330	BIT(29)
#define RCU_RD_GPHY2_XRX330	BIT(28)
#define RCU_RD_GPHY3_XRX330	BIT(10)

/* xRX300 gphy registers */
#define RCU_GFS_ADD0_XRX300	0x0020
#define RCU_GFS_ADD1_XRX300	0x0058
#define RCU_GFS_ADD2_XRX300	0x00AC

/* xRX330 gphy registers */
#define RCU_GFS_ADD0_XRX330	0x0020
#define RCU_GFS_ADD1_XRX330	0x0058
#define RCU_GFS_ADD2_XRX330	0x00AC
#define RCU_GFS_ADD3_XRX330	0x0264

/* reset / boot a gphy */
static struct ltq_xrx200_gphy_reset {
	u32 rd;
	u32 addr;
} xrx200_gphy[] = {
	{RCU_RD_GPHY0_XRX200, RCU_GFS_ADD0_XRX200},
	{RCU_RD_GPHY1_XRX200, RCU_GFS_ADD1_XRX200},
};

/* reset / boot a gphy */
static struct ltq_xrx300_gphy_reset {
	u32 rd;
	u32 addr;
} xrx300_gphy[] = {
	{RCU_RD_GPHY0_XRX300, RCU_GFS_ADD0_XRX300},
	{RCU_RD_GPHY1_XRX300, RCU_GFS_ADD1_XRX300},
	{RCU_RD_GPHY2_XRX300, RCU_GFS_ADD2_XRX300},
};

/* reset / boot a gphy */
static struct ltq_xrx330_gphy_reset {
	u32 rd;
	u32 addr;
} xrx330_gphy[] = {
	{RCU_RD_GPHY0_XRX330, RCU_GFS_ADD0_XRX330},
	{RCU_RD_GPHY1_XRX330, RCU_GFS_ADD1_XRX330},
	{RCU_RD_GPHY2_XRX330, RCU_GFS_ADD2_XRX330},
	{RCU_RD_GPHY3_XRX330, RCU_GFS_ADD3_XRX330},
};

#define LTQ_DRV_MODULE_NAME			"ltq_xrx_gphy"
#define LTQ_PHY_DRV_VERSION      "2.0.0"
static char version[] = LTQ_DRV_MODULE_NAME ": V" LTQ_PHY_DRV_VERSION "";

/* Proc File  */
static int proc_file_create(void);
static void proc_file_delete(void);
static int proc_read_ver(struct seq_file *, void *);
static int proc_read_phy_fw(struct seq_file *, void *);
static ssize_t proc_write_phy_fw(struct file *, const char __user *, size_t, loff_t *);
static int proc_read_ver_open(struct inode *, struct file *);
static int proc_read_phy_fw_open(struct inode *, struct file *);

static int proc_read_phy_off_open(struct inode *, struct file *);
static int proc_read_phy_on_open(struct inode *, struct file *);
static int proc_read_phy_on(struct seq_file *, void *);
static int proc_read_phy_off(struct seq_file *, void *);
static ssize_t proc_write_phy_on(struct file *, const char __user *, size_t, loff_t *);
static ssize_t proc_write_phy_off(struct file *, const char __user *, size_t, loff_t *);
static void lq_ethsw_mdio_data_write(unsigned int phyAddr, unsigned int regAddr,unsigned int data);


//For VRX220 SW control PHY LED
#include <linux/sched.h>
#include <linux/of_gpio.h>
struct task_struct *gphy_rmon_poll_thread_id;
#define LED_OFF	0
#define LED_ON		1
#define LED_FLASH	2
#define GPIO_VRX200_OFFSET  200
enum gphy_gpio_mapping {
	GPHY_2_GPIO = 33 + GPIO_VRX200_OFFSET,
	GPHY_3_GPIO = 11 + GPIO_VRX200_OFFSET,
	GPHY_4_GPIO = 12 + GPIO_VRX200_OFFSET,
	GPHY_5_GPIO = 15 + GPIO_VRX200_OFFSET,
};
int gphy_led_state[NUM_OF_PORTS] = {0,0,0,0,0,0};	/* 0: OFF, 1: ON, 2: flash */
int gphy_led_status_on[NUM_OF_PORTS] = {0,0,0,0,0,0};
//end For VRX220 SW control PHY LED


/*
static unsigned short lq_ethsw_mdio_data_read(unsigned int phyAddr, unsigned int regAddr);
*/
static dma_addr_t xway_gphy_load(unsigned char *pData);
static int ltq_gphy_firmware_config (void);
static struct file_operations file_phy_fw_fops = {
    .owner      = THIS_MODULE,
    .open       = proc_read_phy_fw_open,
    .read       = seq_read,
    .write      = proc_write_phy_fw,
    .llseek     = seq_lseek,
    .release    = single_release,
};

static int proc_read_phy_fw_open(struct inode *inode, struct file *file)
{
    return single_open(file, proc_read_phy_fw, NULL);
}

unsigned int found_magic = 0, found_img = 0, first_block =0, fw_len=0, rcv_size = 0, second_block = 0;
static ssize_t proc_write_phy_fw(struct file *file, const char __user *buf, size_t count, loff_t *data)
{
	gphy_image_header_t header;
	int len = 0;
	char local_buf[4096] = {0};
	memset(&header, 0, sizeof(gphy_image_header_t));
	len = sizeof(local_buf) < count ? sizeof(local_buf) - 1 : count;
	len = len - copy_from_user((local_buf), buf, len);

	if (!found_img) {
		memcpy(&header, (local_buf + (found_magic * sizeof(gphy_image_header_t))), sizeof(gphy_image_header_t));
		if (header.ih_magic == IH_MAGIC_NO) {
			found_magic++;
			first_block = 0;
			second_block = 0;
		}
	}
	if (!found_img) {
		if (strcmp(g_gphy_fw_mode, "11G-FW") == 0) {
/*			dev_info(gphy_dev, "%s: header:%s, size:%d  \n", __func__,header.ih_name, header.ih_size);*/
			if (((strncmp(header.ih_name, "VR9 V1.1 GPHY GE", sizeof(header.ih_name)) == 0)	\
				|| (strncmp(header.ih_name, "AR10 V1.1 GPHY GE", sizeof(header.ih_name)) == 0))	\
				&& (dev_id == 0) ) {
				first_block = 1;
				fw_len = header.ih_size;
				found_img = 1;
				dev_info(gphy_dev, "%s:   Found:%s  FW \n", __func__,header.ih_name);
			} else if (((strncmp(header.ih_name, "VR9 V1.2 GPHY GE", sizeof(header.ih_name)) == 0)	\
				|| (strncmp(header.ih_name, "AR10 V1.2 GPHY GE", sizeof(header.ih_name)) == 0))	\
				&& (dev_id == 1))	{
				first_block = 1;
				fw_len = header.ih_size;
				found_img = 1;
				dev_info(gphy_dev, "%s:   Found:%s  FW \n", __func__,header.ih_name);
			}
		} else if (strcmp(g_gphy_fw_mode, "22F-FW") == 0) {
			if(((strncmp(header.ih_name, "VR9 V1.1 GPHY FE", sizeof(header.ih_name)) == 0)	\
				 || (strncmp(header.ih_name, "AR10 V1.1 GPHY FE", sizeof(header.ih_name)) == 0))	\
				 && (dev_id == 0)) {
				first_block = 1;
				fw_len = header.ih_size;
				found_img = 1;
				dev_info(gphy_dev, "%s:   Found:%s  FW \n", __func__,header.ih_name);
			} else if (((strncmp(header.ih_name, "VR9 V1.2 GPHY FE", sizeof(header.ih_name)) == 0)	\
				|| (strncmp(header.ih_name, "AR10 V1.2 GPHY FE", sizeof(header.ih_name)) == 0 ))	\
				&& (dev_id == 1)) {
				first_block = 1;
				fw_len = header.ih_size;
				found_img = 1;
				dev_info(gphy_dev, "%s:  Found:%s  FW \n", __func__,header.ih_name);
			}
		}
	}

	if ((first_block == 1) && (!second_block) && found_img) {
		g_gphy_fw_dma = (u8*) kmalloc (fw_len * sizeof (unsigned char), GFP_KERNEL);
		memset(g_gphy_fw_dma, 0, fw_len);
/*		dev_info(gphy_dev, "%s:**** Address g_gphy_fw_dma:%X  \n", __func__, g_gphy_fw_dma); */
		rcv_size = (len - (found_magic * sizeof(gphy_image_header_t)));
		memcpy(g_gphy_fw_dma, local_buf+(found_magic * sizeof(gphy_image_header_t)), rcv_size);
		second_block = 1;
	} else if ((second_block == 1) && found_img) {
		if (rcv_size < (fw_len) ) {
			if ((rcv_size + len) >= fw_len) {
				memcpy(g_gphy_fw_dma+rcv_size, local_buf, (fw_len-rcv_size));
				first_block = 0;
				found_img = 0;
				second_block = 0;
				found_magic = 0;
				ltq_gphy_firmware_config();
			} else {
				memcpy(g_gphy_fw_dma+rcv_size, local_buf, (len));
				rcv_size += len;
			}
		} else {
			first_block = 0;
			found_img = 0;
			second_block = 0;
			found_magic = 0;
			ltq_gphy_firmware_config();
		}	
	}
	return len;
}

static int proc_read_phy_fw(struct seq_file *seq, void *v)
{
    return 0;
}
/*
static void space_ignore(char **p, int *len)
{
	while ( *len && (**p <= ' ' || **p == ':' || **p == '.' || **p == ',') ) {
		(*p)++;
		(*len)--;
	}
}
*/
static int get_number(char **p, int *len, int is_hex)
{
	int ret = 0, n = 0;
	if ( (*p)[0] == '0' && (*p)[1] == 'x' ) {
		is_hex = 1;
		(*p) += 2;
		(*len) -= 2;
	}
	if ( is_hex ) {
		while ( *len && ((**p >= '0' && **p <= '9')	\
			|| (**p >= 'a' && **p <= 'f') || (**p >= 'A' && **p <= 'F'))) {
			if ( **p >= '0' && **p <= '9' )
				n = **p - '0';
			else if ( **p >= 'a' && **p <= 'f' )
				n = **p - 'a' + 10;
			else if ( **p >= 'A' && **p <= 'F' )
				n = **p - 'A' + 10;
			ret = (ret << 4) | n;
			(*p)++;
			(*len)--;
		}
	} else {
		while ( *len && **p >= '0' && **p <= '9' ) {
			n = **p - '0';
			ret = ret * 10 + n;
			(*p)++;
			(*len)--;
		}
	}
	return ret;
}
static struct file_operations file_phy_on_fops = {
    .owner      = THIS_MODULE,
    .open       = proc_read_phy_on_open,
    .read       = seq_read,
    .write      = proc_write_phy_on,
    .llseek     = seq_lseek,
    .release    = single_release,
};

static ssize_t proc_write_phy_on(struct file *file, const char __user *buf, size_t count, loff_t *data)
{
	int len, rlen, num;
	char local_buf[512] = {0};
	char *p;
	len = sizeof(local_buf) < count ? sizeof(local_buf) - 1 : count;
	rlen = len - copy_from_user((local_buf), buf, len);
	while ( rlen && local_buf[rlen - 1] <= ' ' )
		rlen--;
	local_buf[rlen] = 0;
	for ( p = local_buf; *p && *p <= ' '; p++, rlen-- );
	if ( !*p )
		return 0;
	num = get_number(&p, &rlen, 0);
	on_interval = ( num < 4 )? 3 : num;
	return len;
}

static int proc_read_phy_on_open(struct inode *inode, struct file *file)
{
    return single_open(file, proc_read_phy_on, NULL);
}

static int proc_read_phy_on(struct seq_file *seq, void *v)
{
    return 0;
}

static struct file_operations file_phy_off_fops = {
    .owner      = THIS_MODULE,
    .open       = proc_read_phy_off_open,
    .read       = seq_read,
    .write      = proc_write_phy_off,
    .llseek     = seq_lseek,
    .release    = single_release,
};

static ssize_t proc_write_phy_off(struct file *file, const char __user *buf, size_t count, loff_t *data)
{
	int len, rlen, num;
	char local_buf[512] = {0};
	char *p;
	len = sizeof(local_buf) < count ? sizeof(local_buf) - 1 : count;
	rlen = len - copy_from_user((local_buf), buf, len);
	while ( rlen && local_buf[rlen - 1] <= ' ' )
		rlen--;
	local_buf[rlen] = 0;
	for ( p = local_buf; *p && *p <= ' '; p++, rlen-- );
	if ( !*p )
		return 0;
	num = get_number(&p, &rlen, 0);
	off_interval = ( num < 4 )? 3 : num;
	return len;
}
static int proc_read_phy_off_open(struct inode *inode, struct file *file)
{
    return single_open(file, proc_read_phy_off, NULL);
}
static int proc_read_phy_off(struct seq_file *seq, void *v)
{
    return 0;
}
static struct file_operations file_ver_fops = {
	.owner	= THIS_MODULE,
	.open	= proc_read_ver_open,
	.read	= seq_read,
	.llseek	= seq_lseek,
	.release	= single_release,
};

static int proc_read_ver_open(struct inode *inode, struct file *file)
{
	return single_open(file, proc_read_ver, NULL);
}

static int proc_read_ver(struct seq_file *seq, void *v)
{
	seq_printf(seq, "LTQ GPHY driver %s, version %s, on:%d, off:%d, pw_mode:%d\n",	\
		g_gphy_fw_mode, version, on_interval, off_interval, pw_save_mode);
	return 0;
}

static int proc_file_create(void)
{
	struct proc_dir_entry *entry;

	g_proc_gphy_dir = proc_mkdir("driver/ltq_gphy", NULL);
	if (!g_proc_gphy_dir)
		return -ENOMEM;
	entry  = proc_create ("version", S_IRUGO, g_proc_gphy_dir, &file_ver_fops);
	if (!entry)
		goto Err0;
	entry = proc_create ("phyfirmware", S_IRUGO|S_IWUSR, g_proc_gphy_dir, &file_phy_fw_fops);
	if (!entry)
		goto Err2;
	entry = proc_create ("on_delay", S_IRUGO|S_IWUSR, g_proc_gphy_dir, &file_phy_on_fops);
	if (!entry)
		goto Err3;
	entry = proc_create ("off_delay", S_IRUGO|S_IWUSR, g_proc_gphy_dir, &file_phy_off_fops);
	if (!entry)
		goto Err4;
	return 0;
Err4:
	remove_proc_entry("on_delay", g_proc_gphy_dir);
Err3:
	remove_proc_entry("phyfirmware", g_proc_gphy_dir);
Err2:
	remove_proc_entry("version", g_proc_gphy_dir);
Err0:
	remove_proc_entry("driver/ltq_gphy", NULL);
	return -ENOMEM;
}

static void proc_file_delete(void)
{
	if (!g_proc_gphy_dir)
		return;
	remove_proc_entry("version", g_proc_gphy_dir);
	remove_proc_entry("phyfirmware", g_proc_gphy_dir);
	remove_proc_entry("driver/ltq_gphy", NULL);
}

static dma_addr_t xway_gphy_load (unsigned char *pData)
{
	dma_addr_t dev_addr = 0;
	void *fw_addr;
	size_t size;
	/*
	 * GPHY cores need the firmware code in a persistent and contiguous
	 * memory area with a 16 kB boundary aligned start address
	 */
	size = fw_len + XWAY_GPHY_FW_ALIGN;
	if (g_fw_addr) {
		dma_free_coherent(gphy_dev, size, g_fw_addr, g_dev_addr);
		g_fw_addr = NULL;
	}
	fw_addr = dma_alloc_coherent(gphy_dev, size, &dev_addr, GFP_KERNEL);
	if (fw_addr) {
		fw_addr = PTR_ALIGN(fw_addr, XWAY_GPHY_FW_ALIGN);
		dev_addr = ALIGN(dev_addr, XWAY_GPHY_FW_ALIGN);
		memcpy(fw_addr, pData, fw_len);
		g_fw_addr = fw_addr;
		g_dev_addr = dev_addr;
	} else {
		dev_err(gphy_dev, "failed to alloc firmware memory\n");
	}
	return dev_addr;
}

/* reset and boot a gphy. these phys only exist on xrx200 SoC */
int xway_gphy_rcu_config(unsigned int id, dma_addr_t dev_addr)
{
	if (of_machine_is_compatible("lantiq,vr9") || of_machine_is_compatible("lantiq,xrx220") ) {
		struct clk *clk;
		clk = clk_get_sys("1f203000.rcu", "gphy");
		if (IS_ERR(clk))
			return PTR_ERR(clk);
		clk_enable(clk);
	}

	if (of_machine_is_compatible("lantiq,vr9") || of_machine_is_compatible("lantiq,xrx220") ) {
		if (id > 1) {
			dev_info(gphy_dev, "%u is an invalid gphy id\n", id);
			return -EINVAL;
		}
		ltq_rcu_w32_mask(0, xrx200_gphy[id].rd, RCU_RST_REQ);
		ltq_rcu_w32(dev_addr, xrx200_gphy[id].addr);
		ltq_rcu_w32_mask(xrx200_gphy[id].rd, 0,  RCU_RST_REQ);
		dev_info(gphy_dev, "booting GPHY%u firmware at %X for VR9\n", id, dev_addr);
	} else if (of_machine_is_compatible("lantiq,ar10")) {
		if (id > 2) {
			dev_info(gphy_dev, "%u is an invalid gphy id\n", id);
			return -EINVAL;
		}
		ltq_rcu_w32_mask(0, xrx300_gphy[id].rd, RCU_RST_REQ);
		ltq_rcu_w32(dev_addr, xrx300_gphy[id].addr);
		ltq_rcu_w32_mask(xrx300_gphy[id].rd, 0,  RCU_RST_REQ);
		dev_info(gphy_dev, "booting GPHY%u firmware at %X for AR10\n", id, dev_addr);
	} else if (of_machine_is_compatible("lantiq,grx390")) {
		if (id > 3) {
			dev_info(gphy_dev, "%u is an invalid gphy id\n", id);
			return -EINVAL;
		}
		ltq_rcu_w32_mask(0, xrx330_gphy[id].rd, RCU_RST_REQ);
		ltq_rcu_w32(dev_addr, xrx330_gphy[id].addr);
		ltq_rcu_w32_mask(xrx330_gphy[id].rd, 0,  RCU_RST_REQ);
		dev_info(gphy_dev, "booting GPHY%u firmware at %X for GRX390\n", id, dev_addr);
	}
	return 0;
}

static int ltq_gphy_firmware_config()
{
	int i;
	dma_addr_t data_ptr;
	data_ptr = xway_gphy_load(g_gphy_fw_dma);
	if (g_gphy_fw_dma)
		kfree(g_gphy_fw_dma);
	if (!data_ptr)
		return -ENOMEM;
	for (i = 0; i < g_no_phys; i++)
		xway_gphy_rcu_config(i,data_ptr);
	pr_err("%s: fw_mode:%s, no of phys:%d,data_ptr:%X\n", __func__, g_gphy_fw_mode, g_no_phys,data_ptr);
	return 0;
}

static void lq_ethsw_mdio_data_write(unsigned int phyAddr, unsigned int regAddr,unsigned int data)
{
	IFX_ETHSW_MDIO_data_t mdio_data;
	memset(&mdio_data, 0, sizeof(IFX_ETHSW_MDIO_data_t));
	mdio_data.nAddressDev = phyAddr;
	mdio_data.nAddressReg = regAddr;
	mdio_data.nData = data;
	ltq_ethsw_api_kioctl(swithc_api_fd, IFX_ETHSW_MDIO_DATA_WRITE, (unsigned int)&mdio_data);
    return ;
}
/*
static unsigned short lq_ethsw_mdio_data_read(unsigned int phyAddr, unsigned int regAddr)
{
	IFX_ETHSW_MDIO_data_t mdio_data;
	memset(&mdio_data, 0, sizeof(IFX_ETHSW_MDIO_data_t));
	mdio_data.nAddressDev = phyAddr;
	mdio_data.nAddressReg = regAddr;
	ltq_ethsw_api_kioctl(swithc_api_fd, IFX_ETHSW_MDIO_DATA_READ, (unsigned int)&mdio_data);
    return (mdio_data.nData );
}
*/
static int gphy_pw_save_thread (void *arg)
{
	allow_signal(SIGKILL);	
	while(!kthread_should_stop ()) {
		u8 index;
		IFX_ETHSW_portLinkCfg_t param;
		memset(&param, 0, sizeof(IFX_ETHSW_portLinkCfg_t));
		set_current_state(TASK_INTERRUPTIBLE);
		if (signal_pending(current))
			break;
		/* Get the port Link Status  */
		for (index = 0; index < NUM_OF_PORTS; index++) {
			if ((gphy_power_down[index] == 1) && (gphy_link_status[index] == 0)) {
				lq_ethsw_mdio_data_write(index, 0x0, 0x1040);
			}
		}
		interruptible_sleep_on_timeout(&gphy_pw_wait, (off_interval * HZ) );
		for (index = 0; index < NUM_OF_PORTS; index++) {
			param.nPortId = index;
			ltq_ethsw_api_kioctl(swithc_api_fd, IFX_ETHSW_PORT_LINK_CFG_GET, (unsigned int)&param);
			if (param.eLink == 0 ) {
				gphy_link_status[index] = 1;
				power_down_cnt[index] = 0;
			} else {
				gphy_link_status[index] = 0;
				power_down_cnt[index]++;
				if ((power_down_cnt[index] % 10) != 0)
					lq_ethsw_mdio_data_write(index, 0x0, 0x1840);
				gphy_power_down[index] = 1;
			}
		}
		/*poll again  once configured time is up */
		interruptible_sleep_on_timeout(&gphy_pw_wait, (on_interval * HZ) );
	}
}

//For VRX220 SW control PHY LED
/* Switches on the LED */
/* Input:   port
 *		:	on_off */
 /* Process: Use the GPIO to ON/OFF the LED 
 */
 void gphy_data_led_on_off (int port, int on_off)
 {
 	u32 gpio_pin;
 	switch (port) {
 		case 2:
 			gpio_pin = GPHY_2_GPIO;
 			break;
 		case 3:
 			gpio_pin = GPHY_3_GPIO;
			break;
		case 4:
			gpio_pin = GPHY_4_GPIO;
			break;	
		case 5:
			gpio_pin = GPHY_5_GPIO;
			break;
	}
	if (on_off) {
		gpio_set_value(gpio_pin, 1);
	} else {
		gpio_set_value(gpio_pin, 0);
	}
}

static int gphy_rmon_poll_thread (void *arg)
{
	int port;
	int port_rx[NUM_OF_PORTS];
	int port_rx_prev[NUM_OF_PORTS] = {0,0,0,0,0,0};
	int port_tx[NUM_OF_PORTS];
	int port_tx_prev[NUM_OF_PORTS] = {0,0,0,0,0,0};
	IFX_ETHSW_RMON_cnt_t param;
	IFX_ETHSW_portLinkCfg_t param_link;
	printk (KERN_INFO "start %p ..\n", current);
	allow_signal(SIGKILL);
	while(!kthread_should_stop ()) {
		set_current_state(TASK_INTERRUPTIBLE);
		if (signal_pending(current))
			break;
		swithc_api_fd = ltq_ethsw_api_kopen("/dev/switch_api/0");	 
		for (port = 2; port < NUM_OF_PORTS; port++) {
			memset(&param, 0, sizeof(IFX_ETHSW_RMON_cnt_t));
			memset(&param_link, 0, sizeof(IFX_ETHSW_portLinkCfg_t));
			param.nPortId = port;
			param_link.nPortId = port;
			ltq_ethsw_api_kioctl(swithc_api_fd, IFX_ETHSW_RMON_GET,(unsigned int)&param);
			port_rx[port] = param.nRxGoodPkts;
			port_tx[port] = param.nTxGoodPkts;
			if ((port_rx[port] != port_rx_prev[port]) ||(port_tx[port] != port_tx_prev[port])) {
				if (gphy_led_status_on[port] == 0) {
					gphy_led_status_on[port] = 1;
					gphy_data_led_on_off(port, LED_ON);
				} else {
					gphy_led_status_on[port] = 0;
					gphy_data_led_on_off(port, LED_OFF);
				}
				port_rx_prev[port] = port_rx[port];
				port_tx_prev[port] = port_tx[port];
			} else {
				ltq_ethsw_api_kioctl(swithc_api_fd, IFX_ETHSW_PORT_LINK_CFG_GET, (unsigned int)&param_link);
				if (param_link.eLink == 0 ) {
					gphy_led_status_on[port] = 1;
					gphy_data_led_on_off (port, LED_ON);
				} else {
					gphy_led_status_on[port] = 0;
					gphy_data_led_on_off (port, LED_OFF);
				}
			}
		}
		ltq_ethsw_api_kclose(swithc_api_fd);
		msleep(30);
	}
}

int AR10_F2_GPHY_LED_init(void)
{
	if (!gpio_request(GPHY_2_GPIO, "SW-LED"))
		gpio_direction_output(GPHY_2_GPIO, 1);  //set gpio direction as  output
	else
		printk(KERN_EMERG "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
	if (!gpio_request(GPHY_3_GPIO, "SW-LED"))
		gpio_direction_output(GPHY_3_GPIO, 1);  //set gpio direction as  output
	else
		return 1;
	if (!gpio_request(GPHY_4_GPIO, "SW-LED"))
		gpio_direction_output(GPHY_4_GPIO, 1);  //set gpio direction as  output
	else
		return 1;
	if (!gpio_request(GPHY_5_GPIO, "SW-LED"))
		gpio_direction_output(GPHY_5_GPIO, 1);  //set gpio direction as  output
	else
		return 1;
	gphy_rmon_poll_thread_id = kthread_create(gphy_rmon_poll_thread, NULL, \
		"gphy_rmon_poll_thread");
	if (!IS_ERR(gphy_rmon_poll_thread_id)) {
		printk (KERN_EMERG "GPHY RMON poll thread created..\n");
		wake_up_process(gphy_rmon_poll_thread_id);
	}
}
//end For VRX220 SW control PHY LED

static int __init gphy_driver_init (struct platform_device *pdev)
{	
	const __be32 *no_phys;
	gphy_dev = &pdev->dev;
	gphy_node = pdev->dev.of_node;

	off_interval = 3;
	on_interval = 3;
	pw_save_mode = 0;
	dev_id = 0;
	if (of_property_read_string(pdev->dev.of_node, "fw-mode", &g_gphy_fw_mode)) {
		dev_err(&pdev->dev, "failed to read  firmware mode\n");
		return 0;
	}
	no_phys = of_get_property(gphy_node, "no_of_phys", NULL);
	if (!no_phys) {
		dev_err(&pdev->dev, "failed to get maximum number of internal gphys ports\n");
		return 0;
	}
	g_no_phys = (*no_phys);
	
	if (of_machine_is_compatible("lantiq,vr9") || of_machine_is_compatible("lantiq,xrx220") ) {
		int type = ltq_get_soc_type();
		if (type == SOC_TYPE_VR9) /*SOC_TYPE_VR9_2*/
			dev_id = 0;
		else
			dev_id = 1;
	} else if (of_machine_is_compatible("lantiq,ar10")) {
		dev_id = 1;
		no_phys = of_get_property(gphy_node, "pw_save_mode", NULL);
		if (no_phys)
			pw_save_mode = (*no_phys);
	} else if (of_machine_is_compatible("lantiq,grx390")) {
		dev_id = 1;
		no_phys = of_get_property(gphy_node, "pw_save_mode", NULL);
		if (no_phys)
			pw_save_mode = (*no_phys);
	}
	proc_file_create();
	pr_err("%s: fw_mode:%s, no of phys:%d, mode:%d\n", \
		__func__, g_gphy_fw_mode, g_no_phys, pw_save_mode);
	
	if ( pw_save_mode == 1 ) {
		swithc_api_fd = ltq_ethsw_api_kopen("/dev/switch_api/0");
		if (swithc_api_fd == 0) {
			pr_err("%s: Open SWAPI device FAILED !!\n", __func__ );
			return -EIO;
		}
		init_waitqueue_head(&gphy_pw_wait);
		gphy_pw_id = kthread_create(gphy_pw_save_thread, NULL, "gphy_pw_save");
		if (!IS_ERR(gphy_pw_id)) {
			wake_up_process(gphy_pw_id);
		}
	}
        //For VRX220 SW control PHY LED
	if (of_machine_is_compatible("lantiq,xrx220"))
	    AR10_F2_GPHY_LED_init();
	return 0;
}

static int __init gphy_driver_exit (struct platform_device *pdev)
{
	proc_file_delete();
	if ( pw_save_mode == 1 )
		ltq_ethsw_api_kclose(swithc_api_fd);
	return 0;
}

static int xway_gphy_fw_probe(struct platform_device *pdev)
{
	gphy_driver_init(pdev);
	return 0;
}

static int xway_gphy_fw_remove(struct platform_device *pdev)
{
	gphy_driver_exit(pdev);
	return 0;
}

static const struct of_device_id xway_gphy_fw_match[] = {
	{ .compatible = "lantiq,xway-phy-fw" },
	{},
};
MODULE_DEVICE_TABLE(of, xway_gphy_fw_match);

static struct platform_driver xway_gphy_driver = {
	.probe = xway_gphy_fw_probe,
	.remove = xway_gphy_fw_remove,
	.driver = {
		.name = "gphy-fw",
		.owner = THIS_MODULE,
		.of_match_table = xway_gphy_fw_match,
	},
};

module_platform_driver(xway_gphy_driver);

MODULE_AUTHOR("Reddy Mallikarjuna <reddy.mallikarjun@lantiq.com>");
MODULE_DESCRIPTION("Lantiq GPHY PHY Firmware Loader");
MODULE_LICENSE("GPL");


