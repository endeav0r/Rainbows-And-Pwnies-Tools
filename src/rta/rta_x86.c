#include "rta_x86.h"

struct _rta_exp * rta_x86_operand_exp (ud_t * ud_obj, int operand)
{
    struct _rta_exp * index = NULL;
    struct _rta_exp * base = NULL;

    switch (ud_obj->operand[operand].type) {

    case UD_NONE :
        return NULL;

    case UD_OP_MEM :
        if (ud_obj->operand[operand].index != UD_NONE)
            index = rta_exp_create(RTA_EXP_VAR,
                                   rta_x86_udis_reg_var(ud_obj->operand[operand].index),
                                   rta_x86_udis_reg_size(ud_obj->operand[operand].index));
        if (ud_obj->operand[operand].scale > 0)
            index  = rta_exp_create_arith(RTA_EXP_MUL, ud_obj->operand[operand].size,
                                          index,
                                          rta_exp_create(RTA_EXP_SCONST,
                                                         ud_obj->operand[operand].scale,
                                                         ud_obj->operand[operand].size));
        if (ud_obj->operand[operand].base != UD_NONE)
            base = rta_exp_create(RTA_EXP_VAR,
                                  rta_x86_udis_reg_var(ud_obj->operand[operand].base),
                                  rta_x86_udis_reg_size(ud_obj->operand[operand].base));
        if (ud_obj->operand[operand].offset > 0)
            base = rta_exp_create_arith(RTA_EXP_ADD, ud_obj->operand[operand].offset,
                                        base,
                                        rta_exp_create(RTA_EXP_SCONST,
                                                       ud_obj->operand[operand].lval.uqword,
                                                       ud_obj->operand[operand].offset));
        if (index == NULL)
            return base;
        else if (base == NULL) // just so we're clear, this should NEVER happen
            return index;
        else
            return rta_exp_create_arith(RTA_EXP_ADD, ud_obj->operand[operand].size, base, index);

    default :
        return NULL;
    }
}


struct _rta_ops * rta_x86_ops (unsigned char * data, int data_size, int mode,
                               uint64_t address)
{
    struct _rta_ops * ops;
    ud_t ud_obj;

    ud_init(&ud_obj);

    ud_set_mode(&ud_obj, mode);
    ud_set_input_buffer(&ud_obj, data, data_size);
    ud_set_syntax(&ud_obj, NULL);

    while (ud_disassemble(&ud_obj)) {
        switch (ud_obj.mnemonic) {
        case UD_Ipush :
            break;
        default :
            break;
        }
    }
    return NULL;
}


int rta_x86_udis_reg_size (int udis_reg)
{
    switch (udis_reg) {
    case UD_R_AL :
    case UD_R_AH :
    case UD_R_BL :
    case UD_R_BH :
    case UD_R_CL :
    case UD_R_CH :
    case UD_R_DL :
    case UD_R_DH :
        return 8;
    case UD_R_AX :
    case UD_R_BX :
    case UD_R_CX :
    case UD_R_DX :
        return 16;
    case UD_R_EAX :
    case UD_R_EBX :
    case UD_R_ECX :
    case UD_R_EDX :
    case UD_R_ESP :
    case UD_R_EBP :
    //case UD_R_EIP : no EIP in udis86. thanks udis86.
        return 32;
    case UD_R_RAX :
    case UD_R_RBX :
    case UD_R_RCX :
    case UD_R_RDX :
    case UD_R_RSP :
    case UD_R_RBP :
    case UD_R_RIP :
        return 64;
    }
    return -1;
}


int rta_x86_udis_reg_var (int udis_reg)
{
    switch (udis_reg) {
    case UD_R_AL  :
    case UD_R_AH  :
    case UD_R_AX  :
    case UD_R_EAX :
    case UD_R_RAX :
        return RTA_X86_VAR_RAX;
    case UD_R_BL  :
    case UD_R_BH  :
    case UD_R_BX  :
    case UD_R_EBX :
    case UD_R_RBX :
        return RTA_X86_VAR_RBX;
    case UD_R_CL  :
    case UD_R_CH  :
    case UD_R_CX  :
    case UD_R_ECX :
    case UD_R_RCX :
        return RTA_X86_VAR_RCX;
    case UD_R_DL  :
    case UD_R_DH  :
    case UD_R_DX  :
    case UD_R_EDX :
    case UD_R_RDX :
        return RTA_X86_VAR_RDX;
    case UD_R_ESP :
    case UD_R_RSP :
        return RTA_X86_VAR_RSP;
    case UD_R_EBP :
    case UD_R_RBP :
        return RTA_X86_VAR_RBP;
    //case UD_R_EIP :
    case UD_R_RIP :
        return RTA_X86_VAR_RIP;
    }
    return -1;
}

