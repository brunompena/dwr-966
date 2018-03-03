/*
 * Copyright (c) 2012 GCT Semiconductor, Inc. All rights reserved.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/slab.h>

#include <linux/mmc/core.h>
#include <linux/mmc/card.h>
#include <linux/mmc/sdio_func.h>
#include <linux/delay.h>

#include "gdm_lte.h"
#include "gdm_dbg.h"
#include "gdm_nv_fw.h"

/*
 * SDIO Boot debugging options
 */

/* #define SDIO_BOOT_DBG */
/* #define SDIO_BOOT_PROFILING */

#define RESENDING_CTRLBLK

#if defined(SDIO_BOOT_PROFILING)
#define tick2ms(t)	((t)*1000/HZ)
static u32 sync_elapsed;
#endif

#if defined(SDIO_BOOT_DBG)
#define dprintk(fmt, args ...)	drv_print(LTE_LOG_NOTICE, fmt, ## args)
#define iprintk(fmt, args ...)	drv_print(LTE_LOG_NOTICE, fmt, ## args)
#else
#define dprintk(...)
#define iprintk(...)
#endif
#define eprintk(fmt, args ...)	drv_print(LTE_LOG_ERROR, fmt, ## args)
#define nprintk(fmt, args ...)	drv_print(LTE_LOG_NOTICE, fmt, ## args)

#define SDIO_TX_BUF_SIZE			(32*1024)

#define SPBC_CTBL_OFFS			0
#define SPBC_CTBL_SIZE			512
#define SPBC_HDR_OFFS			(SPBC_CTBL_SIZE)
#define SPBC_HDR_SIZE			(580)
#define SPBC_BODY_OFFS			(SPBC_HDR_OFFS+SPBC_HDR_SIZE)
#define IMG_HDR_OFFS			0

#ifndef MAX_FILE_PATH
#define MAX_FILE_PATH 260
#endif

#define SD_INTERRUPT_STATUS		0x2c
#define INT_EN_REG_WR_TRAN_OVER		(0x01)

#define HOST_GPR				0x44
#define HOST_DATA				0x48
#define LTE_GPR					0x4C
#define LTE_DATA				0x50

#define PBL_INITIALIZED			0x5A	/*Initialized*/
#define PBL_READY				0x5B	/*Ready to data transfer*/
#define PBL_CTLBL				0x5C	/*SDRAM Control block transmission*/
#define PBL_HDR					0x5D	/*Header transmission*/
#define PBL_PBC					0x5E	/*PBC transmission*/
#define PBL_DONE				0x5F	/*Transfer done*/
#define PBL_ERROR				0xA5	/*Error*/

#define BL_INITIALIZED			0x7A	/*Initialized*/
#define BL_READY				0x7B	/*Ready to data transfer*/
#define BL_HDRSTART				0x7C	/*Header transmission*/
#define BL_IMGSTART				0x7D	/*Image block transmission*/
#define BL_DONE					0x7E	/*Image block done*/
#define BL_PREHDR				0x85	/*Header size transfer*/
#define BL_ERROR				0xA5	/*Error*/

#if defined(SDIO_BOOT_DBG)
#define cmd_ack_delay()			msleep(100)
#define data_ack_delay()		msleep(100)
#else
#define cmd_ack_delay()			schedule()
#define data_ack_delay()		schedule()
#endif
#define sync_delay()			msleep(1)	/*BootROM's requirement!!*/

#define ETIMEDOUT_ACK			1100	/*Ack timed out*/

struct record_hdr_t {
	u32 base;
	u32 length;
	u32 unknown;
	u32 checksum;
	u32 unknown2[7];

};

struct vrl_t {
	u32 magic;
	u32 hdr_size;
	u32 reserved[3];
	u32 records;
	struct record_hdr_t record_hdr[12];

};/*Verified Record List*/

static u8 *sdio_tx_buf;

#if defined(SDIO_BOOT_DBG)
static void print_sdio_cccr(struct sdio_cccr *cccr)
{
	dprintk("[SDIO CCCR]\n");
	dprintk("\t sdio_vsn=%d\n", cccr->sdio_vsn);
	dprintk("\t sd_vsn=%d\n", cccr->sd_vsn);
	dprintk("\t multi_block=%d\n", cccr->multi_block);
	dprintk("\t low_speed=%d\n", cccr->low_speed);
	dprintk("\t wide_bus=%d\n", cccr->wide_bus);
	dprintk("\t high_power=%d\n", cccr->high_power);
	dprintk("\t high_speed=%d\n", cccr->high_speed);
}

static void print_sdio_cis(struct sdio_cis *cis)
{
	dprintk("[SDIO CIS]\n");
	dprintk("\t vendor=0x%04x\n", cis->vendor);
	dprintk("\t device=0x%04x\n", cis->device);
	dprintk("\t blksize=%d\n", cis->blksize);
	dprintk("\t max_dtr=%d\n", cis->max_dtr);
}

static void print_sdio_card(struct mmc_card *card)
{
	print_sdio_cccr(&card->cccr);
	print_sdio_cis(&card->cis);
}

static void print_sdio_func(struct sdio_func *func)
{
	dprintk("[SDIO FUNC]\n");
	dprintk("\t cur_blksize=%d\n", func->cur_blksize);
	dprintk("\t max_blksize=%d\n", func->max_blksize);
	dprintk("\t vendor=0x%04x\n", func->vendor);
	dprintk("\t device=0x%04x\n", func->device);
	dprintk("\t enable_timeout=%dms\n", func->enable_timeout);

	if (func->card)
		print_sdio_card(func->card);
}
#endif

static u8 do_sdio_readb(struct sdio_func *func, unsigned int addr, int *ret)
{
	u8 val;

	val = sdio_readb(func, addr, ret);
	if (ret && *ret)
		eprintk("sdio_readb failure: addr=0x%02x, ret=%d\n", addr, *ret);
	return val;
}

static void do_sdio_writeb(struct sdio_func *func, u8 b, unsigned int addr, int *ret)
{
	sdio_writeb(func, b, addr, ret);
	if (ret && *ret)
		eprintk("sdio_writeb failure: addr=0x%02x, b=0x%02x, ret=%d\n", addr, b, *ret);
}

static int wait_for_value(struct sdio_func *func, u8 offs, u8 value, int timeout_sec)
{
	unsigned long start = jiffies;
	u8 val;
	int ret;

	while ((jiffies - start) < (timeout_sec*HZ)) {
		val = do_sdio_readb(func, offs, &ret);
		if (ret < 0)
			return ret;
		if (val == value)
			return 0;
		cmd_ack_delay();
	}
	eprintk("%s timeout: offs=0x%02x, value=0x%02x, timeout=%d\n",
		__FUNCTION__, offs, value, timeout_sec);
	return -ETIMEDOUT_ACK;
}

static int gpr_send_wait_value(struct sdio_func *func, u8 status, int timeout_sec)
{
	int ret = 0;

	do_sdio_writeb(func, status, HOST_GPR, &ret);
	if (ret < 0)
		return ret;

	ret = wait_for_value(func, LTE_GPR, status, timeout_sec);
	return ret;
}

static int ack_ready(struct sdio_func *func)
{
	#define ACK_TIMEOUT	(1*HZ)
	unsigned long start = jiffies;
	u8 val;
	int ret;

	while ((jiffies - start) < ACK_TIMEOUT) {
		val = do_sdio_readb(func, SD_INTERRUPT_STATUS, &ret);
		if (ret < 0)
			return ret;
		if (!(val & INT_EN_REG_WR_TRAN_OVER))
			return 0;
		data_ack_delay();
	}
	eprintk("ACK is not ready!\n");
	return -EIO;
}

/* FILE Interface */
#define FILE_SEEK_SET		0
#define FILE_SEEK_CUR		1
#define FILE_SEEK_END		2

struct file_t {
	struct file *filp;
	loff_t ofs;
	int size;

};

static void file_close(struct file_t *file)
{
	if (file->filp) {
		filp_close(file->filp, current->files);
		file->filp = NULL;
	}
}

static int file_open(struct file_t *file, const char *file_path)
{
	struct inode *inode = NULL;
	int ret = 0;

	file->filp = filp_open(file_path, O_RDONLY | O_LARGEFILE, 0);
	if (IS_ERR(file->filp)) {
		eprintk("Can't find %s.\n", file_path);
		return -ENOENT;
	}

	if (file->filp->f_dentry)
		inode = file->filp->f_dentry->d_inode;
	if (!inode || !S_ISREG(inode->i_mode)) {
		eprintk("Invalid file type: %s\n", file_path);
		return -EINVAL;
	}

	ret = (int)i_size_read(inode->i_mapping->host);
	if (ret <= 0)
		eprintk("Unable to find file size: %s(%d)\n", file_path, ret);
	else {
		file->size = ret;
		ret = 0;
	}

	if (ret && file->filp) {
		file_close(file);
		file->filp = NULL;
	}

	file->ofs = 0;
	dprintk("%s(%s):%d\n", __FUNCTION__, file_path, ret);
	return ret;
}

static int file_read(struct file_t *file, void *buf, int size)
{
	return file->filp->f_op->read(file->filp, buf, size, &file->ofs);
}

static int file_seek(struct file_t *file, loff_t ofs, int whence)
{
	switch (whence) {
	case FILE_SEEK_SET:
		file->ofs = ofs;
		break;
	case FILE_SEEK_CUR:
		file->ofs += ofs;
		break;
	case FILE_SEEK_END:
		file->ofs = file->size + ofs;
		break;
	}
	return file->ofs;
}

static int do_sdio_memcpy_toio(struct sdio_func *func, void *buf, int size)
{
	int ret;

	ret = ack_ready(func);
	if (ret < 0)
		goto out;

	dprintk("sdio_memcpy_toio: len=%d\n", size);
	ret = sdio_memcpy_toio(func, 0, buf, size);
	if (ret < 0) {
		if (ret == -EILSEQ) {
			nprintk("Ignore CRC error!\n");
			ret = 0;
		} else {
			eprintk("toio error: ret=%d\n", ret);
		}
	}
out:
	return ret;
}

static int sdio_send_data(struct sdio_func *func, void *buf, int size)
{
	u8 *p = (u8 *)buf;
	int ret = 0, no_aligned, aligned;

	no_aligned = size & (func->cur_blksize-1);
	aligned = size - no_aligned;

	if (aligned) {
		ret = do_sdio_memcpy_toio(func, p, aligned);
		p += aligned;
	}

	if (!ret && no_aligned)
		ret = do_sdio_memcpy_toio(func, p, no_aligned);

	return ret;
}

static int file2sdio(struct file_t *file, struct sdio_func *func, int size, loff_t ofs)
{
	int pno, ret = 0, len, total = 0, readn;
	u8 *buf = sdio_tx_buf;

	file_seek(file, ofs, FILE_SEEK_SET);

	pno = 0;
	while (total < size) {
		readn = (size-total) > SDIO_TX_BUF_SIZE ? SDIO_TX_BUF_SIZE : (size-total);
		len = file_read(file, buf, readn);
		if (len < 0) {
			ret = -1;
			break;
		}

		ret = sdio_send_data(func, buf, len);
		if (ret < 0)
			break;

		total += len;
		pno++;
		dprintk("pno=%d, total=%d\n", pno, total);
	}

	if (!ret)
		ret = ack_ready(func);

	if (total < size) {
		eprintk("Data size mismatch(%d<%d)\n", total, size);
		if (!ret)
			ret = -EIO;
	}
	dprintk("%s: ofs=0x%x, size(%d/%d), ret=%d\n",
		__FUNCTION__, (int)ofs, total, size, ret);
	return ret;
}

static int parse_vrl(struct file_t *file, struct vrl_t *vrl, int offs)
{
	int ret, i;

	file_seek(file, offs, FILE_SEEK_SET);
	ret = file_read(file, vrl, sizeof(struct vrl_t));
	if (ret != sizeof(struct vrl_t)) {
		eprintk("Read VRL error: ret=%d\n", ret);
		if (ret >= 0)
			ret = -1;
	} else {
		iprintk("[VRL]\n");
		vrl->magic = L4H(vrl->magic);
		iprintk("Magic: 0x%08x\n", vrl->magic);
		vrl->hdr_size = L4H(vrl->hdr_size);
		iprintk("HDR Size: 0x%08x\n", vrl->hdr_size);
		vrl->records = L4H(vrl->records);
		iprintk("Records: %d\n", vrl->records);
		iprintk("       Base        Length      Checksum\n");
		for (i = 0; i < vrl->records; i++) {
			vrl->record_hdr[i].base = L4H(vrl->record_hdr[i].base);
			vrl->record_hdr[i].length = L4H(vrl->record_hdr[i].length);
			vrl->record_hdr[i].length <<= 2;
			vrl->record_hdr[i].checksum = L4H(vrl->record_hdr[i].checksum);
			iprintk("  [%d]  0x%08x  0x%08x  0x%04x\n", i,
				vrl->record_hdr[i].base,
				vrl->record_hdr[i].length,
				(u16)vrl->record_hdr[i].checksum);
		}
		ret = 0;
	}
	return ret;
}

static int sync_sdio(struct sdio_func *func, u8 status_inited, u8 status_ready)
{
	int ret = 0, i, retry = 2;
	#if defined(SDIO_BOOT_PROFILING)
	u32 start, end;
	#endif

	ret = gpr_send_wait_value(func, status_inited, 10);
	if (ret < 0)
		goto out;

	#if defined(SDIO_BOOT_PROFILING)
	start = jiffies;
	#endif
	for (i = 0; i < retry; i++) {
		sync_delay();

		ret = gpr_send_wait_value(func, status_ready, 1);
		if (!ret)
			break;

		do_sdio_writeb(func, status_inited, HOST_GPR, &ret);
		if (ret < 0)
			break;
	}
	#if defined(SDIO_BOOT_PROFILING)
	end = jiffies;
	sync_elapsed = end - start;
	#endif
out:
	dprintk("%s: 0x%02x->0x%02x: ret=%d\n",
		__FUNCTION__, status_inited, status_ready, ret);
	return ret;
}

static int do_send_ctrlblk(struct sdio_func *func, struct file_t *file)
{
	int ret = 0, size;
	loff_t pos = 0;

	pos = SPBC_CTBL_OFFS;
	size = SPBC_CTBL_SIZE;
	ret = file2sdio(file, func, size, pos);
	return ret;
}

static int send_ctrlbl(struct sdio_func *func, struct file_t *file)
{
	int ret = 0;

	ret = gpr_send_wait_value(func, PBL_CTLBL, 1);
	if (ret < 0)
		goto out;

	ret = do_send_ctrlblk(func, file);
out:
	dprintk("%s: ret=%d\n", __FUNCTION__, ret);
	return ret;
}

static int send_pbc_hdr(struct sdio_func *func, struct file_t *file)
{
	int ret = 0, size;
	loff_t pos = 0;

	ret = gpr_send_wait_value(func, PBL_HDR, 1);
	if (ret < 0)
		goto out;

	pos = SPBC_HDR_OFFS;
	size = SPBC_HDR_SIZE;
	ret = file2sdio(file, func, size, pos);
out:
	dprintk("%s: ret=%d\n", __FUNCTION__, ret);
	return ret;
}

static int send_pbc_body(struct sdio_func *func, struct file_t *file)
{
	int ret = 0, size;
	loff_t pos = 0;
	int i, retry = 1;
	struct vrl_t vrl;

	ret = parse_vrl(file, &vrl, SPBC_HDR_OFFS);
	if (ret < 0)
		goto out;

	ret = gpr_send_wait_value(func, PBL_PBC, 1);
	if (ret < 0)
		goto out;

	for (i = 0; i < retry; i++) {
		pos = SPBC_BODY_OFFS;
		size = vrl.record_hdr[0].length;
		dprintk("Send PBC Record[%d]: base=0x%08x, length=0x%x\n",
			i, vrl.record_hdr[0].base, vrl.record_hdr[0].length);
		ret = file2sdio(file, func, size, pos);
		if (!ret) {
			ret = gpr_send_wait_value(func, PBL_DONE, 1);
			if (!ret)
				break;
		}
	}
out:
	dprintk("%s: ret=%d\n", __FUNCTION__, ret);
	return ret;
}

static int download_spbc(struct sdio_func *func, const char *spbc_path)
{
	struct file_t file = {NULL, 0, 0};
	int ret;
	#if defined(RESENDING_CTRLBLK)
	int i, retry = 10;
	#endif
	#if defined(SDIO_BOOT_PROFILING)
	u32 start, end;
	#endif

	ret = file_open(&file, spbc_path);
	#if defined(SDIO_BOOT_PROFILING)
	start = jiffies;
	#endif
	if (!ret)
		ret = sync_sdio(func, PBL_INITIALIZED, PBL_READY);
	if (!ret)
		ret = send_ctrlbl(func, &file);
	if (!ret) {
		#if defined(RESENDING_CTRLBLK)
		for (i = 0; i < retry; i++) {
			ret = send_pbc_hdr(func, &file);
			if (ret != -ETIMEDOUT_ACK)
				break;
			nprintk("Re-send(%d) control block!\n", i+1);
			ret = do_send_ctrlblk(func, &file);
			if (ret  < 0)
				break;
		}
		if (i == retry) {
			eprintk("Too many retries(%d/%d)!\n", i, retry);
			ret = -1;
		}
		#else
		ret = send_pbc_hdr(func, &file);
		#endif
	}
	if (!ret)
		ret = send_pbc_body(func, &file);
	#if defined(SDIO_BOOT_PROFILING)
	end = jiffies;
	dprintk("PBC start ~ PBC end: %dms.\n", tick2ms(end-start));
	#endif
	if (ret) {
		eprintk("Send error: ret=%d\n", ret);
		do_sdio_writeb(func, PBL_ERROR, HOST_GPR, NULL);
	}
	if (file.filp)
		file_close(&file);
	dprintk("%s: ret=%d\n", __FUNCTION__, ret);
	return ret;
}

static int send_img_hdr(struct sdio_func *func, struct file_t *file, struct vrl_t *vrl)
{
	int ret = 0, size;
	loff_t pos = 0;

	size = vrl->hdr_size * 4/*in word(4 bytes)*/;
	dprintk("Header size=%d\n", size);
	/*Send size of header*/
	do_sdio_writeb(func, (u8)(vrl->hdr_size), HOST_DATA, &ret);
	if (ret < 0)
		goto out;

	ret = gpr_send_wait_value(func, BL_PREHDR, 1);
	if (ret < 0)
		goto out;

	ret = gpr_send_wait_value(func, BL_HDRSTART, 1);
	if (ret < 0)
		goto out;

	pos = IMG_HDR_OFFS;
	ret = file2sdio(file, func, size, pos);
out:
	dprintk("%s: ret=%d\n", __FUNCTION__, ret);
	return ret;
}

static int send_img_body(struct sdio_func *func, struct file_t *file, struct vrl_t *vrl)
{
	int ret = 0, size, i;
	loff_t pos = 0;

	pos = vrl->hdr_size * 4/*in word(4 bytes)*/;
	for (i = 0; i < vrl->records; i++) {
		ret = gpr_send_wait_value(func, BL_IMGSTART, 1);
		if (ret < 0)
			break;

		dprintk("Send Image Record[%d]: base=0x%08x, length=0x%x\n",
			i, vrl->record_hdr[i].base, vrl->record_hdr[i].length);
		size = vrl->record_hdr[i].length;
		ret = file2sdio(file, func, size, pos);
		if (ret < 0)
			break;
		pos += size;

		ret = gpr_send_wait_value(func, BL_READY, 1);
		if (ret < 0)
			break;
	}

	if (!ret) {
		dprintk("Send DONE\n");
		ret = gpr_send_wait_value(func, BL_DONE, 1);
	}
	dprintk("%s: ret=%d\n", __FUNCTION__, ret);
	return ret;
}

static int download_cpuimg(struct sdio_func *func, const char *cpuimg_path)
{
	struct file_t file = {NULL, 0, 0};
	int ret;
	struct vrl_t vrl;
	#if defined(SDIO_BOOT_PROFILING)
	u32 start, end, start2, end2;
	start = jiffies;
	#endif

	ret = file_open(&file, cpuimg_path);
	if (!ret)
		ret = sync_sdio(func, BL_INITIALIZED, BL_READY);
	#if defined(SDIO_BOOT_PROFILING)
	end = jiffies;
	start2 = jiffies;
	#endif

	if (!ret)
		ret = parse_vrl(&file, &vrl, IMG_HDR_OFFS);
	if (!ret)
		ret = send_img_hdr(func, &file, &vrl);
	if (!ret)
		ret = send_img_body(func, &file, &vrl);
	#if defined(SDIO_BOOT_PROFILING)
	end2 = jiffies;
	dprintk("PBC end ~ Img start: %dms.\n", tick2ms(end-start));
	dprintk("Img start ~ Img end: %dms.\n", sync_elapsed + (end2-start2));
	#endif
	if (ret) {
		eprintk("Send error: ret=%d\n", ret);
		do_sdio_writeb(func, BL_ERROR, HOST_GPR, NULL);
	}

	if (file.filp)
		file_close(&file);
	dprintk("%s: ret=%d\n", __FUNCTION__, ret);
	return ret;
}

int sdio_boot(struct sdio_func *func)
{
	static mm_segment_t fs;
	int ret;
	#if defined(SDIO_BOOT_PROFILING)
	u32 start, end;
	#endif
	char spbc_path[MAX_FILE_PATH];
	char simg_path[MAX_FILE_PATH];

	sdio_tx_buf = kmalloc(SDIO_TX_BUF_SIZE, GFP_KERNEL);
	if (sdio_tx_buf == NULL) {
		eprintk("Error: kmalloc: %s\n", __FUNCTION__);
		return -ENOMEM;
	}

	fs = get_fs();
	set_fs(get_ds());

	#if defined(SDIO_BOOT_DBG)
	print_sdio_func(func);
	#endif

	#if defined(SDIO_BOOT_PROFILING)
	start = jiffies;
	#endif
	ret = download_spbc(func, get_filepath_with_fw_dir(spbc_path, SPBC_PATH));
	if (!ret) {
		nprintk("lte: SPBC download success.\n");
		ret = download_cpuimg(func, get_filepath_with_fw_dir(simg_path, SIMG_PATH));
		if (!ret)
			nprintk("lte: CPU Image download success.\n");
		else
			nprintk("lte: CPU Image download FAILURE.\n");
	} else {
		nprintk("lte: SPBC download FAILURE.\n");
	}
#if defined(SDIO_BOOT_PROFILING)
	end = jiffies;
	dprintk("Total elapsed time: %dms.\n", tick2ms(end-start));
#endif

	set_fs(fs);
	kfree(sdio_tx_buf);
	dprintk("%s: ret=%d\n", __FUNCTION__, ret);
	return ret;
}
