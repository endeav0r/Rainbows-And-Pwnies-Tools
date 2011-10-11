#ifndef elf_HEADER
#define elf_HEADER

#include <elf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../types.h"

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

struct _elf_section {
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

struct _elf_symbol {
    int index;
    struct _elf * elf;
    struct _elf_section section;
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

struct _elf_relocation {
    int index;
    struct _elf * elf;
    struct _elf_section section;
    struct _elf_symbol symbol;
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

int elf_section (struct _elf * elf, struct _elf_section * section, int index);


int      elf_symbol_type  (struct _elf_symbol * symbol);
uint_t * elf_symbol_value (struct _elf_symbol * symbol);
char *   elf_symbol_name  (struct _elf_symbol * symbol);
int_t *  elf_symbol_size  (struct _elf_symbol * symbol);
uint_t * elf_symbol_shndx (struct _elf_symbol * symbol);

char *   elf_relocation_name   (struct _elf_relocation * relocation);
uint_t * elf_relocation_offset (struct _elf_relocation * relocation);
int      elf_relocation_type   (struct _elf_relocation * relocation);

void            elf_section_copy    (struct _elf_section * dst,
                                     struct _elf_section * src);
char *          elf_section_name    (struct _elf_section * section);
uint_t *        elf_section_addr    (struct _elf_section * section);
int             elf_section_exec    (struct _elf_section * section);
int_t *         elf_section_size    (struct _elf_section * section);
uint_t *        elf_section_offset  (struct _elf_section * section);
unsigned char * elf_section_data    (struct _elf_section * section);
int_t *         elf_section_type    (struct _elf_section * section);
int             elf_section_num     (struct _elf_section * section);
int_t *         elf_section_flags   (struct _elf_section * section);
int_t *         elf_section_entsize (struct _elf_section * section);
int_t *         elf_section_link    (struct _elf_section * section);

int elf_section_symbol     (struct _elf_section * section,
                            struct _elf_symbol * symbol, int index);
int elf_section_relocation (struct _elf_section * section,
                            struct _elf_relocation * relocation, int index);

#endif
