#include <thread.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

shellcmd xsh_testlfl(int nargs, char *args[]) {
    testlfl();
    return OK;
}
