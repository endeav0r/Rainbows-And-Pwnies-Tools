#ifndef analyze_HEADER
#define analyze_HEADER

#include <stdio.h>
#include <stdlib.h>
#include <udis86.h>

#include "lib/aatree.h"
#include "lib/list.h"
#include "types.h"
#include "strings.h"

struct _analyze_function {
    uint_t address;
    int size;
};

_aatree * analyze_find_functions (unsigned char * data, int data_size,
                                  int mode, uint_t * address, _aatree * old_tree);

_aatree * analyze_find_functions_sizes (_aatree * tree, uint_t * last_address_end);

#endif
