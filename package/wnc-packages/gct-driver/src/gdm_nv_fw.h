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

#ifndef _GDM_NV_FW_H_
#define _GDM_NV_FW_H_

#include "gdm_endian.h"

#ifdef NV_HOST
u32 nv_restore(struct gdm_endian *ed, u8 *send_buf);
u32 nv_save(struct gdm_endian *ed, u8 *read_buf, u8 *send_buf);
#endif /* NV_HOST */

char *get_filepath_with_fw_dir(char *buf, char *filename);
char *get_filepath_with_nv_dir(char *buf, char *filename);

#endif /* _GDM_NV_FW_H_ */
