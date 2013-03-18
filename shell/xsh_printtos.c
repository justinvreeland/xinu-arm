#include <thread.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

shellcmd xsh_printtos(int nargs, char *args[])
{
    printtos();
    return OK;
}
