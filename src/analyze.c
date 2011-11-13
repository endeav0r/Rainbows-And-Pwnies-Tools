#include "analyze.h"

int analyze_function_cmp (void * a, void * b)
{
    struct _analyze_function * af;
    struct _analyze_function * bf;

    af = (struct _analyze_function *) a;
    bf = (struct _analyze_function *) b;

    return uint_t_cmp(&(af->address), &(bf->address));
}


// if oldtree is not NULL, will continue adding to the oldtree. This has the
// advantage of improving the finding the correct sizes of functions called from
// previous calls of analyze_find_functions
_aatree * analyze_find_functions (unsigned char * data, int data_size,
                                  int mode, uint_t * address, _aatree * old_tree)
{
    int_t offset;
    int bytes_disassembled = 0;
    int_t tmp_address_signed;
    int_t base_address_signed;
    uint_t last_address_end;
    ud_t ud_obj;
    struct _analyze_function analyze_function;
    _aatree * tree;

    if (old_tree == NULL)
        tree = aatree_create(analyze_function_cmp, sizeof(analyze_function));
    else
        tree = old_tree;
    
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
            analyze_function.size =  -1;
            aatree_insert(tree, &analyze_function);
        }
    }

    uint_t_set(&last_address_end, address);
    uint_t_add_int(&last_address_end, data_size);
    tree = analyze_find_functions_sizes(tree, &last_address_end);

    return tree;
}


// last address is the address of the right past the end of the last function
// leave it NULL if you don't want to leave size of last function as it is
_aatree * analyze_find_functions_sizes (_aatree * tree, uint_t * last_address_end)
{
    struct _analyze_function * last;
    struct _analyze_function * next;
    _list * list;
    _list_iterator it;
    uint_t tmp;

    // we assume a function ends where the next function begins
    // this is much simpler if we convert our tree to a list first and then reinsert into the tree
    list = list_copy_aatree(tree);
    aatree_destroy(tree);
    tree = aatree_create(analyze_function_cmp, sizeof(struct _analyze_function));
    
    list_iterator(list, &it);
    last = (struct _analyze_function *) list_next(&it);
    if (last != NULL) {
        while ((next = (struct _analyze_function *) list_next(&it)) != NULL) {
            uint_t_set(&tmp, &(next->address));
            uint_t_sub(&tmp, &(last->address));
            last->size = uint_t_get(&tmp);
            aatree_insert(tree, last);
            last = next;
        }
        if (last_address_end != NULL) {
            if (last->size < 0 ) {
                uint_t_set(&tmp, last_address_end);
                uint_t_sub(&tmp, &(last->address));
                last->size = uint_t_get(&tmp);
            }
        }
        aatree_insert(tree, last);
    }

    list_destroy(list);

    return tree;
}
