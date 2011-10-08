#include "aux.h"


struct _sym_list * sym_list_insert (struct _sym_list * sym_list,
                                    struct _elf_symbol * symbol)
{
    struct _sym_list * next;
    struct _sym_list * new;
    
    new = (struct _sym_list *) malloc(sizeof(struct _sym_list));
    new->section_i = symbol->section.index;
    new->symbol_i = symbol->index;
    uint_t_set(&(new->value), elf_symbol_value(symbol));
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


int aux_func_sym_at_address (struct _elf * elf, struct _elf_symbol * symbol,
                             uint_t * address)
{
    struct _elf_section section;
    uint_t top_addr;
    int section_i;
    int symbol_i;
    
    for (section_i = 0; section_i < int_t_get(elf_shnum(elf)); section_i++) {
        elf_section(elf, &section, section_i);
        if (int_t_get(elf_section_type(&section)) == SHT_SYMTAB) {
            for (symbol_i = 0; symbol_i < elf_section_num(&section); symbol_i++) {
                elf_section_symbol(&section, symbol, symbol_i);
                uint_t_set(&top_addr, elf_symbol_value(symbol));
                uint_t_add_int(&top_addr, int_t_get(elf_symbol_size(symbol)));
                if (    (uint_t_cmp(elf_symbol_value(symbol), address) <= 0)
                     && (uint_t_cmp(&top_addr, address) >= 0))
                    return 1;
            }
        }
    }
    
    return 0;
}
            


struct _sym_list * aux_func_syms (struct _elf * elf)
{
    struct _elf_section section;
    struct _elf_symbol symbol;
    struct _sym_list * sym_list = NULL;
    int section_i;
    int symbol_i;
    
    for (section_i = 0; section_i < int_t_get(elf_shnum(elf)); section_i++) {
        elf_section(elf, &section, section_i);
        if (int_t_get(elf_section_type(&section)) == SHT_SYMTAB) {
            for (symbol_i = 0; symbol_i < elf_section_num(&section); symbol_i++) {
                elf_section_symbol(&section, &symbol, symbol_i);
                if (    (elf_symbol_type(&symbol) == STT_FUNC)
                     && (uint_t_get(elf_symbol_shndx(&symbol)) != 0))
                    sym_list = sym_list_insert(sym_list, &symbol);
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
                    
            
