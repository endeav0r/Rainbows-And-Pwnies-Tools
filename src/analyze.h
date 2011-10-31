#ifndef analyze_HEADER
#define analyze_HEADER

#include <stdio.h>
#include <stdlib.h>
#include <udis86.h>

#include "lib/aatree.h"
#include "types.h"
#include "strings.h"

struct _analyze_function {
    uint_t address;
    uint_t size;
};

_aatree * analyze_find_functions (unsigned char * data, int data_size,
                                  int mode, uint_t * address);

#endif
