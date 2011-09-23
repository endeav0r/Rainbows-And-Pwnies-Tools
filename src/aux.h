#ifndef aux_HEADER
#define aux_HEADER

#include <stdlib.h>

#include "elf.h"
#include "types.h"

struct _sym_list {
    int shdr_i;
    int sym_i;
    uint_t value;
    struct _sym_list * next;
};

/** returns information necessary to reconstruct all of the FUNC syms in elf
* @param elf a valid elf from elf_open
* @return a sym_list sorted by value ascending
*/
struct _sym_list * aux_func_syms           (struct _elf * elf);
struct _sym_list * sym_list_insert (struct _sym_list * sym_list,
                                    struct _elf_sym * sym);
void               sym_list_destroy        (struct _sym_list * sym_list);

#endif
