/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Author: Leonid Yegoshin (yegoshin@mips.com)
 * Copyright (C) 2012 MIPS Technologies, Inc.
 */

#ifndef _ASM_LANTIQ_SPACES_H
#define _ASM_LANTIQ_SPACES_H

#ifdef CONFIG_EVA

#define PAGE_OFFSET             _AC(0x0, UL)
/* skip first 512MB */
#define PHYS_OFFSET             _AC(0x20000000, UL)
#define HIGHMEM_START           _AC(0xffff0000, UL)

/*  I put INDEX_BASE here to underline the fact that in EVA mode kernel
    may be located somethere and not in CKSEG0, so CKSEG0 may have
    a "surprise" location and index-based CACHE may give unexpected result */
#define INDEX_BASE      CKSEG0

#define UNCAC_BASE              _AC(0xa0000000, UL)
#define CAC_BASE	             _AC(0x0, UL)
#define IO_BASE                 UNCAC_BASE

#define KSEG
#define KUSEG                   0x00000000
#define KSEG0                   0x80000000
#define KSEG3                   0xa0000000
#define KSEG2                   0xc0000000
#define KSEG1                   0xe0000000

#define CKUSEG                  0x00000000
#define CKSEG0                  0x80000000
#define CKSEG3                  0xa0000000
#define CKSEG2                  _AC(0xc0000000, UL)
#define CKSEG1                  0xe0000000

#define MAP_BASE                CKSEG2
#define VMALLOC_START 					MAP_BASE
#define VMALLOC_END             (MAP_BASE + _AC(0x20000000, UL) - 2*PAGE_SIZE)


#define IO_SIZE                 _AC(0x10000000, UL)
#define IO_SHIFT                _AC(0x10000000, UL)

#endif  /* CONFIG_EVA */

#include <asm/mach-generic/spaces.h>

#endif /* __ASM_MALTA_SPACES_H */
