/*
 *
 */

#ifndef LFS_H_
#define LFS_H_

#include <device.h>
#include <stdarg.h>
#include <stddef.h>

#define MAXPATHL 1500
#define MAXFILEL 255

devcall lfsInit( struct dentry *);
devcall lfsOpen( struct dentry *, char *, char *);
devcall rfsControl(void *); // for remote fs unused

#endif
