#ifndef rta_ops_HEADER
#define rta_ops_HEADER

#include <stdlib.h>

#include "exp.h"
#include "mem.h"

enum {
    RTA_OP_UNKNOWN,
    RTA_OP_ASSIGN,
    RTA_OP_JMP,
    RTA_OP_STORE,
    RTA_OP_LOAD
};

struct _rta_op {
    uint64_t source_address;
    int type;
    struct _rta_exp * exp_dst;
    struct _rta_exp * exp_src;
    struct _rta_op * next;
};

struct _rta_ops {
    struct _rta_op * ops;
    struct _rta_op * last;
    int num_ops;
};

struct _rta_ops * rta_ops_create  ();
void              rta_ops_destroy (struct _rta_ops * ops);
void              rta_ops_append  (struct _rta_ops * ops, struct _rta_op * op);

struct _rta_op * rta_op_create (uint64_t source_address, int type,
                                struct _rta_exp * exp_dst,
                                struct _rta_exp * exp_src);



#endif
