#ifndef rta_var_HEADER
#define rta_var_HEADER

// when we realloc vars, we do so by this amount
#define VARS_REALLOC_SIZE 32

struct _vars {
    uint64_t * vars;
    int * vars_size;
    int size;
};

struct _vars * vars_create   ();
uint64_t       vars_get      (int index);
int            vars_get_size (int index);
int            vars_set      (int index, uint64_t value, int size);

#endif
