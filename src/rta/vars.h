#ifndef rta_var_HEADER
#define rta_var_HEADER

#include <stdlib.h>
#include <stdint.h>

struct _vars {
    uint64_t * vars;
    int * vars_size;
    int size;
};

struct _vars * vars_create   ();
void           vars_destroy  (struct _vars * vars);
uint64_t       vars_get      (struct _vars * vars, int index);
int            vars_get_size (struct _vars * vars, int index);
int            vars_set      (struct _vars * vars, int index, uint64_t value, int size);

#endif
