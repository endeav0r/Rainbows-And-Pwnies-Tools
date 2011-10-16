#ifndef rta_exp_HEADER
#define rta_exp_HEADER

#include <stdint.h>
#include <stdlib.h>

enum {
    RTA_EXP_VAR,
    RTA_EXP_SCONST,
    RTA_EXP_UCONST,
    RTA_EXP_ADD,
    RTA_EXP_SUB,
    RTA_EXP_MUL,
    RTA_EXP_DIV,
    RTA_EXP_MOD,
    RTA_EXP_ROTR,
    RTA_EXP_ROTL,
    RTA_EXP_AND,
    RTA_EXP_OR,
    RTA_EXP_XOR,
    RTA_EXP_EQ,
    RTA_EXP_NE,
    RTA_EXP_LT,
    RTA_EXP_LTE,
};

struct _rta_exp {
    int type;
    int bits; // size of the value in bits
    uint64_t value;
    struct _rta_exp * left;
    struct _rta_exp * right;
};

struct _rta_exp * rta_exp_create_arith (int type, int bits,
                                        struct _rta_exp * left,
                                        struct _rta_exp * right);
struct _rta_exp * rta_exp_create (int type, uint64_t value, int bits);

uint64_t rta_exp_eval (struct _rta_exp * exp);

#endif
