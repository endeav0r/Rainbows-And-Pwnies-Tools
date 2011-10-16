#include "vars.h"

struct _vars * var_create ()
{
    struct _vars * vars;

    vars = (struct _vars *) malloc(sizeof(struct _vars));

    vars->vars = NULL;
    vars->vars_size = NULL;
    vars->size = 0;

    return vars;
}


void vars_destroy (struct _vars * vars)
{
    if (vars->vars != NULL)
        free(vars->vars);
    if (vars->vars_size != NULL)
        free(vars->vars_size);
    free (vars);
}


uint64_t vars_get (struct _vars * vars, int index)
{
    if (index > vars->size)
        return -1;
    return vars->vars[index];
}


int vars_get_size (struct _vars * vars, int index)
{
    if (index > vars->size)
        return -1;
    return vars->vars_size[index];
}


int vars_set (struct _vars * vars, int index, uint64_t value, int size)
{
    uint64_t * tmp;

    if (index > vars->size) {
        // we need to add at least 1, but by adding 8 we cache some space
        tmp = realloc(vars->vars, sizeof(uint64_t) * (index + 8));
        if (tmp == NULL)
            return -1;
        vars->vars = tmp;
        vars->size = index + 7;
    }
    
    vars->vars[index] = value;
    vars->vars_size[index] = size;

    return 0;
}
