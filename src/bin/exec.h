#ifndef exec_HEADER
#define exec_HEADER

#include <stdio.h>
#include <stdlib.h>

#include "elf.h"
#include "pe.h"
#include "../strings.h"
#include "../types.h"

#define EXEC_TYPE_UNKNOWN 0
#define EXEC_TYPE_ELF     1
#define EXEC_TYPE_PE      2

#define EXEC_SECTION_TYPE_SYMBOL     0x1
#define EXEC_SECTION_TYPE_IMPORT     0x2
#define EXEC_SECTION_TYPE_TEXT       0x4
#define EXEC_SECTION_TYPE_UNKNOWN    0x8
#define EXEC_SECTION_TYPE_EXECUTABLE 0x10

#define EXEC_SYMBOL_TYPE_UNKNOWN  0
#define EXEC_SYMBOL_TYPE_FUNCTION 1

struct _exec {
    int type;
    union {
        struct _elf * elf;
        struct _pe *  pe;
    } e;
    uint_t entry;
};

struct _exec_section {
    struct _exec * exec;
    union {
        struct _elf_section elf_section;
        struct _pe_section  pe_section;
    } s;
};

struct _exec_symbol {
    struct _exec * exec;
    union {
        struct _elf_symbol elf_symbol;
        struct _pe_symbol  pe_symbol;
    } s;
    uint_t address;
};

struct _exec_import {
    struct _exec * exec;
    union {
        struct _elf_relocation elf_relocation;
    } i;
    uint_t address;
};

struct _exec * exec_open    (char * filename);
void           exec_destroy (struct _exec * exec);
int            exec_copy    (struct _exec * dst, struct _exec * src);

int      exec_type         (struct _exec * exec);
int      exec_num_sections (struct _exec * exec);
int      exec_num_symbols  (struct _exec * exec);
int      exec_mode         (struct _exec * exec);
int      exec_size         (struct _exec * exec);
uint_t * exec_entry        (struct _exec * exec);

int exec_find_functions (struct _exec * exec);

int exec_section (struct _exec * exec, struct _exec_section * section, int index);
int exec_symbol  (struct _exec * exec, struct _exec_symbol * symbol,   int index);
/*
int exec_relocation (struct _exec * exec,
                     struct _exec_relocation * relocation,
                     int index);
*/

char *          exec_section_name         (struct _exec_section * section);
int             exec_section_types        (struct _exec_section * section);
int             exec_section_size         (struct _exec_section * section);
int             exec_section_virtual_size (struct _exec_section * section);
unsigned char * exec_section_data         (struct _exec_section * section);
uint_t *        exec_section_address      (struct _exec_section * section);
int             exec_section_offset       (struct _exec_section * section);

char *   exec_symbol_name        (struct _exec_symbol * symbol);
uint_t * exec_symbol_value       (struct _exec_symbol * symbol);
uint_t * exec_symbol_address     (struct _exec_symbol * symbol);
char *   exec_symbol_description (struct _exec_symbol * symbol);
int      exec_symbol_type        (struct _exec_symbol * symbol);
int      exec_symbol_size        (struct _exec_symbol * symbol);

uint_t * exec_import_address (struct _exec_import * import);
char *   exec_import_name    (struct _exec_import * import);

#endif
