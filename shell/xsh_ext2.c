#include <thread.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

shellcmd xsh_ext2(int nargs, char *args[]) {
    ext2();
    return OK;
}
