#include "elf.h"



struct _elf * elf_open (char * filename)
{
    struct _elf * elf;
    size_t bytes_read;
    FILE * fh;
    
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
                filename, elf->bytes_size, bytes_read);
        exit(-1);
    }
    
    fclose(fh);
    
    elf->elf = (Elf32_Ehdr *) elf->bytes;
    
    return elf;
}

void elf_destroy (struct _elf * elf)
{
    free(elf->bytes);
}


// returns byte for ELFCLASSNONE, ELFCLASS32 or ELFCLASS64
unsigned char elf_type  (struct _elf * elf)    { 
    return elf->elf->e_ident[EI_CLASS]; }
int           elf_shnum (struct _elf * elf)    { return elf->elf->e_shnum; }
int           elf_shoff (struct _elf * elf)    { return elf->elf->e_shoff; }
int           elf_shstrndx (struct _elf * elf) { return elf->elf->e_shstrndx; }


// returns string from section strtab at offset
char * elf_strtab_str (struct _elf * elf, int strtab, int offset)
{
    struct _elf_shdr shdr;
    
    elf_shdr(elf, &shdr, strtab);
    
    return (char *) &(elf->bytes[shdr.shdr->sh_offset + offset]);
}


// fills _elf_shdr with Shdr at ndx
int elf_shdr (struct _elf * elf, struct _elf_shdr * shdr, int index)
{
    shdr->elf  = elf;
    shdr->index = index;
    shdr->shdr = (Elf32_Shdr *) &(elf->bytes[elf_shoff(elf) 
                                             + sizeof(Elf32_Shdr) * index]);
    return 1;
}


int shdr_sym (struct _elf_shdr * shdr, struct _elf_sym * sym, int index)
{
    unsigned char * data;
    if (shdr_type(shdr) != SHT_SYMTAB)
        return 0;
    
    sym->elf = shdr->elf;
    sym->index = index;
    elf_shdr(shdr->elf, &(sym->shdr), shdr->index);
    data = shdr_data(shdr);
    sym->sym = (Elf32_Sym *) &(data[shdr_entsize(shdr) * index]);
    
    return 1;
}


int elf_sym_func_addr (struct _elf * elf,
                       struct _elf_sym * sym,
                       unsigned int addr)
{
    int i, j;
    int best_shdr = -1;
    int best_sym = -1;
    unsigned int best_sym_addr = 0;
    struct _elf_shdr shdr;
    struct _elf_sym sym_tmp;
    
    // check all shdr to see if they're SHT_SYMTAB
    for (i = 0; i < elf_shnum(elf); i++) {
        elf_shdr(elf, &shdr, i);
        if (shdr_type(&shdr) == SHT_SYMTAB) {
            // load all symbols, pick FUNC symbol with address below and closest
            // to addr 
            for (j = 0; j < shdr_num(&shdr); j++) {
                shdr_sym(&shdr, &sym_tmp, j);
                if (    (sym_type(&sym_tmp) == STT_FUNC)
                     && (sym_addr(&sym_tmp) < addr)
                     && (sym_addr(&sym_tmp) > best_sym_addr)) {
                    best_shdr = i;
                    best_sym = j;
                    best_sym_addr = sym_addr(&sym_tmp);
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
                


int sym_type (struct _elf_sym * sym) { return ELF32_ST_TYPE(sym->sym->st_info); }
unsigned int sym_addr (struct _elf_sym * sym) { return sym->sym->st_value; }
char * sym_name (struct _elf_sym * sym) {
    return elf_strtab_str(sym->elf, shdr_link(&(sym->shdr)), sym->sym->st_name); }

unsigned int shdr_addr (struct _elf_shdr * shdr) { return shdr->shdr->sh_addr; }
int          shdr_size (struct _elf_shdr * shdr) { return shdr->shdr->sh_size; }
int          shdr_type (struct _elf_shdr * shdr) { return shdr->shdr->sh_type; }
int          shdr_link (struct _elf_shdr * shdr) { return shdr->shdr->sh_link; }
int    shdr_entsize (struct _elf_shdr * shdr) { return shdr->shdr->sh_entsize; }

int shdr_num (struct _elf_shdr * shdr) {
    return shdr->shdr->sh_size / shdr->shdr->sh_entsize; }

unsigned char * shdr_data (struct _elf_shdr * shdr) {
    return (unsigned char *) &(shdr->elf->bytes[shdr->shdr->sh_offset]); }

char * shdr_name (struct _elf_shdr * shdr) {
    return elf_strtab_str(shdr->elf, elf_shstrndx(shdr->elf), shdr->shdr->sh_name); }

int shdr_exec (struct _elf_shdr * shdr) {
    return shdr->shdr->sh_flags & SHF_EXECINSTR; }


