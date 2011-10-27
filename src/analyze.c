#include "analyze.h"

int analyze_function_cmp (void * a, void * b)
{
    struct _analyze_function * af;
    struct _analyze_function * bf;

    af = (struct _analyze_function *) a;
    bf = (struct _analyze_function *) b;

    return uint_t_cmp(&(af->address), &(bf->address));
}


_aatree * analyze_find_functions (unsigned char * data,int data_size,
                                  int mode, uint_t * address)
{
    int_t offset;
    int bytes_disassembled = 0;
    int_t tmp_address_signed;
    int_t base_address_signed;
    ud_t ud_obj;
    struct _analyze_function analyze_function;
    _aatree * tree;

    tree = aatree_create(analyze_function_cmp, sizeof(analyze_function));
    
    // find the start of all functions
    ud_init(&ud_obj);
    ud_set_mode(&ud_obj, mode);
    
    ud_set_input_buffer(&ud_obj, data, data_size);
    
    int_t_uint_t(&base_address_signed, address);
    
    while (ud_disassemble(&ud_obj))
    {
        bytes_disassembled += ud_insn_len(&ud_obj);
        if (    (ud_obj.mnemonic == UD_Icall)
             && (ud_obj.operand[0].type == UD_OP_JIMM)) {
            switch (ud_obj.operand[0].size) {
            case 32 :
                int_t_32_set(&offset, ud_obj.operand[0].lval.udword);
                break;
            case 64 :
                int_t_64_set(&offset, ud_obj.operand[0].lval.uqword);
                break;
            }
            int_t_set(&tmp_address_signed, &base_address_signed);
            int_t_add(&tmp_address_signed, &offset);
            int_t_add_int(&tmp_address_signed, bytes_disassembled);
            uint_t_int_t(&(analyze_function.address), &tmp_address_signed);
            aatree_insert(tree, &analyze_function);
        }
    }
    
    return tree;
}

