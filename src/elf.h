#ifndef elf_HEADER
#define elf_HEADER

#include <elf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "types.h"

#define ELF_CLASS(e) (elf_class(e) == ELFCLASS32 ? 32 : 64)

struct _elf {
    char * filename;
    size_t bytes_size;
    unsigned char * bytes;
    union {
        Elf32_Ehdr * elf32;
        Elf64_Ehdr * elf64;
    } e;
    unsigned char type;
    int_t shnum;
    uint_t shoff;
    int_t shstrndx;
};

struct _elf_shdr {
    int index;
    struct _elf * elf;
    union {
        Elf32_Shdr * shdr32;
        Elf64_Shdr * shdr64;
    } s;
    int_t  name;
    int_t  type;
    int_t  flags;
    uint_t addr;
    uint_t offset;
    int_t  size;
    int_t  link;
    int_t  entsize;
};

struct _elf_sym {
    int index;
    struct _elf * elf;
    struct _elf_shdr shdr;
    union {
        Elf32_Sym * sym32;
        Elf64_Sym * sym64;
    } s;
    int    type;
    uint_t value;
    int_t  name;
    int_t  size;
    uint_t shndx;
};

struct _elf_rel {
    int index;
    struct _elf * elf;
    struct _elf_shdr shdr;
    struct _elf_sym sym;
    union {
        Elf32_Rel  * rel32;
        Elf32_Rela * rela32;
        Elf64_Rel  * rel64;
        Elf64_Rela * rela64;
    } r;
    uint_t offset;
    int type;
};

struct _elf * elf_open              (char * filename);
void          elf_destroy           (struct _elf * elf);
void          elf_destroy_resources (struct _elf * elf);
int           elf_copy              (struct _elf * dst, struct _elf * src);

unsigned char elf_class      (struct _elf * elf);
char *        elf_filename   (struct _elf * elf);
int_t *       elf_shnum      (struct _elf * elf);
uint_t *      elf_shoff      (struct _elf * elf);
int_t *       elf_shstrndx   (struct _elf * elf);
char *        elf_strtab_str (struct _elf * elf, int strtab, int offset);

int elf_shdr (struct _elf * elf, struct _elf_shdr * shdr, int index);
int shdr_sym (struct _elf_shdr * shdr, struct _elf_sym * sym, int index);
int shdr_rel (struct _elf_shdr * shdr, struct _elf_rel * rel, int index);

int      sym_type  (struct _elf_sym * sym);
uint_t * sym_value (struct _elf_sym * sym);
char *   sym_name  (struct _elf_sym * sym);
int_t *  sym_size  (struct _elf_sym * sym);
uint_t * sym_shndx (struct _elf_sym * sym);

char *   rel_name   (struct _elf_rel * rel);
uint_t * rel_offset (struct _elf_rel * rel);
int      rel_type   (struct _elf_rel * rel);

void            shdr_copy    (struct _elf_shdr * dst, struct _elf_shdr * src);
char *          shdr_name    (struct _elf_shdr * shdr);
uint_t *        shdr_addr    (struct _elf_shdr * shdr);
int             shdr_exec    (struct _elf_shdr * shdr);
int_t *         shdr_size    (struct _elf_shdr * shdr);
uint_t *        shdr_offset  (struct _elf_shdr * shdr);
unsigned char * shdr_data    (struct _elf_shdr * shdr);
int_t *         shdr_type    (struct _elf_shdr * shdr);
int             shdr_num     (struct _elf_shdr * shdr);
int_t *         shdr_entsize (struct _elf_shdr * shdr);
int_t *         shdr_link    (struct _elf_shdr * shdr);

int elf_sym_func_addr (struct _elf * elf,
                       struct _elf_sym * sym,
                       uint_t * addr);

#endif
