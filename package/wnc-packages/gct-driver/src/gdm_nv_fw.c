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

#include <linux/version.h>
#include <linux/syscalls.h>
#include <linux/fcntl.h>
#include <linux/uaccess.h>
#include <asm/segment.h>
#include <linux/fs.h>
#include <linux/module.h>

#include "hci.h"
#include "hci_packet.h"
#include "gdm_nv_fw.h"
#include "gdm_lte.h"
#include "gdm_dbg.h"
#include "gdm_endian.h"

#ifndef DEF_LTE_FW_DIR
#define DEF_LTE_FW_DIR          "/etc/gct/"
#endif

#ifndef DEF_LTE_NV_DIR
#define DEF_LTE_NV_DIR          "/etc/gct/nv_data/"
#endif

char fw_dir[MAX_FILE_PATH] = DEF_LTE_FW_DIR;
char nv_dir[MAX_FILE_PATH] = DEF_LTE_NV_DIR;


static char *get_full_path(char *buf, char *path, char *filename)
{
	int path_len;

	path_len = strlen(path);

	strcpy(buf, path);
	if ('/' != path[path_len - 1])
		strcat(buf, "/");
	strcat(buf, filename);

	return buf;
}


char *get_filepath_with_fw_dir(char *buf, char *filename)
{
	if (!buf || !filename)
		return NULL;

	return get_full_path(buf, fw_dir, filename);
}

char *get_filepath_with_nv_dir(char *buf, char *filename)
{
	if (!buf || !filename)
		return NULL;

	return get_full_path(buf, nv_dir, filename);
}


#ifdef NV_HOST

#define LTE_INFO_NV		"lte_info.nv_data"
#define BOARD_CONF_NV	"board_conf.nv_data"
#define RF_CAL_NV		"rf_cal.nv_data"
#define TEMP_NV			"temp.nv_data"
#define NET_INFO_NV		"net_info.nv_data"
#define SAFETY_INFO_NV	"safety_info.nv_data"
#define CDMA_CAL_NV		"cdma_cal.nv_data"
#define VENDOR_NV		"vendor.nv_data"

#define MAX_NV_SIZE		(1024*30)

struct nv_data {
	u16 magic;
	u8 version;
	u8 crc;
	u8 data[0];
};

struct nv_tlv {
	u8 type;
	u32 len;
	u8 data[0];
} __packed;


static char filepath_lte_info[MAX_FILE_PATH];
static char filepath_board_conf[MAX_FILE_PATH];
static char filepath_rf_cal[MAX_FILE_PATH];
static char filepath_temp[MAX_FILE_PATH];
static char filepath_net_info[MAX_FILE_PATH];
static char filepath_safety_info[MAX_FILE_PATH];
static char filepath_vendor[MAX_FILE_PATH];
static char filepath_cdma_cal[MAX_FILE_PATH];

static const unsigned char crc8_tab[256] = {
	0x00, 0x07, 0x0E, 0x09, 0x1C, 0x1B, 0x12, 0x15,
	0x38, 0x3F, 0x36, 0x31, 0x24, 0x23, 0x2A, 0x2D,
	0x70, 0x77, 0x7E, 0x79, 0x6C, 0x6B, 0x62, 0x65,
	0x48, 0x4F, 0x46, 0x41, 0x54, 0x53, 0x5A, 0x5D,
	0xE0, 0xE7, 0xEE, 0xE9, 0xFC, 0xFB, 0xF2, 0xF5,
	0xD8, 0xDF, 0xD6, 0xD1, 0xC4, 0xC3, 0xCA, 0xCD,
	0x90, 0x97, 0x9E, 0x99, 0x8C, 0x8B, 0x82, 0x85,
	0xA8, 0xAF, 0xA6, 0xA1, 0xB4, 0xB3, 0xBA, 0xBD,
	0xC7, 0xC0, 0xC9, 0xCE, 0xDB, 0xDC, 0xD5, 0xD2,
	0xFF, 0xF8, 0xF1, 0xF6, 0xE3, 0xE4, 0xED, 0xEA,
	0xB7, 0xB0, 0xB9, 0xBE, 0xAB, 0xAC, 0xA5, 0xA2,
	0x8F, 0x88, 0x81, 0x86, 0x93, 0x94, 0x9D, 0x9A,
	0x27, 0x20, 0x29, 0x2E, 0x3B, 0x3C, 0x35, 0x32,
	0x1F, 0x18, 0x11, 0x16, 0x03, 0x04, 0x0D, 0x0A,
	0x57, 0x50, 0x59, 0x5E, 0x4B, 0x4C, 0x45, 0x42,
	0x6F, 0x68, 0x61, 0x66, 0x73, 0x74, 0x7D, 0x7A,
	0x89, 0x8E, 0x87, 0x80, 0x95, 0x92, 0x9B, 0x9C,
	0xB1, 0xB6, 0xBF, 0xB8, 0xAD, 0xAA, 0xA3, 0xA4,
	0xF9, 0xFE, 0xF7, 0xF0, 0xE5, 0xE2, 0xEB, 0xEC,
	0xC1, 0xC6, 0xCF, 0xC8, 0xDD, 0xDA, 0xD3, 0xD4,
	0x69, 0x6E, 0x67, 0x60, 0x75, 0x72, 0x7B, 0x7C,
	0x51, 0x56, 0x5F, 0x58, 0x4D, 0x4A, 0x43, 0x44,
	0x19, 0x1E, 0x17, 0x10, 0x05, 0x02, 0x0B, 0x0C,
	0x21, 0x26, 0x2F, 0x28, 0x3D, 0x3A, 0x33, 0x34,
	0x4E, 0x49, 0x40, 0x47, 0x52, 0x55, 0x5C, 0x5B,
	0x76, 0x71, 0x78, 0x7F, 0x6A, 0x6D, 0x64, 0x63,
	0x3E, 0x39, 0x30, 0x37, 0x22, 0x25, 0x2C, 0x2B,
	0x06, 0x01, 0x08, 0x0F, 0x1A, 0x1D, 0x14, 0x13,
	0xAE, 0xA9, 0xA0, 0xA7, 0xB2, 0xB5, 0xBC, 0xBB,
	0x96, 0x91, 0x98, 0x9F, 0x8A, 0x8D, 0x84, 0x83,
	0xDE, 0xD9, 0xD0, 0xD7, 0xC2, 0xC5, 0xCC, 0xCB,
	0xE6, 0xE1, 0xE8, 0xEF, 0xFA, 0xFD, 0xF4, 0xF3
};     /* 8-bit table */

static unsigned char crc8_accumulate(unsigned char crc8val, const unsigned char *s, int len)
{
	int i;

	for (i = 0;  i < len;  i++)
		crc8val = crc8_tab[(crc8val) ^ s[i]];

	return crc8val;
}


static void nv_load_filepath(void)
{
	static int once;

	if (!once) {
		get_filepath_with_nv_dir(filepath_lte_info, LTE_INFO_NV);
		get_filepath_with_nv_dir(filepath_board_conf, BOARD_CONF_NV);
		get_filepath_with_nv_dir(filepath_rf_cal, RF_CAL_NV);
		get_filepath_with_nv_dir(filepath_temp, TEMP_NV);
		get_filepath_with_nv_dir(filepath_net_info, NET_INFO_NV);
		get_filepath_with_nv_dir(filepath_safety_info, SAFETY_INFO_NV);
		get_filepath_with_nv_dir(filepath_cdma_cal, CDMA_CAL_NV);
		get_filepath_with_nv_dir(filepath_vendor, VENDOR_NV);
		once = 1;
	}
}


static int restore_nv_to_device(struct gdm_endian *ed, unsigned char *data, const char *path, u8 type)
{
	char *buf;
	struct file *file;
	mm_segment_t old_fs = get_fs();
	unsigned long long pos = 0;
	int ret = -1;
	struct hci_nv_restore_rsp *nv_rsp = (struct hci_nv_restore_rsp *)data;

	buf = kmalloc(MAX_NV_SIZE, GFP_ATOMIC);
	if (!buf) {
		drv_print(LTE_LOG_ERROR, "%s kmalloc failed\n", __FUNCTION__);
		return ret;
	}

	set_fs(get_ds());

	file = filp_open(path, O_RDONLY, 0);
	if (IS_ERR(file)) {
		drv_print(LTE_LOG_ERROR, "restore nv : no such file %s\n", path);
	} else {
		struct nv_data *nv_data;

		ret = vfs_read(file, buf, MAX_NV_SIZE, &pos);
		memcpy(nv_rsp->data, buf, ret);

		nv_rsp->type = type;
		nv_rsp->len = H4D(ed, (u32)ret);

		if(type != NV_TYPE_OF_VENDOR) {
			nv_data = (struct nv_data *)nv_rsp->data;

			nv_data->crc = crc8_accumulate(0,
										nv_data->data,
										ret - offsetof(struct nv_data, data)
										);
			drv_print(LTE_LOG_FUNCTION, "file size %d crc %d\n", ret, nv_data->crc);
		}

		ret += offsetof(struct hci_nv_restore_rsp, data);
	}

	kfree(buf);

	set_fs(old_fs);

	if (!IS_ERR(file))
		filp_close(file, NULL);

	return ret;
}

u32 nv_restore(struct gdm_endian *ed, u8 *send_buf)
{
	struct hci_packet *hci = (struct hci_packet *)send_buf;
	u32 offset = 0;
	int ret;

	nv_load_filepath();

	hci->cmd_evt = H2D(ed, LTE_NV_RESTORE_RESPONSE);

	do {
		ret = restore_nv_to_device(ed, hci->data+offset, filepath_lte_info, NV_TYPE_OF_LTE_INFO);
		if (ret < 0) {
			offset = 0;
			break;
		} else {
			offset += ret;
		}

		ret = restore_nv_to_device(ed, hci->data+offset, filepath_board_conf, NV_TYPE_OF_BOARD_CONFIG);
		if (ret < 0) {
			offset = 0;
			break;
		} else {
			offset += ret;
		}

		ret = restore_nv_to_device(ed, hci->data+offset, filepath_rf_cal, NV_TYPE_OF_RF_CALIBRATION);
		if (ret < 0) {
			offset = 0;
			break;
		} else {
			offset += ret;
		}

		ret = restore_nv_to_device(ed, hci->data+offset, filepath_temp, NV_TYPE_OF_TEMP_CHARACTERISTICS);
		if (ret < 0) {
			offset = 0;
			break;
		} else {
			offset += ret;
		}

		ret = restore_nv_to_device(ed, hci->data+offset, filepath_net_info, NV_TYPE_OF_NET_INFO);
		if (ret < 0) {
			offset = 0;
			break;
		} else {
			offset += ret;
		}

		ret = restore_nv_to_device(ed, hci->data+offset, filepath_safety_info, NV_TYPE_OF_SAFTY_INFO);
		if (ret < 0) {
			offset = 0;
			break;
		} else {
			offset += ret;
		}

		ret = restore_nv_to_device(ed, hci->data+offset, filepath_cdma_cal, NV_TYPE_OF_CDMA_CAL);
		if (ret < 0)
			/*
			offset = 0;
			Returning offset = 0 will result in F/W NV to default.
			*/
			break;
		else
			offset += ret;

		ret = restore_nv_to_device(ed, hci->data+offset, filepath_vendor, NV_TYPE_OF_VENDOR);
		if (ret < 0)
			/*
			offset = 0;
			Returning offset = 0 will result in F/W NV to default.
			*/
			break;
		else
			offset += ret;
	} while (0);

	hci->len = H2D(ed, (u16)offset);

	return offset + HCI_HEADER_SIZE;
}

static s32 save_nv_to_file(struct gdm_endian *ed, struct nv_tlv *tlv)
{
	mm_segment_t old_fs = get_fs();
	struct file *file;
	s32 ret = 0;

	set_fs(get_ds());

	switch (tlv->type) {
	case NV_TYPE_OF_LTE_INFO:
		file = filp_open(filepath_lte_info, O_RDWR | O_CREAT, 0);
		break;
	case NV_TYPE_OF_BOARD_CONFIG:
		file = filp_open(filepath_board_conf, O_RDWR | O_CREAT, 0);
		break;
	case NV_TYPE_OF_RF_CALIBRATION:
		file = filp_open(filepath_rf_cal, O_RDWR | O_CREAT, 0);
		break;
	case NV_TYPE_OF_TEMP_CHARACTERISTICS:
		file = filp_open(filepath_temp, O_RDWR | O_CREAT, 0);
		break;
	case NV_TYPE_OF_NET_INFO:
		file = filp_open(filepath_net_info, O_RDWR | O_CREAT, 0);
		break;
	case NV_TYPE_OF_SAFTY_INFO:
		file = filp_open(filepath_safety_info, O_RDWR | O_CREAT, 0);
		break;
	case NV_TYPE_OF_CDMA_CAL:
		file = filp_open(filepath_cdma_cal, O_RDWR | O_CREAT, 0);
		break;
	case NV_TYPE_OF_VENDOR:
		file = filp_open(filepath_vendor, O_RDWR | O_CREAT, 0);
		break;
	default:
		drv_print(LTE_LOG_ERROR, "wrong nv type %d\n", tlv->type);
		ret = -1;
		return ret;
	}

	if (IS_ERR(file))
		drv_print(LTE_LOG_ERROR, "save nv : no such file\n");
	else
		ret = vfs_write(file, tlv->data, D4H(ed, tlv->len), &file->f_pos);

	if (!IS_ERR(file))
		filp_close(file, NULL);

	set_fs(old_fs);

	drv_print(LTE_LOG_NOTICE, "NV save for type %d: written %d bytes\n", tlv->type, ret);

	if (ret < 0)
		return ret;

	return D4H(ed, tlv->len) + offsetof(struct nv_tlv, data);
}

u32 nv_save(struct gdm_endian *ed, u8 *read_buf, u8 *send_buf)
{
	struct hci_packet *hci_send = (struct hci_packet *)send_buf;
	struct hci_packet *hci_read = (struct hci_packet *)read_buf;
	s32 status = 0;
	int offset = 0;
	struct nv_tlv *tlv;
	u32 *nv_status = (u32 *)hci_send->data;
	int ret = 0;

	while (offset < D2H(ed, hci_read->len)) {
		tlv = (struct nv_tlv *)(hci_read->data + offset);
		ret = save_nv_to_file(ed, tlv);

		if (ret < 0) {
			status = 1;
			break;
		} else {
			offset += ret;
		}
	}

	if (D2H(ed, hci_read->len) != offset)
		drv_print(LTE_LOG_ERROR, "NV Save error: HCI length=%d, Written=%d\n", D2H(ed, hci_read->len), offset);

	hci_send->cmd_evt = H2D(ed, LTE_NV_SAVE_RESPONSE);
	hci_send->len = H2D(ed, 4);
	*nv_status = H4D(ed, status);

	return HCI_HEADER_SIZE + 4;
}

#endif

module_param_string(fw_dir, fw_dir, MAX_FILE_PATH, 0644);
module_param_string(nv_dir, nv_dir, MAX_FILE_PATH, 0644);
MODULE_PARM_DESC(fw_dir, "Firwmare directory (default: " DEF_LTE_FW_DIR ")");
MODULE_PARM_DESC(nv_dir, "NV directory (default: " DEF_LTE_NV_DIR ")");

