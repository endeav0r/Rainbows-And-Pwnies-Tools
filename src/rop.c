#include "rop.h"

#define MAX_BACKTRACK_LEN 32

int rop_detect_ret (unsigned char * data, int data_size, int mode)
{
    ud_t ud_obj;

    ud_init(&ud_obj);
    
    ud_set_mode(&ud_obj, mode);
    ud_set_input_buffer(&ud_obj, data, data_size);
    ud_set_syntax(&ud_obj, NULL);
    
    ud_disassemble(&ud_obj);
    if (ud_obj.mnemonic == UD_Iret) {
        return ud_insn_len(&ud_obj);
    }
    return 0;
}


int rop_detect_jmp_reg (unsigned char * data, int data_size, int mode)
{
    ud_t ud_obj;

    ud_init(&ud_obj);
    
    ud_set_mode(&ud_obj, mode);
    ud_set_input_buffer(&ud_obj, data, data_size);
    ud_set_syntax(&ud_obj, NULL);
    
    ud_disassemble(&ud_obj);
    if (ud_obj.mnemonic == UD_Ijmp) {
        if (ud_obj.operand[0].type == UD_OP_REG)
            return ud_insn_len(&ud_obj);
    }
    return 0;
}


int rop_detect_cond_jmp_reg (unsigned char * data, int data_size, int mode)
{
    ud_t ud_obj;

    ud_init(&ud_obj);
    
    ud_set_mode(&ud_obj, mode);
    ud_set_input_buffer(&ud_obj, data, data_size);
    ud_set_syntax(&ud_obj, NULL);
    
    ud_disassemble(&ud_obj);
    if (    (ud_obj.mnemonic == UD_Ijo)
         || (ud_obj.mnemonic == UD_Ijno)
         || (ud_obj.mnemonic == UD_Ijb)
         || (ud_obj.mnemonic == UD_Ijae)
         || (ud_obj.mnemonic == UD_Ijz)
         || (ud_obj.mnemonic == UD_Ijnz)
         || (ud_obj.mnemonic == UD_Ijbe)
         || (ud_obj.mnemonic == UD_Ija)
         || (ud_obj.mnemonic == UD_Ijns)
         || (ud_obj.mnemonic == UD_Ijp)
         || (ud_obj.mnemonic == UD_Ijnp)
         || (ud_obj.mnemonic == UD_Ijl)
         || (ud_obj.mnemonic == UD_Ijge)
         || (ud_obj.mnemonic == UD_Ijle)
         || (ud_obj.mnemonic == UD_Ijg)
         || (ud_obj.mnemonic == UD_Ijcxz)
         || (ud_obj.mnemonic == UD_Ijecxz)
         || (ud_obj.mnemonic == UD_Ijecxz)) {
        if (    (ud_obj.operand[0].type == UD_OP_REG)
             || (ud_obj.operand[0].type == UD_OP_MEM))
            return ud_insn_len(&ud_obj);
    }
    return 0;
}


int rop_detect_call_reg (unsigned char * data, int data_size, int mode)
{
    ud_t ud_obj;

    ud_init(&ud_obj);
    
    ud_set_mode(&ud_obj, mode);
    ud_set_input_buffer(&ud_obj, data, data_size);
    ud_set_syntax(&ud_obj, NULL);
    
    ud_disassemble(&ud_obj);
    if (ud_obj.mnemonic == UD_Icall) {
        if (ud_obj.operand[0].type == UD_OP_REG)
            return ud_insn_len(&ud_obj);
    }
    return 0;
}


struct _rop_list * rop_find_rops (unsigned char * data,
                                  int data_size,
                                  int depth,
                                  int (* detect_callback) (unsigned char * data,
                                                           int data_size,
                                                           int mode),
                                  int mode)
{
    int d; // iterator for location in data
    int ins_size;
    int backtrack;
    
    struct _rop_list * rop_list = NULL;
    struct _rop_list * next = NULL;
    ud_t ud_obj;

    ud_init(&ud_obj);
    
    ud_set_mode(&ud_obj, mode);
    ud_set_input_buffer(&ud_obj, data, data_size);
    ud_set_syntax(&ud_obj, NULL);
    
    for (d = 0; d < data_size; d++) {
        if ((ins_size = detect_callback(&(data[d]), data_size - d, mode)) > 0) {
            // we don't count the d byte in our rop chain depth
            backtrack = d - 1;
            while (d - backtrack <= MAX_BACKTRACK_LEN) {
                if (rop_depth(&(data[backtrack]), d - backtrack, mode) == depth) {
                    if (rop_list == NULL) {
                        rop_list = (struct _rop_list *) malloc(sizeof(struct _rop_list));
                        next = rop_list;
                    }
                    else {
                        next->next = (struct _rop_list *) malloc(sizeof(struct _rop_list));
                        next = next->next;
                    }
                    next->offset = d - (d - backtrack);
                    next->ins = rop_ins_create(&(data[backtrack]),
                                               d - backtrack + ins_size,
                                               d - (d - backtrack),
                                               mode);
                    next->next = NULL;
                }
                else if (backtrack < 0)
                    break;
                backtrack--;
            }
        }
    }
    
    return rop_list;
}


struct _rop_list * rop_ret_rops (unsigned char * data, int data_size,
                                 int depth, int mode)
{
    return rop_find_rops(data, data_size, depth, rop_detect_ret, mode);
}


struct _rop_list * rop_jmp_reg_rops (unsigned char * data, int data_size,
                                     int depth, int mode)
{
    return rop_find_rops(data, data_size, depth, rop_detect_jmp_reg, mode);
}


struct _rop_list * rop_cond_jmp_reg_rops (unsigned char * data, int data_size,
                                          int depth, int mode)
{
    return rop_find_rops(data, data_size, depth, rop_detect_cond_jmp_reg, mode);
}


struct _rop_list * rop_call_reg_rops (unsigned char * data, int data_size,
                                     int depth, int mode)
{
    return rop_find_rops(data, data_size, depth, rop_detect_call_reg, mode);
}


void rop_list_destroy (struct _rop_list * rop_list)
{
    if (rop_list == NULL)
        return;
    if (rop_list->ins != NULL)
        rop_ins_destroy(rop_list->ins);
    rop_list_destroy(rop_list->next);
    free(rop_list);
}


struct _rop_ins * rop_list_ins (struct _rop_list * rop_list)
{
    return rop_list->ins;
}


int rop_depth (unsigned char * data, int data_size, int mode)
{
    int depth = 0;
    ud_t ud_obj;

    ud_init(&ud_obj);
    
    ud_set_mode(&ud_obj, mode);
    ud_set_input_buffer(&ud_obj, data, data_size);
    ud_set_syntax(&ud_obj, NULL);
    
    while (ud_disassemble(&ud_obj)) {
        if (ud_obj.mnemonic == UD_Iinvalid)
            return 0;
        depth++;
    }
    return depth;    
}


struct _rop_ins * rop_ins_create (unsigned char * data, int data_size,
                                  int offset, int mode)
{
    struct _rop_ins * rop_ins = NULL;
    struct _rop_ins * next = NULL;
    ud_t ud_obj;

    ud_init(&ud_obj);
    
    ud_set_mode(&ud_obj, mode);
    ud_set_input_buffer(&ud_obj, data, data_size);
    ud_set_syntax(&ud_obj, UD_SYN_INTEL);
    
    while (ud_disassemble(&ud_obj)) {
        if (rop_ins == NULL) {
            rop_ins = (struct _rop_ins *) malloc(sizeof(struct _rop_ins));
            next = rop_ins;
        }
        else {
            next->next = (struct _rop_ins *) malloc(sizeof(struct _rop_ins));
            next = next->next;
        }
        next->mnemonic = ud_obj.mnemonic;
        next->bytes_size = ud_insn_len(&ud_obj);
        next->bytes = (unsigned char *) malloc(next->bytes_size);
        memcpy(next->bytes, ud_insn_ptr(&ud_obj), next->bytes_size);        
        next->description = (char *) malloc(strlen(ud_insn_asm(&ud_obj)) + 1);
        strcpy(next->description, ud_insn_asm(&ud_obj));
        next->offset = offset + (int) ud_insn_off(&ud_obj);
        next->next = NULL;
    }
    return rop_ins;    
}


void rop_ins_destroy (struct _rop_ins * ins) {
    if (ins == NULL)
        return;
    if (ins->description != NULL)
        free(ins->description);
    if (ins->bytes != NULL)
        free(ins->bytes);
    rop_ins_destroy(ins->next);
    free(ins);
}

