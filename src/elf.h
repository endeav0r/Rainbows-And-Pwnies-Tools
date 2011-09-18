#ifndef elf_HEADER
#define elf_HEADER

#include <elf.h>
#include <stdio.h>
#include <stdlib.h>

struct _elf {
    size_t bytes_size;
    unsigned char * bytes;
    Elf32_Ehdr * elf;
};

struct _elf_shdr {
    int index;
    struct _elf * elf;
    Elf32_Shdr * shdr;
};

struct _elf_sym {
    int index;
    struct _elf * elf;
    struct _elf_shdr shdr;
    Elf32_Sym * sym;
};

struct _elf_rel {
    int index;
    struct _elf * elf;
    struct _elf_shdr shdr;
    struct _elf_sym sym;
    union {
        Elf32_Rel  * rel;
        Elf32_Rela * rela;
    };
};

struct _elf * elf_open    (char * filename);
void          elf_destroy (struct _elf * elf);

unsigned char elf_class      (struct _elf * elf);
int           elf_shnum      (struct _elf * elf);
int           elf_shoff      (struct _elf * elf);
int           elf_shstrndx   (struct _elf * elf);
char        * elf_strtab_str (struct _elf * elf, int strtab, int offset);

int elf_shdr          (struct _elf * elf, struct _elf_shdr * shdr, int index);
int elf_sym_func_addr (struct _elf * elf,
                       struct _elf_sym * sym,
                       unsigned int addr);
int shdr_sym (struct _elf_shdr * shdr, struct _elf_sym * sym, int index);
int shdr_rel (struct _elf_shdr * shdr, struct _elf_rel * rel, int index);

int          sym_type (struct _elf_sym * sym);
unsigned int sym_addr (struct _elf_sym * sym);
char *       sym_name (struct _elf_sym * sym);

char *       rel_name   (struct _elf_rel * rel);
unsigned int rel_offset (struct _elf_rel * rel);
int          rel_type   (struct _elf_rel * rel);

char *          shdr_name    (struct _elf_shdr * shdr);
unsigned int    shdr_addr    (struct _elf_shdr * shdr);
int             shdr_exec    (struct _elf_shdr * shdr);
int             shdr_size    (struct _elf_shdr * shdr);
unsigned char * shdr_data    (struct _elf_shdr * shdr);
int             shdr_type    (struct _elf_shdr * shdr);
int             shdr_num     (struct _elf_shdr * shdr);
int             shdr_entsize (struct _elf_shdr * shdr);
int             shdr_link    (struct _elf_shdr * shdr);


#endif
