/* adpUnistd.h - place holder for unistd.h for threadX */

/* Copyright (c) 2006, TeamF1, Inc. */

/*
modification history
--------------------
01b,17may06,pkk  written
*/

#ifndef __INCadpUnistdH
#define __INCadpUnistdH

/* have the needed prototypes for IO */
extern int close(int);

extern int  read(int, void *, size_t);

extern int  write(int, void *, size_t);

extern off_t lseek(int, off_t, int);

extern int unlink (const char * );

extern int chdir(const char *);

extern int rmdir(const char *);

extern int truncate(const char *, off_t);

extern int ftruncate(int, off_t);

extern int chown(const char *, uid_t, gid_t);

extern int isatty(int);

#endif /* __INCadpUnistdH */
