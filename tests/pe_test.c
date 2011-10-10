#include <stdio.h>

#include "../src/pe.h"
#include "../src/strings.h"

int main (int argc, char * argv[])
{
    struct _pe * pe;
    int section_i;
    int symbol_i;
    int relocation_i;
    struct _pe_section section;
    struct _pe_symbol symbol;
    struct _pe_relocation relocation;
    
    pe = pe_open(argv[1]);
    
    for (symbol_i = 0;
         symbol_i < uint_t_get(pe_NumberOfSymbols(pe));
         symbol_i++) {
        if (pe_symbol_type(pe, symbol_i) == PE_SYMBOL_TYPE_AUXILIARY) {
            printf("%d is auxiliary\n", symbol_i);
            continue;
        }
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
        printf("%d %s [%s]\n",
               uint_t_get(pe_symbol_NumberOfAuxSymbols(&symbol)),
               pe_symbol_Name(&symbol),
               pe_symbol_class_strings_helper(
                                  uint_t_get(pe_symbol_StorageClass(&symbol))));
    }
    
    for (section_i = 1;
         section_i <= uint_t_get(pe_NumberOfSections(pe));
         section_i++) {
        pe_section(pe, &section, section_i);
        if (uint_t_get(pe_section_NumberOfRelocations(&section)) > 0) {
            for (relocation_i = 0;
                 relocation_i < uint_t_get(pe_section_NumberOfRelocations(&section));
                 relocation_i++) {
                 printf("%d %d %s\n",
                        section_i,
                        relocation_i,
                        uint_t_strx(pe_relocation_VirtualAddress(&relocation)));
             }
        }
    }
    
    for (section_i = 1;
         section_i <= uint_t_get(pe_NumberOfSections(pe));
         section_i++) {
        pe_section(pe, &section, section_i);
        printf("%d %s   \t%s\t%s\t%s\t%d\t%d\t%d\n", section_i, pe_section_Name(&section),
               uint_t_str0x(pe_section_address(&section)),
               uint_t_strx(pe_section_VirtualSize(&section)),
               uint_t_strx(pe_section_SizeOfRawData(&section)),
               uint_t_get(pe_section_PointerToRelocations(&section)),
               (uint_t_get(pe_section_Characteristics(&section))
               & IMAGE_SCN_MEM_EXECUTE) > 0 ? 1 : 0,
               uint_t_get(pe_section_NumberOfRelocations(&section)));
    }
    
    printf("ImageBase: %s\n", uint_t_str0x(&(pe->ImageBase)));
    printf("%s\n", uint_t_strx(&(pe->BaseOfCode)));
    printf("%s\n", uint_t_strx(&(pe->AddressOfEntryPoint)));
    
    pe_destroy(pe);
    
    return 0;
}
