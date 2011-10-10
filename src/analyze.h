#ifndef analyze_HEADER
#define analyze_HEADER

#include <stdio.h>
#include <stdlib.h>
#include <udis86.h>

#include "types.h"
#include "strings.h"

struct _analyze_function {
    int level;
    uint_t address;
    struct _analyze_function * prev;
    struct _analyze_function * next;
};

struct _analyze_function * analyze_find_functions (unsigned char * data,
                                                   int data_size, int mode,
                                                   uint_t * address,
                                                   struct _analyze_function * functions);
void analyze_function_destroy (struct _analyze_function * analyze_function);


struct _analyze_function * analyze_function_find (struct _analyze_function * function,
                                                  uint_t * value);
struct _analyze_function * analyze_function_skew (struct _analyze_function * function);
struct _analyze_function * analyze_function_split (struct _analyze_function * function);
struct _analyze_function * analyze_function_insert (struct _analyze_function * functions,
                                                    uint_t * address);
struct _analyze_function * analyze_function_create (uint_t * address);
struct _analyze_function * analyze_function_listify (struct _analyze_function * functions);


#endif
