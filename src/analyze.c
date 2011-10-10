#include "analyze.h"


/*
* the following functions treat struct _analyze_function as a tree, where
* prev is left, and next is right
*/
struct _analyze_function * analyze_function_find (struct _analyze_function * function,
                                                  uint_t * value)
{
    int cmp;
    if (function == NULL)
        return NULL;
    
    cmp = uint_t_cmp(&(function->address), value);
    if (cmp > 0)
        return analyze_function_find(function->prev, value);
    else if (cmp < 0)
        return analyze_function_find(function->next, value);
    else
        return function;
}


struct _analyze_function * analyze_function_skew (struct _analyze_function * function)
{
    struct _analyze_function * L;
    
    if (function == NULL)
        return NULL;
    if (function->prev == NULL)
        return function;
    
    if (function->prev->level == function->level) {
        L = function->prev;
        function->prev = L->next;
        L->next = function;
        return L;
    }
    return function;
}

struct _analyze_function * analyze_function_split (struct _analyze_function * function)
{
    struct _analyze_function * R;
    
    if (function == NULL)
        return NULL;
    if (function->next == NULL)
        return function;
    if (function->next->next == NULL)
        return function;
    
    if (function->level == function->next->next->level) {
        R = function->next;
        function->next = R->prev;
        R->prev = function;
        R->level++;
        return R;
    }
    return function;
}

struct _analyze_function * analyze_function_insert (struct _analyze_function * functions,
                                                    uint_t * address)
{
    int cmp;
    
    if (functions == NULL)
        return analyze_function_create(address);
    
    cmp = uint_t_cmp(&(functions->address), address);
    
    if (cmp > 0)
        functions->prev = analyze_function_insert(functions->prev, address);
    else if (cmp < 0)
        functions->next = analyze_function_insert(functions->next, address);
    else {
        fprintf(stderr,
                "tried to insert already existing address %s in _analyze_function\n",
                uint_t_str0x(address));
        return NULL;
    }
    
    functions = analyze_function_skew(functions);
    functions = analyze_function_split(functions);
    
    return functions;
}

struct _analyze_function * analyze_function_create (uint_t * address)
{
    struct _analyze_function * function;
    
    function = (struct _analyze_function *) malloc(sizeof(struct _analyze_function));
    function->level = 0;
    uint_t_set(&(function->address), address);
    function->next = NULL;
    function->prev = NULL;
    
    return function;
}

// turns a tree of struct _analyze_function into a list
struct _analyze_function * analyze_function_listify (struct _analyze_function * functions)
{
    struct _analyze_function * first;
    struct _analyze_function * next;
        
    if (functions == NULL)
        return NULL;
    
    if (functions->prev == NULL)
        first = functions;
    else {
        first = analyze_function_listify(functions->prev);
        next = first;
        while (next->next != NULL)
            next = next->next;
        functions->prev = next;
        next->next = functions;
    }
    
    if (functions->next != NULL) {
        functions->next = analyze_function_listify(functions->next);
        functions->next->prev = functions;
    }
    
    return first;
}
    

struct _analyze_function * analyze_find_functions (unsigned char * data,
                                                   int data_size, int mode,
                                                   uint_t * address,
                                                   struct _analyze_function * functions)
{
    int_t offset;
    int bytes_disassembled = 0;
    int_t tmp_address_signed;
    int_t base_address_signed;
    uint_t tmp_address;
    ud_t ud_obj;
    
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
            uint_t_int_t(&tmp_address, &tmp_address_signed);
            if (analyze_function_find(functions, &tmp_address) == NULL)
                functions = analyze_function_insert(functions, &tmp_address);
        }
    }
    
    return functions;
}


void analyze_function_destroy (struct _analyze_function * function)
{
    struct _analyze_function * next;
    while (function != NULL) {
        next = function->next;
        free(function);
        function = next;
    }
}

