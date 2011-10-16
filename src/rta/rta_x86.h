#ifndef rta_x86_HEADER
#define rta_x86_HEADER

#include <udis86.h>

#include "exp.h"

enum {
    RTA_X86_VAR_RAX,
    RTA_X86_VAR_RBX,
    RTA_X86_VAR_RCX,
    RTA_X86_VAR_RDX,
    RTA_X86_VAR_RSI,
    RTA_X86_VAR_RDI,
    RTA_X86_VAR_RBP,
    RTA_X86_VAR_RSP,
    RTA_X86_VAR_R8,
    RTA_X86_VAR_R9,
    RTA_X86_VAR_R10,
    RTA_X86_VAR_R11,
    RTA_X86_VAR_R12,
    RTA_X86_VAR_R13,
    RTA_X86_VAR_R14,
    RTA_X86_VAR_R15,
    RTA_X86_VAR_RIP,
    RTA_X86_VAR_LAST
};

struct _rta_ops * rta_x86_ops (unsigned char * data, int data_size, int mode,
                               uint64_t address);

int rta_x86_udis_reg_var  (int udis_reg);
int rta_x86_udis_reg_size (int udis_reg);

#endif
