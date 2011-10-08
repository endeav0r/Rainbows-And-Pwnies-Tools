#ifndef pe_HEADER
#define pe_HEADER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "types.h"
#include "pe_spec.h"

#define PE_NAME_SIZE 64
#define PE_SYMBOL_TYPE_REGULAR   1
#define PE_SYMBOL_TYPE_AUXILIARY 2

struct _pe {
    char * filename;
    size_t bytes_size;
    unsigned char * bytes;
    Pe_FileHeader * FileHeader;
    int FileHeader_offset;
    unsigned char * symbol_types;
    int string_table_offset;
    uint_t Machine;
    uint_t NumberOfSections;
    uint_t TimeDateStamp;
    uint_t PointerToSymbolTable;
    uint_t NumberOfSymbols;
    uint_t SizeOfOptionalHeader;
    uint_t Characteristics;
};

struct _pe_section {
    int index;
    struct _pe * pe;
    Pe_SectionHeader SectionHeader;
    char Name[PE_NAME_SIZE];
    uint_t VirtualSize;
    uint_t VirtualAddress;
    uint_t SizeOfRawData;
    uint_t PointerToRawData;
    uint_t PointerToRelocations;
    uint_t PointerToLinenumbers;
    uint_t NumberOfRelocations;
    uint_t NumberOfLinenumbers;
    uint_t Characteristics;
};

struct _pe_symbol {
    int index;
    struct _pe * pe;
    Pe_Symbol Symbol;
    char Name[PE_NAME_SIZE];
    uint_t Value;
    uint_t SectionNumber;
    uint_t Type;
    uint_t StorageClass;
    uint_t NumberOfAuxSymbols;
};

struct _pe_relocation {
    int index;
    struct _pe * pe;
    Pe_Relocation Relocation;
    uint_t VirtualAddress;
    uint_t SymbolTableIndex;
    uint_t Type;
};
    


struct _pe * pe_open    (char * filename);
void         pe_destroy (struct _pe * pe);

uint_t * pe_Machine              (struct _pe * pe);
uint_t * pe_NumberOfSections     (struct _pe * pe);
uint_t * pe_TimeDateStamp        (struct _pe * pe);
uint_t * pe_PointerToSymbolTable (struct _pe * pe);
uint_t * pe_NumberOfSymbols      (struct _pe * pe);
uint_t * pe_SizeOfOptionalHeader (struct _pe * pe);
uint_t * pe_Characteristics      (struct _pe * pe);

char *        pe_string          (struct _pe * pe, int offset);
unsigned char pe_symbol_type     (struct _pe * pe, int index);

int pe_section (struct _pe * pe, struct _pe_section * section, int index);

uint_t * pe_section_VirtualSize          (struct _pe_section * section);
uint_t * pe_section_VirtualAddress       (struct _pe_section * section);
uint_t * pe_section_SizeOfRawData        (struct _pe_section * section);
uint_t * pe_section_PointerToRawData     (struct _pe_section * section);
uint_t * pe_section_PointerToRelocations (struct _pe_section * section);
uint_t * pe_section_PointerToLinenumbers (struct _pe_section * section);
uint_t * pe_section_NumberOfRelocations  (struct _pe_section * section);
uint_t * pe_section_NumberOfLinenumbers  (struct _pe_section * section);
uint_t * pe_section_Characteristics      (struct _pe_section * section);
char *   pe_section_Name                 (struct _pe_section * section);

int pe_symbol (struct _pe * pe, struct _pe_symbol * symbol, int index);

uint_t * pe_symbol_Value              (struct _pe_symbol * symbol);
uint_t * pe_symbol_SectionNumber      (struct _pe_symbol * symbol);
uint_t * pe_symbol_Type               (struct _pe_symbol * symbol);
uint_t * pe_symbol_StorageClass       (struct _pe_symbol * symbol);
uint_t * pe_symbol_NumberOfAuxSymbols (struct _pe_symbol * symbol);
char *   pe_symbol_Name               (struct _pe_symbol * symbol);

#endif
