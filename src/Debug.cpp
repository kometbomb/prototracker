#include "Debug.h"
#include <cstdarg>


void debug(const char * message, ...)
{
    char dest[1024];
    va_list argptr;
    va_start(argptr, message);
    vsnprintf(dest, sizeof(dest), message, argptr);
    va_end(argptr);
    printf("[DEBUG] %s\n", dest);
}
