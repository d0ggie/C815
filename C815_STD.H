#ifndef __C815_STD_H
#define __C815_STD_H

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <assert.h>

#define asizof(_x) (sizeof(_x)/sizeof((_x)[0]))
#define bsizof(_x) (sizeof(_x)<<3)

#endif//__C815_STD_H
