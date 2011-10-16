#ifndef rta_op_HEADER
#define rta_op_HEADER

#include "mem.h"

enum {
    RTA_OP_UNKNOWN,
    RTA_OP_ASSIGN,
    RTA_OP_JMP,
    RTA_OP_STORE,
    RTA_OP_FETCH
};

struct _rta_op {
    uint64_t source_address;
    int type;
    struct _rta_exp exp_a;
    struct _rta_exp exp_b;
    struct _rta_op * next;
};

struct _rta_ops {
    struct _rta_op * ops;
    struct _rta_op * last;
    int num_ops;
};

int rta_op_execute (struct _rta_op * op, struct _rta_mem * mem);
struct _rta_

#endif
