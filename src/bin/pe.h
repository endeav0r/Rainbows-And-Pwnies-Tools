#ifndef pe_HEADER
#define pe_HEADER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../types.h"
#include "pe_spec.h"

#define PE_NAME_SIZE 64
#define PE_SYMBOL_TYPE_REGULAR   1
#define PE_SYMBOL_TYPE_AUXILIARY 2

struct _pe {
    char * filename;
    size_t bytes_size;
    unsigned char * bytes;
    
    Pe_FileHeader * FileHeader;
    union {
        Pe_OptionalHeaderStandard * OptionalHeaderStandard;
        Pe_OptionalHeaderStandardPlus * OptionalHeaderStandardPlus;
    } ohs;
    union {
        Pe_OptionalHeaderWindows * OptionalHeaderWindows;
        Pe_OptionalHeaderWindowsPlus * OptionalHeaderWindowsPlus;
    } ohw;
    
    int FileHeader_offset;
    int total_symbols;
    unsigned char * symbol_types;
    int string_table_offset;
    
    // from the regular FileHeader
    uint_t Machine;
    uint_t NumberOfSections;
    uint_t TimeDateStamp;
    uint_t PointerToSymbolTable;
    uint_t NumberOfSymbols;
    uint_t SizeOfOptionalHeader;
    uint_t Characteristics;
    // from optional standard header
    uint_t Magic;
    uint_t MajorLinkerVersion;
    uint_t MinorLinkerVersion;
    uint_t SizeOfCode;
    uint_t SizeOfInitializedData;
    uint_t SizeOfUninitializedData;
    uint_t AddressOfEntryPoint;
    uint_t BaseOfCode;
    uint_t BaseOfData;
    // from optional windows header (wtf is up with all these optional headers)
    uint_t ImageBase;
    uint_t SectionAlignment;
    uint_t FileAlignment;
    uint_t MajorOperatingSystemVersion;
    uint_t MinorOperatingSystemVersion;
    uint_t MajorImageVersion;
    uint_t MinorImageVersion;
    uint_t MajorSubsystemVersion;
    uint_t MinorSubsystemVersion;
    uint_t Win32VersionValue;
    uint_t SizeOfImage;
    uint_t SizeOfHeaders;
    uint_t CheckSum;
    uint_t Subsystem;
    uint_t DllCharacteristics;
    uint_t SizeOfStackReserve;
    uint_t SizeOfStackCommit;
    uint_t SizeOfHeapReserve;
    uint_t SizeOfHeapCommit;
    uint_t LoaderFlags;
    uint_t NumberOfRvaAndSizes;
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
    uint_t address;
};

struct _pe_symbol {
    int index;
    struct _pe * pe;
    Pe_Symbol Symbol;
    Pe_SymbolFunctionDefinition SymbolFunctionDefinition;
    char Name[PE_NAME_SIZE];
    uint_t Value;
    uint_t SectionNumber;
    uint_t Type;
    uint_t StorageClass;
    uint_t NumberOfAuxSymbols;
    // from Pe_SymbolFunctionDefinition;
    uint_t TagIndex;
    uint_t TotalSize;
    uint_t PointerToLinenumber;
    uint_t PointerToNextFunction;
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
int          pe_copy    (struct _pe * dst, struct _pe * src);

uint_t * pe_Machine                 (struct _pe * pe);
uint_t * pe_NumberOfSections        (struct _pe * pe);
uint_t * pe_TimeDateStamp           (struct _pe * pe);
uint_t * pe_PointerToSymbolTable    (struct _pe * pe);
uint_t * pe_NumberOfSymbols         (struct _pe * pe);
uint_t * pe_SizeOfOptionalHeader    (struct _pe * pe);
uint_t * pe_Characteristics         (struct _pe * pe);
uint_t * pe_Magic                   (struct _pe * pe);
uint_t * pe_MajorLinkerVersion      (struct _pe * pe);
uint_t * pe_MinorLinkerVersion      (struct _pe * pe);
uint_t * pe_SizeOfCode              (struct _pe * pe);
uint_t * pe_SizeOfInitializedData   (struct _pe * pe);
uint_t * pe_SizeOfUninitializedData (struct _pe * pe);
uint_t * pe_AddressOfEntryPoint     (struct _pe * pe);
uint_t * pe_BaseOfCode              (struct _pe * pe);
uint_t * pe_BaseOfData              (struct _pe * pe);
uint_t * pe_ImageBase               (struct _pe * pe);
uint_t * pe_SectionAlignment        (struct _pe * pe);
uint_t * pe_FileAlignment           (struct _pe * pe);

char *        pe_string          (struct _pe * pe, int offset);
unsigned char pe_symbol_type     (struct _pe * pe, int index);
int           pe_total_symbols   (struct _pe * pe);

int pe_section    (struct _pe * pe, struct _pe_section * section, int index);

uint_t *        pe_section_VirtualSize          (struct _pe_section * section);
uint_t *        pe_section_VirtualAddress       (struct _pe_section * section);
uint_t *        pe_section_SizeOfRawData        (struct _pe_section * section);
uint_t *        pe_section_PointerToRawData     (struct _pe_section * section);
uint_t *        pe_section_PointerToRelocations (struct _pe_section * section);
uint_t *        pe_section_PointerToLinenumbers (struct _pe_section * section);
uint_t *        pe_section_NumberOfRelocations  (struct _pe_section * section);
uint_t *        pe_section_NumberOfLinenumbers  (struct _pe_section * section);
uint_t *        pe_section_Characteristics      (struct _pe_section * section);
char *          pe_section_Name                 (struct _pe_section * section);
unsigned char * pe_section_data                 (struct _pe_section * section);
uint_t *        pe_section_address              (struct _pe_section * section);

int pe_symbol (struct _pe * pe, struct _pe_symbol * symbol, int index);

uint_t * pe_symbol_Value              (struct _pe_symbol * symbol);
uint_t * pe_symbol_SectionNumber      (struct _pe_symbol * symbol);
uint_t * pe_symbol_Type               (struct _pe_symbol * symbol);
uint_t * pe_symbol_StorageClass       (struct _pe_symbol * symbol);
uint_t * pe_symbol_NumberOfAuxSymbols (struct _pe_symbol * symbol);
uint_t * pe_symbol_TotalSize          (struct _pe_symbol * symbol);
char *   pe_symbol_Name               (struct _pe_symbol * symbol);

int pe_section_relocation (struct _pe_section * section,
                           struct _pe_relocation * relocation,
                           int index);

uint_t * pe_relocation_VirtualAddress   (struct _pe_relocation * relocation);
uint_t * pe_relocation_SymbolTableIndex (struct _pe_relocation * relocation);
uint_t * pe_relocation_Type             (struct _pe_relocation * relocation);

#endif
