#ifndef exec_HEADER
#define exec_HEADER

#include <stdio.h>
#include <stdlib.h>

#include "elf.h"
#include "pe.h"

#define EXEC_TYPE_ELF 1
#define EXEC_TYPE_PE  2

#define EXEC_SECTION_TYPE_SYMBOL     0x1
#define EXEC_SECTION_TYPE_RELOCATION 0x2
#define EXEC_SECTION_TYPE_TEXT       0x4
#define EXEC_SECTION_TYPE_UNKNOWN    0x8

struct _exec {
    int type;
    union {
        struct _elf * elf;
        struct _pe *  pe;
    } e;
};

struct _exec_section {
    struct _exec * exec;
    union {
        struct _elf_section elf_section;
        struct _pe_section  pe_section;
    } s;
};

struct _exec * exec_open    (char * filename);
void           exec_destroy (struct _exec * exec);

int exec_type         (struct _exec * exec);
int exec_num_sections (struct _exec * exec);

int exec_section (struct _exec * exec, struct _exec_section * section, int index);

char *          exec_section_name  (struct _exec_section * section);
int             exec_section_types (struct _exec_section * section);
int             exec_section_size  (struct _exec_section * section);
unsigned char * exec_section_data  (struct _exec_section * section);

#endif
