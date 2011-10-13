#ifndef aux_HEADER
#define aux_HEADER

#include <stdlib.h>

#include "bin/elf.h"
#include "types.h"

struct _sym_list {
    int section_i;
    int symbol_i;
    uint_t value;
    struct _sym_list * next;
};

/** returns information necessary to reconstruct all of the FUNC syms in elf
* @param elf a valid elf from elf_open
* @return a sym_list sorted by value ascending
*/
struct _sym_list * aux_func_syms           (struct _elf * elf);
int                aux_func_sym_at_address (struct _elf * elf,
                                            struct _elf_symbol * symbol,
                                            uint_t * address);
struct _sym_list * sym_list_insert         (struct _sym_list * sym_list,
                                            struct _elf_symbol * symbol);
void               sym_list_destroy        (struct _sym_list * sym_list);


#endif
