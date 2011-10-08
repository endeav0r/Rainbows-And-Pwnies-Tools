#ifndef strings_HEADER
#define strings_HEADER

#define SHDR_TYPE_STRINGS_SIZE 20
#define SYM_TYPE_STRINGS_SIZE  16

extern char mnemonic_strings[][16];
extern char types_strings[][16];
extern char elf_section_type_strings[][16];
extern char elf_symbol_type_strings[][16];
extern char elf_relocation_type_strings[39][16];
extern char pe_symbol_class_strings[][32];

char * pe_symbol_class_strings_helper (int class);

#endif
