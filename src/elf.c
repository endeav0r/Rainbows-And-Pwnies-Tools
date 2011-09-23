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
    
    fseek(fh, 0, SEEK_END);
    elf->bytes_size = ftell(fh);
    fseek(fh, 0, SEEK_SET);
    
    elf->bytes = (unsigned char *) malloc(elf->bytes_size);
    
    bytes_read = fread(elf->bytes, 1, elf->bytes_size, fh);
    
    if (bytes_read != elf->bytes_size) {
        fprintf(stderr, "reading %s, filesize: %d but read %d bytes\n",
                filename, (int) elf->bytes_size, (int) bytes_read);
        exit(-1);
    }
    
    fclose(fh);
    
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
    free(elf->bytes);
    free(elf);
}


// returns byte for ELFCLASSNONE, ELFCLASS32 or ELFCLASS64
unsigned char elf_class  (struct _elf * elf) { return elf->type; }
int_t * elf_shnum    (struct _elf * elf) { return &(elf->shnum); }
uint_t * elf_shoff    (struct _elf * elf) { return &(elf->shoff); }
int_t * elf_shstrndx (struct _elf * elf) { return &(elf->shstrndx); }


// returns string from section strtab at offset
char * elf_strtab_str (struct _elf * elf, int strtab, int offset)
{
    struct _elf_shdr shdr;
    
    elf_shdr(elf, &shdr, strtab);
    
    return (char *) &(elf->bytes[uint_t_get(shdr_offset(&shdr)) + offset]);
}


// fills _elf_shdr with Shdr at ndx
int elf_shdr (struct _elf * elf, struct _elf_shdr * shdr, int index)
{
    shdr->elf  = elf;
    shdr->index = index;
    switch (elf_class(elf)) {
    case ELFCLASS32 :
        shdr->s.shdr32 = (Elf32_Shdr *) &(elf->bytes[uint_t_get(elf_shoff(elf))
                                                     + sizeof(Elf32_Shdr) * index]);
        int_t_32_set (&(shdr->name),    shdr->s.shdr32->sh_name);
        int_t_32_set (&(shdr->type),    shdr->s.shdr32->sh_type);
        int_t_32_set (&(shdr->flags),   shdr->s.shdr32->sh_flags);
        uint_t_32_set(&(shdr->addr),    shdr->s.shdr32->sh_addr);
        uint_t_32_set(&(shdr->offset),  shdr->s.shdr32->sh_offset);
        int_t_32_set (&(shdr->size),    shdr->s.shdr32->sh_size);
        int_t_32_set (&(shdr->link),    shdr->s.shdr32->sh_link);
        int_t_32_set (&(shdr->entsize), shdr->s.shdr32->sh_entsize);
        break;
    case ELFCLASS64 :
        shdr->s.shdr64 = (Elf64_Shdr *) &(elf->bytes[uint_t_get(elf_shoff(elf))
                                                     + sizeof(Elf64_Shdr) * index]);
        int_t_64_set (&(shdr->name),    shdr->s.shdr64->sh_name);
        int_t_64_set (&(shdr->type),    shdr->s.shdr64->sh_type);
        int_t_64_set(&(shdr->flags),    shdr->s.shdr64->sh_flags);
        uint_t_64_set(&(shdr->addr),    shdr->s.shdr64->sh_addr);
        uint_t_64_set(&(shdr->offset),  shdr->s.shdr64->sh_offset);
        int_t_64_set (&(shdr->size),    shdr->s.shdr64->sh_size);
        int_t_64_set (&(shdr->link),    shdr->s.shdr64->sh_link);
        int_t_64_set (&(shdr->entsize), shdr->s.shdr64->sh_entsize);
        break;
    }
    return 1;
}


int shdr_sym (struct _elf_shdr * shdr, struct _elf_sym * sym, int index)
{
    unsigned char * data;
    
    if (    (int_t_get(shdr_type(shdr)) != SHT_SYMTAB)
         && (int_t_get(shdr_type(shdr)) != SHT_DYNSYM))
        return 0;
    
    sym->elf = shdr->elf;
    sym->index = index;
    elf_shdr(shdr->elf, &(sym->shdr), shdr->index);
    
    data = shdr_data(shdr);
    switch (elf_class(shdr->elf)) {
    case ELFCLASS32 :
        sym->s.sym32 = (Elf32_Sym *) &(data[int_t_get(shdr_entsize(shdr)) * index]);
        sym->type  = ELF32_ST_TYPE(sym->s.sym32->st_info);
        uint_t_32_set(&(sym->value), sym->s.sym32->st_value);
        int_t_32_set (&(sym->name), sym->s.sym32->st_name);
        int_t_32_set (&(sym->size), sym->s.sym32->st_size);
        uint_t_16_set(&(sym->shndx), sym->s.sym32->st_shndx);
        break;
    case ELFCLASS64 :
        sym->s.sym64 = (Elf64_Sym *) &(data[int_t_get(shdr_entsize(shdr)) * index]);
        sym->type  = ELF64_ST_TYPE(sym->s.sym64->st_info);
        uint_t_64_set(&(sym->value), sym->s.sym64->st_value);
        int_t_64_set (&(sym->name), sym->s.sym64->st_name);
        int_t_64_set (&(sym->size), sym->s.sym64->st_size);
        uint_t_16_set(&(sym->shndx), sym->s.sym64->st_shndx);
        break;
    }
    return 1;
}


int shdr_rel (struct _elf_shdr * shdr, struct _elf_rel * rel, int index)
{
    unsigned char * data;
    struct _elf_shdr link;
    
    if (    (int_t_get(shdr_type(shdr)) != SHT_REL)
         && (int_t_get(shdr_type(shdr)) != SHT_RELA))
        return 0;
    
    rel->elf = shdr->elf;
    rel->index = index;
    elf_shdr(shdr->elf, &(rel->shdr), shdr->index);
    data = shdr_data(shdr);
    

    switch (elf_class(shdr->elf)) {
    case ELFCLASS32 :
        if (int_t_get(shdr_type(shdr)) == SHT_REL) {
            rel->r.rel32 = (Elf32_Rel *) &(data[int_t_get(shdr_entsize(shdr)) * index]);
            elf_shdr(shdr->elf, &link, int_t_get(shdr_link(shdr)));
            shdr_sym(&link, &(rel->sym), ELF32_R_SYM(rel->r.rel32->r_info));
            uint_t_32_set(&(rel->offset), rel->r.rel32->r_offset);
            rel->type = ELF32_R_TYPE(rel->r.rel32->r_info);
            break;
        }
        else {
            rel->r.rela32 = (Elf32_Rela *) &(data[int_t_get(shdr_entsize(shdr)) * index]);
            elf_shdr(shdr->elf, &link, int_t_get(shdr_link(shdr)));
            shdr_sym(&link, &(rel->sym), ELF32_R_SYM(rel->r.rela32->r_info));
            uint_t_32_set(&(rel->offset), rel->r.rela32->r_offset);
            rel->type = ELF32_R_TYPE(rel->r.rela32->r_info);
        }
        break;
    case ELFCLASS64 :
        if (int_t_get(shdr_type(shdr)) == SHT_REL) {
            rel->r.rel64 = (Elf64_Rel *) &(data[int_t_get(shdr_entsize(shdr)) * index]);
            elf_shdr(shdr->elf, &link, int_t_get(shdr_link(shdr)));
            shdr_sym(&link, &(rel->sym), ELF64_R_SYM(rel->r.rel64->r_info));
            uint_t_64_set(&(rel->offset), rel->r.rel64->r_offset);
            rel->type = ELF64_R_TYPE(rel->r.rel64->r_info);
            break;
        }
        else {
            rel->r.rela64 = (Elf64_Rela *) &(data[int_t_get(shdr_entsize(shdr)) * index]);
            elf_shdr(shdr->elf, &link, int_t_get(shdr_link(shdr)));
            shdr_sym(&link, &(rel->sym), ELF64_R_SYM(rel->r.rela64->r_info));
            uint_t_64_set(&(rel->offset), rel->r.rela64->r_offset);
            rel->type = ELF64_R_TYPE(rel->r.rela64->r_info);
        }
        break;
    }
    return 1;
}
                

int      sym_type  (struct _elf_sym * sym) { return sym->type; }
uint_t * sym_value (struct _elf_sym * sym) { return &(sym->value); }
int_t  * sym_size  (struct _elf_sym * sym) { return &(sym->size); }
uint_t * sym_shndx (struct _elf_sym * sym) { return &(sym->shndx); }
char *   sym_name  (struct _elf_sym * sym)
{
    return elf_strtab_str(sym->elf,
                          int_t_get(shdr_link(&(sym->shdr))),
                          int_t_get(&(sym->name)));
}
                          


char *   rel_name   (struct _elf_rel * rel) { return sym_name(&(rel->sym)); }
uint_t * rel_offset (struct _elf_rel * rel) { return &(rel->offset); }
int      rel_type   (struct _elf_rel * rel) { return rel->type; }



uint_t * shdr_addr    (struct _elf_shdr * shdr) { return &(shdr->addr); }
int_t *  shdr_size    (struct _elf_shdr * shdr) { return &(shdr->size); }
uint_t * shdr_offset  (struct _elf_shdr * shdr) { return &(shdr->offset); }
int_t *  shdr_type    (struct _elf_shdr * shdr) { return &(shdr->type); }
int_t *  shdr_link    (struct _elf_shdr * shdr) { return &(shdr->link); }
int_t *  shdr_entsize (struct _elf_shdr * shdr) { return &(shdr->entsize); }


int shdr_exec (struct _elf_shdr * shdr) {
    return int_t_get(&(shdr->flags)) & SHF_EXECINSTR; }

int shdr_num (struct _elf_shdr * shdr) {
    return int_t_get(&(shdr->size)) / int_t_get(&(shdr->entsize)); }

unsigned char * shdr_data (struct _elf_shdr * shdr) {
    return (unsigned char *) &(shdr->elf->bytes[uint_t_get(&(shdr->offset))]); }

char * shdr_name (struct _elf_shdr * shdr)
{
    return elf_strtab_str(shdr->elf,
                          int_t_get(elf_shstrndx(shdr->elf)),
                          int_t_get(&(shdr->name)));
}




int elf_sym_func_addr (struct _elf * elf,
                       struct _elf_sym * sym,
                       uint_t * addr)
{
    int i, j;
    int best_shdr = -1;
    int best_sym = -1;
    uint_t best_sym_addr;
    struct _elf_shdr shdr;
    struct _elf_sym sym_tmp;
    
    uint_t_make(&best_sym_addr, addr);
    
    // check all shdr to see if they're SHT_SYMTAB
    for (i = 0; i < int_t_get(elf_shnum(elf)); i++) {
        elf_shdr(elf, &shdr, i);
        if (int_t_get(shdr_type(&shdr)) == SHT_SYMTAB) {
            // load all symbols, pick FUNC symbol with address below and closest
            // to addr 
            for (j = 0; j < shdr_num(&shdr); j++) {
                shdr_sym(&shdr, &sym_tmp, j);
                if (    (sym_type(&sym_tmp) == STT_FUNC)
                     && (uint_t_cmp(sym_value(&sym_tmp), addr) < 0)
                     && (uint_t_cmp(sym_value(&sym_tmp), &best_sym_addr) > 0)) {
                    best_shdr = i;
                    best_sym = j;
                    uint_t_set(&best_sym_addr, sym_value(&sym_tmp));
                }
            }
        }
    }
     
    if (best_shdr == -1)
        return 0;
    
    elf_shdr(elf, &shdr, best_shdr);
    shdr_sym(&shdr, sym, best_sym);
    
    return 1;
}
