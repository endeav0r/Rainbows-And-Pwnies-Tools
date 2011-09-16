#include "rop.h"

#define MAX_BACKTRACK_LEN 32

struct _rop_list * rop_ret_rops (unsigned char * data, int data_size, int depth)
{

    int d; // iterator for location in data
    int backtrack;
    int backtrack_depth;
    
    struct _rop_list * rop_list = NULL;
    struct _rop_list * next = NULL;
    ud_t ud_obj;

    ud_init(&ud_obj);
    
    ud_set_mode(&ud_obj, 32);
    ud_set_input_buffer(&ud_obj, data, data_size);
    ud_set_syntax(&ud_obj, NULL);
    
    for (d = 0; d < data_size; d++) {
        // find a ret
        // ret = 0xc3
        if (data[d] == 0xc3) {
            // we don't count the d byte in our rop chain depth
            backtrack = d - 1;
            while ((backtrack_depth = rop_depth(&(data[backtrack]), d - backtrack)) < depth) {
                backtrack--;
                if (    (backtrack_depth > depth) 
                     || (backtrack < 0)
                     || (d - backtrack >= MAX_BACKTRACK_LEN)) {
                    backtrack = -1;
                    break;
                }
            }
            // if backtrack underflows data, this wasn't a valid chain
            if (backtrack < 0)
                continue;
            // create this node in list
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
                                       d - backtrack + 1,
                                       d - (d - backtrack));
            next->next = NULL;
        }
    }
    
    return rop_list;
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


int rop_depth (unsigned char * data, int data_size)
{
    int depth = 0;
    ud_t ud_obj;

    ud_init(&ud_obj);
    
    ud_set_mode(&ud_obj, 32);
    ud_set_input_buffer(&ud_obj, data, data_size);
    ud_set_syntax(&ud_obj, NULL);
    
    while (ud_disassemble(&ud_obj)) {
        if (ud_obj.mnemonic == UD_Iinvalid)
            return 0;
        depth++;
    }
    return depth;    
}


struct _rop_ins * rop_ins_create (unsigned char * data, int data_size, int offset)
{
    struct _rop_ins * rop_ins = NULL;
    struct _rop_ins * next = NULL;
    ud_t ud_obj;

    ud_init(&ud_obj);
    
    ud_set_mode(&ud_obj, 32);
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
    rop_ins_destroy(ins->next);
    free(ins);
}



