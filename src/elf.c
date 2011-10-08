#include "elf.h"


struct _elf * elf_open (char * filename)
{
    struct _elf * elf;
    size_t bytes_read;
    FILE * fh;
    
    // read in the ELF binary
    fh = fopen(filename, "rb");
    if (fh == NULL) {
        fprintf(stderr, "failed to open file %s\n", filename);
        return NULL;
    }
    
    elf = (struct _elf *) malloc(sizeof(struct _elf));
    elf->bytes = NULL;
    elf->filename = NULL;
    
    fseek(fh, 0, SEEK_END);
    elf->bytes_size = ftell(fh);
    fseek(fh, 0, SEEK_SET);
    
    elf->bytes = (unsigned char *) malloc(elf->bytes_size);
    
    bytes_read = fread(elf->bytes, 1, elf->bytes_size, fh);
    
    if (bytes_read != elf->bytes_size) {
        fprintf(stderr, "reading %s, filesize: %d but read %d bytes\n",
                filename, (int) elf->bytes_size, (int) bytes_read);
        return NULL;
    }
    
    fclose(fh);
    
    elf->filename = (char *) malloc(strlen(filename) + 1);
    strcpy(elf->filename, filename);
    
    // set up elf based 32 or 64 bit
    elf->e.elf32 = (Elf32_Ehdr *) elf->bytes;
    elf->e.elf64 = (Elf64_Ehdr *) elf->bytes;
    
    elf->type = elf->e.elf32->e_ident[EI_CLASS];
    switch (elf->type) {
    case ELFCLASS32 :
        int_t_16_set (&(elf->shnum),    elf->e.elf32->e_shnum);
        uint_t_32_set(&(elf->shoff),    elf->e.elf32->e_shoff);
        int_t_16_set (&(elf->shstrndx), elf->e.elf32->e_shstrndx);
        break;
    case ELFCLASS64 :
        int_t_16_set (&(elf->shnum),    elf->e.elf64->e_shnum);
        uint_t_64_set(&(elf->shoff),    elf->e.elf64->e_shoff);
        int_t_16_set (&(elf->shstrndx), elf->e.elf64->e_shstrndx);
        break;
    }   
    
    return elf;
}


void elf_destroy (struct _elf * elf)
{
    elf_destroy_resources(elf);
    free(elf);
}


void elf_destroy_resources (struct _elf * elf)
{
    free(elf->filename);
    free(elf->bytes);
}


int elf_copy (struct _elf * dst, struct _elf * src)
{
    if (dst == NULL) dst = (struct _elf *) malloc(sizeof(struct _elf));
    memcpy(dst, src, sizeof(struct _elf));
    dst->filename = (char *) malloc(strlen(src->filename) + 1);
    strcpy(dst->filename, src->filename);
    dst->bytes = (unsigned char *) malloc(src->bytes_size);
    memcpy(dst->bytes, src->bytes, src->bytes_size);
    return 0;
}


// returns byte for ELFCLASSNONE, ELFCLASS32 or ELFCLASS64
unsigned char elf_class    (struct _elf * elf) { return elf->type; }
char *        elf_filename (struct _elf * elf) { return elf->filename; }
int_t *       elf_shnum    (struct _elf * elf) { return &(elf->shnum); }
uint_t *      elf_shoff    (struct _elf * elf) { return &(elf->shoff); }
int_t *       elf_shstrndx (struct _elf * elf) { return &(elf->shstrndx); }


// returns string from section strtab at offset
char * elf_strtab_str (struct _elf * elf, int strtab, int offset)
{
    struct _elf_section section;
    
    elf_section(elf, &section, strtab);
    
    return (char *) &(elf->bytes[uint_t_get(elf_section_offset(&section))
                                 + offset]);
}


// fills _elf_section with Shdr at ndx
int elf_section (struct _elf * elf, struct _elf_section * section, int index)
{
    section->elf  = elf;
    section->index = index;
    if (index >= int_t_get(elf_shnum(elf)))
        return 0;
    switch (elf_class(elf)) {
    case ELFCLASS32 :
        section->s.shdr32 = (Elf32_Shdr *) &(elf->bytes[uint_t_get(elf_shoff(elf))
                                                        + sizeof(Elf32_Shdr) * index]);
        int_t_32_set (&(section->name),    section->s.shdr32->sh_name);
        int_t_32_set (&(section->type),    section->s.shdr32->sh_type);
        int_t_32_set (&(section->flags),   section->s.shdr32->sh_flags);
        uint_t_32_set(&(section->addr),    section->s.shdr32->sh_addr);
        uint_t_32_set(&(section->offset),  section->s.shdr32->sh_offset);
        int_t_32_set (&(section->size),    section->s.shdr32->sh_size);
        int_t_32_set (&(section->link),    section->s.shdr32->sh_link);
        int_t_32_set (&(section->entsize), section->s.shdr32->sh_entsize);
        break;
    case ELFCLASS64 :
        section->s.shdr64 = (Elf64_Shdr *) &(elf->bytes[uint_t_get(elf_shoff(elf))
                                                        + sizeof(Elf64_Shdr) * index]);
        int_t_64_set (&(section->name),    section->s.shdr64->sh_name);
        int_t_64_set (&(section->type),    section->s.shdr64->sh_type);
        int_t_64_set(&(section->flags),    section->s.shdr64->sh_flags);
        uint_t_64_set(&(section->addr),    section->s.shdr64->sh_addr);
        uint_t_64_set(&(section->offset),  section->s.shdr64->sh_offset);
        int_t_64_set (&(section->size),    section->s.shdr64->sh_size);
        int_t_64_set (&(section->link),    section->s.shdr64->sh_link);
        int_t_64_set (&(section->entsize), section->s.shdr64->sh_entsize);
        break;
    default :
        return 0;
    }
    return 1;
}


int elf_section_symbol (struct _elf_section * section, 
                        struct _elf_symbol * symbol, int index)
{
    unsigned char * data;
    
    if (    (int_t_get(elf_section_type(section)) != SHT_SYMTAB)
         && (int_t_get(elf_section_type(section)) != SHT_DYNSYM))
        return 0;
    
    symbol->elf = section->elf;
    symbol->index = index;
    elf_section(section->elf, &(symbol->section), section->index);
    
    data = elf_section_data(section);
    switch (elf_class(section->elf)) {
    case ELFCLASS32 :
        symbol->s.sym32 = (Elf32_Sym *) &(data[int_t_get(elf_section_entsize(section))
                                            * index]);
        symbol->type  = ELF32_ST_TYPE(symbol->s.sym32->st_info);
        uint_t_32_set(&(symbol->value), symbol->s.sym32->st_value);
        int_t_32_set (&(symbol->name), symbol->s.sym32->st_name);
        int_t_32_set (&(symbol->size), symbol->s.sym32->st_size);
        uint_t_16_set(&(symbol->shndx), symbol->s.sym32->st_shndx);
        break;
    case ELFCLASS64 :
        symbol->s.sym64 = (Elf64_Sym *) &(data[int_t_get(elf_section_entsize(section))
                                            * index]);
        symbol->type  = ELF64_ST_TYPE(symbol->s.sym64->st_info);
        uint_t_64_set(&(symbol->value), symbol->s.sym64->st_value);
        int_t_64_set (&(symbol->name), symbol->s.sym64->st_name);
        int_t_64_set (&(symbol->size), symbol->s.sym64->st_size);
        uint_t_16_set(&(symbol->shndx), symbol->s.sym64->st_shndx);
        break;
    }
    return 1;
}


int elf_section_relocation (struct _elf_section * section, 
                            struct _elf_relocation * relocation, int index)
{
    unsigned char * data;
    struct _elf_section link;
    
    if (    (int_t_get(elf_section_type(section)) != SHT_REL)
         && (int_t_get(elf_section_type(section)) != SHT_RELA))
        return 0;
    
    relocation->elf = section->elf;
    relocation->index = index;
    elf_section(section->elf, &(relocation->section), section->index);
    data = elf_section_data(section);
    

    switch (elf_class(section->elf)) {
    case ELFCLASS32 :
        if (int_t_get(elf_section_type(section)) == SHT_REL) {
            relocation->r.rel32 = (Elf32_Rel *)
                       &(data[int_t_get(elf_section_entsize(section)) * index]);
            elf_section(section->elf, &link, int_t_get(elf_section_link(section)));
            elf_section_symbol(&link, &(relocation->symbol), 
                               ELF32_R_SYM(relocation->r.rel32->r_info));
            uint_t_32_set(&(relocation->offset), relocation->r.rel32->r_offset);
            relocation->type = ELF32_R_TYPE(relocation->r.rel32->r_info);
            break;
        }
        else {
            relocation->r.rela32 = (Elf32_Rela *)
                       &(data[int_t_get(elf_section_entsize(section)) * index]);
            elf_section(section->elf, &link, int_t_get(elf_section_link(section)));
            elf_section_symbol(&link, &(relocation->symbol),
                                     ELF32_R_SYM(relocation->r.rela32->r_info));
            uint_t_32_set(&(relocation->offset), relocation->r.rela32->r_offset);
            relocation->type = ELF32_R_TYPE(relocation->r.rela32->r_info);
        }
        break;
    case ELFCLASS64 :
        if (int_t_get(elf_section_type(section)) == SHT_REL) {
            relocation->r.rel64 = (Elf64_Rel *)
                       &(data[int_t_get(elf_section_entsize(section)) * index]);
            elf_section(section->elf, &link, int_t_get(elf_section_link(section)));
            elf_section_symbol(&link, &(relocation->symbol),
                                      ELF64_R_SYM(relocation->r.rel64->r_info));
            uint_t_64_set(&(relocation->offset), relocation->r.rel64->r_offset);
            relocation->type = ELF64_R_TYPE(relocation->r.rel64->r_info);
            break;
        }
        else {
            relocation->r.rela64 = (Elf64_Rela *)
                       &(data[int_t_get(elf_section_entsize(section)) * index]);
            elf_section(section->elf, &link, int_t_get(elf_section_link(section)));
            elf_section_symbol(&link, &(relocation->symbol),
                                     ELF64_R_SYM(relocation->r.rela64->r_info));
            uint_t_64_set(&(relocation->offset), relocation->r.rela64->r_offset);
            relocation->type = ELF64_R_TYPE(relocation->r.rela64->r_info);
        }
        break;
    }
    return 1;
}
                

int      elf_symbol_type  (struct _elf_symbol * symbol) { return symbol->type; }
uint_t * elf_symbol_value (struct _elf_symbol * symbol) { return &(symbol->value); }
int_t  * elf_symbol_size  (struct _elf_symbol * symbol) { return &(symbol->size); }
uint_t * elf_symbol_shndx (struct _elf_symbol * symbol) { return &(symbol->shndx); }
char *   elf_symbol_name  (struct _elf_symbol * symbol)
{
    return elf_strtab_str(symbol->elf,
                          int_t_get(elf_section_link(&(symbol->section))),
                          int_t_get(&(symbol->name)));
}
                          


char *   elf_relocation_name   (struct _elf_relocation * relocation) { return elf_symbol_name(&(relocation->symbol)); }
uint_t * elf_relocation_offset (struct _elf_relocation * relocation) { return &(relocation->offset); }
int      elf_relocation_type   (struct _elf_relocation * relocation) { return relocation->type; }


uint_t * elf_section_addr    (struct _elf_section * section) { return &(section->addr); }
int_t *  elf_section_size    (struct _elf_section * section) { return &(section->size); }
uint_t * elf_section_offset  (struct _elf_section * section) { return &(section->offset); }
int_t *  elf_section_type    (struct _elf_section * section) { return &(section->type); }
int_t *  elf_section_link    (struct _elf_section * section) { return &(section->link); }
int_t *  elf_section_entsize (struct _elf_section * section) { return &(section->entsize); }
int_t *  elf_section_flags   (struct _elf_section * section) { return &(section->flags); }

void elf_section_copy (struct _elf_section * dst, struct _elf_section * src)
{
    memcpy(dst, src, sizeof(struct _elf_section));
}

int elf_section_exec (struct _elf_section * section) {
    return int_t_get(&(section->flags)) & SHF_EXECINSTR; }

int elf_section_num (struct _elf_section * section) {
    return int_t_get(&(section->size)) / int_t_get(&(section->entsize)); }

unsigned char * elf_section_data (struct _elf_section * section) {
    return (unsigned char *) &(section->elf->bytes[uint_t_get(&(section->offset))]); }

char * elf_section_name (struct _elf_section * section)
{
    return elf_strtab_str(section->elf,
                          int_t_get(elf_shstrndx(section->elf)),
                          int_t_get(&(section->name)));
}



/*
int elf_sym_func_addr (struct _elf * elf,
                       struct _elf_sym * sym,
                       uint_t * addr)
{
    int i, j;
    int best_section = -1;
    int best_sym = -1;
    uint_t best_sym_addr;
    struct _elf_section section;
    struct _elf_sym sym_tmp;
    
    // sets best_sym_addr same size as addr and zeroes it out
    uint_t_set(&best_sym_addr, addr);
    uint_t_sub(&best_sym_addr, addr);
    
    // check all sections to see if they're SHT_SYMTAB
    for (i = 0; i < int_t_get(elf_shnum(elf)); i++) {
        elf_section(elf, &section, i);
        if (int_t_get(elf_section_type(&section)) == SHT_SYMTAB) {
            // load all symbols, pick FUNC symbol with address below and closest
            // to addr 
            for (j = 0; j < elf_section_num(&section); j++) {
                elf_section_sym(&section, &sym_tmp, j);
                if (    (sym_type(&sym_tmp) == STT_FUNC)
                     && (uint_t_cmp(sym_value(&sym_tmp), addr) < 0)
                     && (uint_t_cmp(sym_value(&sym_tmp), &best_sym_addr) > 0)) {
                    best_section = i;
                    best_sym = j;
                    uint_t_set(&best_sym_addr, sym_value(&sym_tmp));
                }
            }
        }
    }
     
    if (best_section == -1)
        return 0;
    
    elf_section(elf, &section, best_section);
    elf_section_sym(&section, sym, best_sym);
    
    return 1;
}
*/
