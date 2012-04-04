#include "exec.h"


struct _exec * exec_open (char * filename)
{

    FILE * fh;
    size_t filesize;
    unsigned char buf[256];
    int offset;
    struct _exec * exec;
    
    Elf32_Ehdr * ehdr;
    
    
    fh = fopen(filename, "rb");
    if (fh == NULL) {
        fprintf(stderr, "failed to open file %s\n", filename);
        return NULL;
    }
    
    fseek(fh, 0, SEEK_END);
    filesize = ftell(fh);
    fseek(fh, 0, SEEK_SET);
    
    fread(buf, 1, filesize < 256 ? filesize : 256, fh);
    
    fclose(fh);
    
    // load pe offset now
    offset = *((int *) &(buf[0x3c]));
    
    // check for ELF
    ehdr = (Elf32_Ehdr *) buf;
    if (    (ehdr->e_ident[EI_MAG0] == ELFMAG0)
         && (ehdr->e_ident[EI_MAG1] == ELFMAG1)
         && (ehdr->e_ident[EI_MAG2] == ELFMAG2)
         && (ehdr->e_ident[EI_MAG3] == ELFMAG3)) {
        exec = (struct _exec *) malloc(sizeof(struct _exec));
        exec->type = EXEC_TYPE_ELF;
        exec->e.elf = elf_open(filename);
        // set ELF specific values
        uint_t_set(&(exec->entry), elf_entry(exec->e.elf));
    }
    // check for PE
    else if (    (buf[offset+0] == 'P')
              && (buf[offset+1] == 'E')
              && (buf[offset+2] == '\0')
              && (buf[offset+3] == '\0')) {
        exec = (struct _exec *) malloc(sizeof(struct _exec));
        exec->type = EXEC_TYPE_PE;
        exec->e.pe = pe_open(filename);
        // set PE specific values
        uint_t_set(&(exec->entry), pe_AddressOfEntryPoint(exec->e.pe));
        uint_t_add(&(exec->entry), pe_ImageBase(exec->e.pe));
    }
    else {
        fprintf(stderr, "file format for %s not recognized\n", filename);
        exec = NULL;
    }
    
    return exec;
}

void exec_destroy (struct _exec * exec)
{
    switch (exec->type) {
    case EXEC_TYPE_ELF : elf_destroy(exec->e.elf); break;
    case EXEC_TYPE_PE  : pe_destroy (exec->e.pe);  break;
    }
    
    free(exec);
}

int exec_copy (struct _exec * dst, struct _exec * src)
{
    if (dst == NULL) dst = malloc(sizeof(struct _exec));
    memcpy(dst, src, sizeof(struct _exec));
    switch (exec_type(src)) {
    case EXEC_TYPE_ELF :
        dst->e.elf = (struct _elf *) malloc(sizeof(struct _elf));
        elf_copy(dst->e.elf, src->e.elf);
        break;
    case EXEC_TYPE_PE :
        dst->e.pe = (struct _pe *) malloc(sizeof(struct _pe));
        pe_copy(dst->e.pe, src->e.pe);
        break;
    }
    return 0;
}

int exec_type (struct _exec * exec) {
    return exec->type;
}

int exec_num_sections (struct _exec * exec)
{
    switch (exec->type) {
    case EXEC_TYPE_ELF : return int_t_get(elf_shnum(exec->e.elf));
    case EXEC_TYPE_PE  : return uint_t_get(pe_NumberOfSections(exec->e.pe));
    }
    return -1;
}

int exec_num_symbols (struct _exec * exec)
{
    struct _elf_section section;
    int i;
    int symbols_n = 0;
    
    switch (exec_type(exec)) {
    case EXEC_TYPE_ELF :
        for (i = 0; i < int_t_get(elf_shnum(exec->e.elf)); i++) {
            elf_section(exec->e.elf, &section, i);
            if (    (int_t_get(elf_section_type(&section)) == SHT_SYMTAB)
                 || (int_t_get(elf_section_type(&section)) == SHT_DYNSYM))
                symbols_n += elf_section_num(&section);
                
        }
        return symbols_n;
    case EXEC_TYPE_PE :
        return pe_total_symbols(exec->e.pe);
    }
    return -1;
}

int exec_mode (struct _exec * exec)
{
    switch (exec_type(exec)) {
    case EXEC_TYPE_ELF :
        return ELF_CLASS(exec->e.elf);
    case EXEC_TYPE_PE :
        if (uint_t_get(pe_Machine(exec->e.pe)) == IMAGE_FILE_MACHINE_AMD64)
            return 64;
        else if (uint_t_get(pe_Machine(exec->e.pe)) == IMAGE_FILE_MACHINE_I386)
            return 32;
    }
    return -1;
}

int exec_size (struct _exec * exec)
{
    switch (exec_type(exec)) {
    case EXEC_TYPE_ELF :
        return exec->e.elf->bytes_size;
    case EXEC_TYPE_PE :
        return exec->e.pe->bytes_size;
    }
    return -1;
}

uint_t * exec_entry (struct _exec * exec)
{
    return &(exec->entry);
    /*
    switch (exec_type(exec)) {
    case EXEC_TYPE_ELF :
        return elf_entry(exec->e.elf);
    case EXEC_TYPE_PE :
        return pe_AddressOfEntryPoint(exec->e.pe);
    }
    return NULL;
    */
}

int exec_section (struct _exec * exec, struct _exec_section * section,
                   int index)
{
    section->exec = exec;
    
    switch (exec_type(exec)) {
    case EXEC_TYPE_ELF :
        elf_section(exec->e.elf, &(section->s.elf_section), index);
        break;
    case EXEC_TYPE_PE :
        pe_section(exec->e.pe, &(section->s.pe_section), index);
        break;
    }
    return 0;
}

int exec_symbol (struct _exec * exec, struct _exec_symbol * symbol, int index)
{
    struct _elf_section section_elf;
    struct _pe_section  section_pe;
    int section_i;
    int symbol_i;
    int symbol_found = 0;
    int class;

    symbol->exec = exec;
    
    switch (exec_type(exec)) {
    case EXEC_TYPE_ELF :
        for (section_i = 0; section_i < int_t_get(elf_shnum(exec->e.elf));
             section_i++) {
            elf_section(exec->e.elf, &section_elf, section_i);
            if (    (int_t_get(elf_section_type(&section_elf)) == SHT_SYMTAB)
                 || (int_t_get(elf_section_type(&section_elf)) == SHT_DYNSYM)) {
                if (elf_section_num(&section_elf) <= index)
                    index -= elf_section_num(&section_elf);
                else {
                    elf_section_symbol(&section_elf, &(symbol->s.elf_symbol), index);
                    symbol_found = 1;
                    break;
                }
            }
        }
        if (symbol_found == 0)
            break;
        uint_t_set(&(symbol->address), elf_symbol_value(&(symbol->s.elf_symbol)));
    case EXEC_TYPE_PE :
        // find the non-aux symbol corresponding to this index
        for (symbol_i = 0;
             symbol_i < uint_t_get(pe_NumberOfSymbols(exec->e.pe));
             symbol_i++) {
            if (pe_symbol_type(exec->e.pe, symbol_i) == PE_SYMBOL_TYPE_REGULAR)
                index--;
            if (index < 0)
                break;
        }
        
        if (index >= 0)
            break;
        symbol_found = 1;
        pe_symbol(exec->e.pe, &(symbol->s.pe_symbol), symbol_i);
        
        class = uint_t_get(pe_symbol_StorageClass(&(symbol->s.pe_symbol)));
        if (    (class == IMAGE_SYM_CLASS_STATIC)
             || (    (class == IMAGE_SYM_CLASS_EXTERNAL)
                  && (uint_t_get(pe_symbol_SectionNumber(&(symbol->s.pe_symbol)))
                      != IMAGE_SYM_UNDEFINED
                     )
                 )
             || (class == IMAGE_SYM_CLASS_LABEL)) {
            pe_section(exec->e.pe, &section_pe,
               uint_t_get(pe_symbol_SectionNumber(&(symbol->s.pe_symbol))));
            uint_t_set(&(symbol->address), pe_section_VirtualAddress(&section_pe));
            uint_t_add(&(symbol->address), pe_ImageBase(exec->e.pe));
            uint_t_add(&(symbol->address), pe_symbol_Value(&(symbol->s.pe_symbol)));
            break;
        }
        else
            uint_t_set(&(symbol->address), pe_symbol_Value(&(symbol->s.pe_symbol)));
        
        break;
    }
    return symbol_found - 1;
}                


char * exec_section_name (struct _exec_section * section)
{
    switch (exec_type(section->exec)) {
    case EXEC_TYPE_ELF : return elf_section_name(&(section->s.elf_section));
    case EXEC_TYPE_PE  : return pe_section_Name(&(section->s.pe_section));
    }
    return NULL;
}

int exec_section_types (struct _exec_section * section)
{
    int types = 0;
    switch (exec_type(section->exec)) {
    case EXEC_TYPE_ELF :
        switch (int_t_get(elf_section_type(&(section->s.elf_section)))) {
            case SHT_SYMTAB :
            case SHT_DYNSYM :
                types |= EXEC_SECTION_TYPE_SYMBOL;
                break;
            case SHT_REL  :
            case SHT_RELA :
                types |= EXEC_SECTION_TYPE_IMPORT;
                break;
            case SHT_PROGBITS :
                types |= EXEC_SECTION_TYPE_TEXT;
                break;
        }
        if (int_t_get(elf_section_flags(&(section->s.elf_section))) 
            & SHF_EXECINSTR)
            types |= EXEC_SECTION_TYPE_EXECUTABLE;
        break;
    case EXEC_TYPE_PE :
        types = 0;
        if (uint_t_get(pe_section_Characteristics(&(section->s.pe_section)))
            & IMAGE_SCN_CNT_CODE)
            types |= EXEC_SECTION_TYPE_TEXT;
        if (uint_t_get(pe_section_Characteristics(&(section->s.pe_section)))
            & IMAGE_SCN_MEM_EXECUTE)
            types |= EXEC_SECTION_TYPE_EXECUTABLE;
        break;
    }
    return types;
}

int exec_section_size (struct _exec_section * section)
{
    switch (exec_type(section->exec)) {
    case EXEC_TYPE_ELF :
        return int_t_get(elf_section_size(&(section->s.elf_section)));
    case EXEC_TYPE_PE  : 
        return uint_t_get(pe_section_SizeOfRawData(&(section->s.pe_section)));
    }
    return -1;
}

int exec_section_virtual_size (struct _exec_section * section)
{
    switch (exec_type(section->exec)) {
    case EXEC_TYPE_ELF :
        return int_t_get(elf_section_size(&(section->s.elf_section)));
    case EXEC_TYPE_PE  : 
        return uint_t_get(pe_section_VirtualSize(&(section->s.pe_section)));
    }
    return -1;
}

unsigned char * exec_section_data (struct _exec_section * section)
{
    switch (exec_type(section->exec)) {
    case EXEC_TYPE_ELF :
        return elf_section_data(&(section->s.elf_section));
    case EXEC_TYPE_PE :
        return pe_section_data(&(section->s.pe_section));
    }
    return NULL;
}

uint_t * exec_section_address (struct _exec_section * section)
{
    switch (exec_type(section->exec)) {
    case EXEC_TYPE_ELF :
        return elf_section_addr(&(section->s.elf_section));
    case EXEC_TYPE_PE :
        return pe_section_address(&(section->s.pe_section));
    }
    return NULL;
}

int exec_section_offset (struct _exec_section * section)
{
    switch (exec_type(section->exec)) {
    case EXEC_TYPE_ELF :
        return (int) uint_t_get(elf_section_offset(&(section->s.elf_section)));
    case EXEC_TYPE_PE :
        return (int) uint_t_get(pe_section_PointerToRawData(&(section->s.pe_section)))
                   * uint_t_get(pe_FileAlignment(section->exec->e.pe));
    }
    return -1;
}


uint_t * exec_symbol_value (struct _exec_symbol * symbol)
{
    switch (exec_type(symbol->exec)) {
    case EXEC_TYPE_ELF :
        return elf_symbol_value(&(symbol->s.elf_symbol));
    case EXEC_TYPE_PE :
        return pe_symbol_Value(&(symbol->s.pe_symbol));
    }
    return NULL;
}



uint_t * exec_symbol_address (struct _exec_symbol * symbol)
{
    return &(symbol->address);
}


char * exec_symbol_name (struct _exec_symbol * symbol)
{
    switch (exec_type(symbol->exec)) {
    case EXEC_TYPE_ELF : return elf_symbol_name(&(symbol->s.elf_symbol));
    case EXEC_TYPE_PE  : return pe_symbol_Name(&(symbol->s.pe_symbol));
    }
    return NULL;
}


// I must have been drunk when i thought this function was a good idea
char * exec_symbol_description (struct _exec_symbol * symbol)
{
    switch (exec_type(symbol->exec)) {
    case EXEC_TYPE_ELF :
        return elf_symbol_type_strings[elf_symbol_type(&(symbol->s.elf_symbol))];
    case EXEC_TYPE_PE :
        return pe_symbol_class_strings_helper(
                    uint_t_get(pe_symbol_StorageClass(&(symbol->s.pe_symbol))));
    }
    return NULL;
}


int exec_symbol_type (struct _exec_symbol * symbol)
{
    int symbol_type;
    switch (exec_type(symbol->exec)) {
    case EXEC_TYPE_ELF :
        if (elf_symbol_type(&(symbol->s.elf_symbol)) == STT_FUNC)
            return EXEC_SYMBOL_TYPE_FUNCTION;
        break;
    case EXEC_TYPE_PE :
        symbol_type = PE_SYM_TYPE_COMPLEX(uint_t_get(pe_symbol_Type(
                                                      &(symbol->s.pe_symbol))));
        if (    (symbol_type == IMAGE_SYM_DTYPE_FUNCTION)
             || (symbol_type == IMAGE_SYM_MSFT_FUNCTION))
            return EXEC_SYMBOL_TYPE_FUNCTION;
        break;
    }
    return EXEC_SYMBOL_TYPE_UNKNOWN;
}


int exec_symbol_size (struct _exec_symbol * symbol)
{
    switch (exec_type(symbol->exec)) {
    case EXEC_TYPE_ELF :
        return int_t_get(elf_symbol_size(&(symbol->s.elf_symbol)));
    case EXEC_TYPE_PE :
        return uint_t_get(pe_symbol_TotalSize(&(symbol->s.pe_symbol)));
    }
    return 0;
}

/*
int exec_relocation (struct _exec * exec,
                     struct _exec_relocation * relocation,
                     int index)
{
    struct _elf_section section_elf;
    struct _pe_section  section_pe;
    int section_i;
    int relocation_found = 0;
    
    switch (exec_type(exec)) {
    case EXEC_TYPE_ELF :
        for (section_i = 0;
             section_i < int_t_get(elf_shnum(exec->e.elf));
             section_i++) {
            elf_section(exec->e.elf, &(section_elf), section_i);
            if (    (int_t_get(elf_section_type(&section_elf)) == SHT_REL)
                 || (int_t_get(elf_section_type(&section_elf)) == SHT_RELA)) {
                if (elf_section_num(&section_elf) < index)
                    index -= elf_section_num(&section_elf);
                else {
                    relocation_found = 1;
                    break;
                }
            }
        }
        
        if (relocation_found == 0)
            break;
        elf_section_relocation(&section_elf, &(relocation->r.elf_relocation),
                               index);
        uint_t_set(&(relocation->address),
                   elf_relocation_offset(&(relocation->r.elf_relocation)));
        break;
    case EXEC_TYPE_PE :
        break;
    }
    
    return relocation_found - 1;
}
*/
