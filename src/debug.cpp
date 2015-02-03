#include "debug.h"

void DbgOut(int src, int verbosity, const char *fmt, ...)
{
#ifdef _DEBUG
    if(verbosity <= VERBOSE_LEVEL)
    {
        printf("DEBUG: ");
        va_list arg;
        int done;

        va_start(arg, fmt);
        switch(src){
            case DBG_CPU:
                printf("CPU - ");
            break;

            case DBG_VID:
                printf("VID - ");
            break;

            case DBG_APP:
                printf("APP - ");
            break;

            case DBG_MMU:
                printf("MMU - ");
            break;

            default:
                printf("UNKNOWN: ");
            break;
        }
        vfprintf(stdout, fmt, arg);
        va_end(arg);
        printf("\n");
    }
#endif
}
