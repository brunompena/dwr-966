/* adpVxRamFs.h - ram-based filesystem for vxworks */

/* Copyright (c) 2007, TeamF1, Inc. */

/*
modification history
--------------------
01b,10oct07,rnp  changed BLKS_PER_TRACK 30->40, to fix crash on mips vx5.4
01a,27jul07,rnp  written
*/

/* includes */
#include <adaptos.h>

#ifndef HAVE_ADP_VX_RAMFS_OPT
/* defines to create a 30KB(=512*30*2) RAM Disk */
/* size of each block(sector) in the file system */
#define ADP_VX_RAMFS_BLK_SIZE         (512)
/* number of blocks per track in the file system */
#define ADP_VX_RAMFS_BLKS_PER_TRACK   (40)
/* number of tracks in the file system */
#define ADP_VX_RAMFS_TRACK_COUNT      (2)

typedef struct adpVxRamFsOpt
    {
    int  blkSize;
    int  blksPerTrk;
    int  trkCount;
    } ADP_VX_RAMFS_OPT;

IMPORT ADP_VX_RAMFS_OPT adpVxRamFsOpt;
#define HAVE_ADP_VX_RAMFS_OPT
#endif /* HAVE_ADP_VX_RAMFS_OPT */
