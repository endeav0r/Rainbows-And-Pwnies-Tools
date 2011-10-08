#include <stdio.h>

#include "../src/pe.h"

int main (int argc, char * argv[])
{
    struct _pe * pe;
    int section_i;
    int symbol_i;
    struct _pe_section section;
    struct _pe_symbol symbol;
    
    pe = pe_open(argv[1]);
    
    for (symbol_i = 0;
         symbol_i < uint_t_get(pe_NumberOfSymbols(pe));
         symbol_i++) {
        if (pe_symbol_type(pe, symbol_i) == PE_SYMBOL_TYPE_AUXILIARY)
            continue;
        pe_symbol(pe, &symbol, symbol_i);
        printf("%d ", symbol_i);
        switch (PE_SYM_TYPE_COMPLEX(uint_t_get(pe_symbol_Type(&symbol)))) {
        case IMAGE_SYM_DTYPE_FUNCTION : printf("( F) "); break;
        case IMAGE_SYM_DTYPE_POINTER  : printf("( P) "); break;
        case IMAGE_SYM_MSFT_FUNCTION  : printf("( M) "); break;
        default :
            printf("(%02x) ",
                   PE_SYM_TYPE_COMPLEX(uint_t_get(pe_symbol_Type(&symbol))));
            break;
        }
        printf("%d %s\n", uint_t_get(pe_symbol_NumberOfAuxSymbols(&symbol)),
               pe_symbol_Name(&symbol));
    }
    
    for (section_i = 0;
         section_i < uint_t_get(pe_NumberOfSections(pe));
         section_i++) {
        pe_section(pe, &section, section_i);
        printf("%d %s %d\n", section_i, pe_section_Name(&section),
               uint_t_get(pe_section_PointerToRelocations(&section)));
    }
    
    pe_destroy(pe);
    
    return 0;
}
