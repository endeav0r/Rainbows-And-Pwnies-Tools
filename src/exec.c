#include "exec.h"


struct _exec * exec_open (char * filename)
{

    FILE * fh;
    size_t filesize;
    size_t bytes_read;
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
    
    bytes_read = fread(buf, 1, filesize < 256 ? filesize : 256, fh);
    
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
    }
    // check for PE
    else if (    (buf[offset+0] == 'P')
              && (buf[offset+1] == 'E')
              && (buf[offset+2] == '\0')
              && (buf[offset+3] == '\0')) {
        exec = (struct _exec *) malloc(sizeof(struct _exec));
        exec->type = EXEC_TYPE_PE;
        exec->e.pe = pe_open(filename);
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

int exec_type (struct _exec * exec) { return exec->type; }

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
    struct _elf_section section;
    int section_i;
    int symbol_i;
    symbol->exec = exec;
    
    switch (exec_type(exec)) {
    case EXEC_TYPE_ELF :
        for (section_i = 0; section_i < int_t_get(elf_shnum(exec->e.elf));
             section_i++) {
            elf_section(exec->e.elf, &section, section_i);
            if (    (int_t_get(elf_section_type(&section)) == SHT_SYMTAB)
                 || (int_t_get(elf_section_type(&section)) == SHT_DYNSYM)) {
                if (elf_section_num(&section) <= index)
                    index -= elf_section_num(&section);
                else {
                    elf_section_symbol(&section, &(symbol->s.elf_symbol), index);
                    return 0;
                }
            }
        }
        break;
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
        pe_symbol(exec->e.pe, &(symbol->s.pe_symbol), symbol_i);
        return 0;
    }
    return -1;
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
            case SHT_REL  :
            case SHT_RELA :
                types |= EXEC_SECTION_TYPE_RELOCATION;
            case SHT_PROGBITS :
                types |= EXEC_SECTION_TYPE_TEXT;
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
        if (uint_t_get(pe_section_NumberOfRelocations(&(section->s.pe_section))))
            types |= EXEC_SECTION_TYPE_RELOCATION;
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
    struct _pe_section section;
    
    switch (exec_type(symbol->exec)) {
    case EXEC_TYPE_ELF :
        return elf_symbol_value(&(symbol->s.elf_symbol));
    case EXEC_TYPE_PE :
        switch (uint_t_get(pe_symbol_StorageClass(&(symbol->s.pe_symbol)))) {
            case IMAGE_SYM_CLASS_STATIC :
            pe_section(symbol->exec->e.pe, &section,
                       uint_t_get(pe_symbol_SectionNumber(&(symbol->s.pe_symbol))));
            uint_t_set(&(symbol->address), pe_section_VirtualAddress(&section));
            uint_t_add(&(symbol->address), pe_symbol_Value(&(symbol->s.pe_symbol)));
            return &(symbol->address);
        }
        return pe_symbol_Value(&(symbol->s.pe_symbol));
    }
    return NULL;
}


char * exec_symbol_name (struct _exec_symbol * symbol)
{
    switch (exec_type(symbol->exec)) {
    case EXEC_TYPE_ELF : return elf_symbol_name(&(symbol->s.elf_symbol));
    case EXEC_TYPE_PE  : return pe_symbol_Name(&(symbol->s.pe_symbol));
    }
    return NULL;
}


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
