#ifndef rta_op_HEADER
#define rta_op_HEADER

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
};

#endif
