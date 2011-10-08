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

int exec_type (struct _exec * exec) { return exec->type; }

int exec_num_sections (struct _exec * exec)
{
    switch (exec->type) {
    case EXEC_TYPE_ELF : return int_t_get(elf_shnum(exec->e.elf));
    case EXEC_TYPE_PE  : return uint_t_get(pe_NumberOfSections(exec->e.pe));
    }
    return -1;
}

int exec_section (struct _exec * exec, struct _exec_section * exec_section,
                   int index)
{
    exec_section->exec = exec;
    
    switch (exec_type(exec)) {
    case EXEC_TYPE_ELF :
        elf_section(exec->e.elf, &(exec_section->s.elf_section), index);
        break;
    case EXEC_TYPE_PE :
        pe_section(exec->e.pe, &(exec_section->s.pe_section), index);
        break;
    }
    return 0;
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
    int types;
    switch (exec_type(section->exec)) {
    case EXEC_TYPE_ELF :
        switch (int_t_get(elf_section_type(&(section->s.elf_section)))) {
            case SHT_SYMTAB :
            case SHT_DYNSYM :
                return EXEC_SECTION_TYPE_SYMBOL;
            case SHT_REL  :
            case SHT_RELA :
                return EXEC_SECTION_TYPE_RELOCATION;
            case SHT_PROGBITS :
                return EXEC_SECTION_TYPE_TEXT;
        }
        break;
    case EXEC_TYPE_PE :
        types = 0;
        if (uint_t_get(pe_section_Characteristics(&(section->s.pe_section)))
            & IMAGE_SCN_CNT_CODE)
            types |= EXEC_SECTION_TYPE_TEXT;
        if (uint_t_get(pe_section_NumberOfRelocations(&(section->s.pe_section))))
            types |= EXEC_SECTION_TYPE_RELOCATION;
        return types;
        break;
    }
    return 0;
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
