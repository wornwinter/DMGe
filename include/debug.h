#ifndef DEBUG_H
#define DEBUG_H

#include <iostream>
#include <string>
#include <cstdarg>
#include <cstdint>
#include <stdio.h>

#define DBG_CPU 0
#define DBG_VID 1
#define DBG_APP 2
#define DBG_MMU 3

#define VERBOSE_0 0 // Important notifications (invalid opcodes etc).
#define VERBOSE_1 1
#define VERBOSE_2 2 // Very detailed output. Shows individual opcodes as they are executed.

#define VERBOSE_LEVEL 1

void DbgOut(int src, int verbosity, const char *fmt, ...);

#endif // DEBUG_H
