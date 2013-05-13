/*
 *
 *
 */

#ifndef LFL_H_
#define LFL_H_

#include <device.h>
#include <stdarg.h>
#include <stddef.h>

/* maybe some flags for file access restrictions */


devcall lflInit(device *);
devcall lflRead(device *, void *, uint);
devcall lflClose(device *);
devcall lflGetc(device *);
devcall lflWrite(device *, void *, uint);
devcall lflPutc(device *, char);
devcall lflSeek( void *);


#endif
