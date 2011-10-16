#include "ops.h"


struct _rta_ops * rta_ops_create ()
{
    struct _rta_ops * ops;

    ops = (struct _rta_ops *) malloc(sizeof(struct _rta_ops));
    ops->ops = NULL;
    ops->last = NULL;
    ops->num_ops = 0;

    return ops;
}


void rta_ops_destroy (struct _rta_ops * ops)
{
    struct _rta_op * next;
    while (ops->ops != NULL) {
        next = ops->ops->next;
        if (ops->ops->exp_dst != NULL)
            rta_exp_destroy(ops->ops->exp_dst);
        if (ops->ops->exp_src != NULL)
            rta_exp_destroy(ops->ops->exp_src);
        free(ops->ops);
        ops->ops = next;
    }
    free(ops);
}


void rta_ops_append (struct _rta_ops * ops, struct _rta_op * op)
{
    if (ops->ops == NULL) {
        ops->ops = op;
        ops->last = op;
    }
    else {
        ops->last->next = op;
        ops->last = op;
    }
    ops->num_ops++;
}


struct _rta_op * rta_op_create (uint64_t source_address, int type,
                                struct _rta_exp * exp_dst,
                                struct _rta_exp * exp_src)
{
    struct _rta_op * op;

    op = (struct _rta_op *) malloc(sizeof(struct _rta_op));

    op->source_address = source_address;
    op->type = type;
    op->exp_dst = exp_dst;
    op->exp_src = exp_src;
    op->next = NULL;

    return op;
}
