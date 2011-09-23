#include "aux.h"


struct _sym_list * sym_list_insert (struct _sym_list * sym_list,
                                    struct _elf_sym * sym)
{
    struct _sym_list * next;
    struct _sym_list * new;
    
    new = (struct _sym_list *) malloc(sizeof(struct _sym_list));
    new->shdr_i = sym->shdr.index;
    new->sym_i = sym->index;
    uint_t_set(&(new->value), sym_value(sym));
    new->next = NULL;
    
    if (sym_list == NULL)
        return new;
        
    next = sym_list;
    while (next->next != NULL) {
        if (uint_t_cmp(&(new->value), &(next->next->value)) > 0)
            break;
        next = next->next;
    }
    new->next = next->next;
    next->next = new;
    
    return sym_list;
}


int aux_func_sym_at_address (struct _elf * elf, struct _elf_sym * sym,
                             uint_t * address)
{
    struct _elf_shdr shdr;
    uint_t top_addr;
    int shdr_i;
    int sym_i;
    
    for (shdr_i = 0; shdr_i < int_t_get(elf_shnum(elf)); shdr_i++) {
        elf_shdr(elf, &shdr, shdr_i);
        if (int_t_get(shdr_type(&shdr)) == SHT_SYMTAB) {
            for (sym_i = 0; sym_i < shdr_num(&shdr); sym_i++) {
                shdr_sym(&shdr, sym, sym_i);
                uint_t_set(&top_addr, sym_value(sym));
                uint_t_add_int(&top_addr, int_t_get(sym_size(sym)));
                if (    (uint_t_cmp(sym_value(sym), address) <= 0)
                     && (uint_t_cmp(&top_addr, address) >= 0))
                    return 1;
            }
        }
    }
    
    return 0;
}
            


struct _sym_list * aux_func_syms (struct _elf * elf)
{
    struct _elf_shdr shdr;
    struct _elf_sym sym;
    struct _sym_list * sym_list = NULL;
    int shdr_i;
    int sym_i;
    
    for (shdr_i = 0; shdr_i < int_t_get(elf_shnum(elf)); shdr_i++) {
        elf_shdr(elf, &shdr, shdr_i);
        if (int_t_get(shdr_type(&shdr)) == SHT_SYMTAB) {
            for (sym_i = 0; sym_i < shdr_num(&shdr); sym_i++) {
                shdr_sym(&shdr, &sym, sym_i);
                if (    (sym_type(&sym) == STT_FUNC)
                     && (uint_t_get(sym_shndx(&sym)) != 0))
                    sym_list = sym_list_insert(sym_list, &sym);
            }
        }
    }
    
    return sym_list;
}


void sym_list_destroy (struct _sym_list * sym_list)
{
    struct _sym_list * next;
    while (sym_list != NULL) {
        next = sym_list->next;
        free(sym_list);
        sym_list = next;
    }
}
                    
            
