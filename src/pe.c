#include "pe.h"

#define PE_ACCESSOR(MX) uint_t * pe_##MX (struct _pe * pe) { return &(pe->MX); }
#define PE_SECTION_ACCESSOR(MX) uint_t * pe_section_##MX \
    (struct _pe_section * section) { return &(section->MX); }
#define PE_SYMBOL_ACCESSOR(MX) uint_t * pe_symbol_##MX \
    (struct _pe_symbol * symbol) { return &(symbol->MX); }
#define PE_RELOCATION_ACCESSOR(MX) uint_t * pe_relocation_##MX \
    (struct _pe_relocation * relocation) { return &(relocation->MX); }

struct _pe * pe_open (char * filename)
{
    struct _pe * pe;
    Pe_Symbol symbol;
    unsigned int symbol_types_i;
    unsigned char aux_symbol_i;
    size_t bytes_read;
    int i;
    int file_offset;
    FILE * fh;
    
    fh = fopen(filename, "rb");
    if (fh == NULL) {
        fprintf(stderr, "falied to open file %s\n", filename);
        return NULL;
    }
    
    pe = (struct _pe *) malloc(sizeof(struct _pe));
    pe->bytes = NULL;
    pe->filename = NULL;
    pe->symbol_types = NULL;
    
    fseek(fh, 0, SEEK_END);
    pe->bytes_size = ftell(fh);
    fseek(fh, 0, SEEK_SET);
    
    pe->bytes = (unsigned char *) malloc(pe->bytes_size);
    
    bytes_read = fread(pe->bytes, 1, pe->bytes_size, fh);
    if (bytes_read != pe->bytes_size) {
        fprintf(stderr, "reading %s, filesize: %d but read %d bytes\n",
                filename, (int) pe->bytes_size, (int) bytes_read);
        pe_destroy(pe);
        return NULL;
    }
    
    fclose(fh);
    
    file_offset = *((int *) &(pe->bytes[PE_FILE_OFFSET]));
    // is this really a PE?
    for (i = 0; i < PE_SIGNATURE_SIZE; i++) {
        if (pe->bytes[file_offset + i] != PE_SIGNATURE[i]) {
            fprintf(stderr, "%s was not a valid PE\n", filename);
            pe_destroy(pe);
            return NULL;
        }
    }
    file_offset += PE_SIGNATURE_SIZE;
    
    pe->FileHeader_offset = file_offset;
    
    pe->filename = (char *) malloc(strlen(filename) + 1);
    strcpy(pe->filename, filename);
    
    pe->FileHeader = (Pe_FileHeader *) &(pe->bytes[file_offset]);
    
    uint_t_16_set(&(pe->Machine),           pe->FileHeader->Machine);
    uint_t_16_set(&(pe->NumberOfSections),  pe->FileHeader->NumberOfSections);
    uint_t_32_set(&(pe->TimeDateStamp),     pe->FileHeader->TimeDateStamp);
    uint_t_32_set(&(pe->PointerToSymbolTable),
                  pe->FileHeader->PointerToSymbolTable);
    uint_t_32_set(&(pe->NumberOfSymbols),   pe->FileHeader->NumberOfSymbols);
    uint_t_16_set(&(pe->SizeOfOptionalHeader),
                  pe->FileHeader->SizeOfOptionalHeader);
    uint_t_16_set(&(pe->Characteristics),   pe->FileHeader->Characteristics);
    
    // we're going to go through and mark symbols as regular or auxiliar now,
    // so that when we call them up by index later we can query this information
    // in O(1) time
    pe->symbol_types = (unsigned char *) malloc(sizeof(unsigned char) * 
                                            uint_t_get(pe_NumberOfSymbols(pe)));
    symbol_types_i = 0;
    while (symbol_types_i < uint_t_get(pe_NumberOfSymbols(pe))) {
        pe->symbol_types[symbol_types_i] = PE_SYMBOL_TYPE_REGULAR;
        memcpy(&symbol,
               &(pe->bytes[uint_t_get(pe_PointerToSymbolTable(pe)) + 
                           (PE_SYMBOL_SIZE * symbol_types_i)]),
               sizeof(Pe_Symbol));

        symbol_types_i++;
        aux_symbol_i = symbol.NumberOfAuxSymbols;
        
        while (aux_symbol_i--)
            pe->symbol_types[symbol_types_i++] = PE_SYMBOL_TYPE_AUXILIARY;
    }
    pe->string_table_offset = uint_t_get(pe_PointerToSymbolTable(pe)) +
                              (PE_SYMBOL_SIZE * 
                               uint_t_get(pe_NumberOfSymbols(pe)));
    
    return pe;
}


void pe_destroy (struct _pe * pe)
{
    if (pe->filename != NULL)
        free(pe->filename);
    if (pe->bytes != NULL)
        free(pe->bytes);
    if (pe->symbol_types != NULL)
        free(pe->symbol_types);
    free(pe);
}

PE_ACCESSOR(Machine)
PE_ACCESSOR(NumberOfSections)
PE_ACCESSOR(TimeDateStamp)
PE_ACCESSOR(PointerToSymbolTable)
PE_ACCESSOR(NumberOfSymbols)
PE_ACCESSOR(SizeOfOptionalHeader)
PE_ACCESSOR(Characteristics)

char * pe_string (struct _pe * pe, int offset)
{
    return (char *) &(pe->bytes[pe->string_table_offset + offset]);
}

unsigned char pe_symbol_type (struct _pe * pe, int index) {
    return pe->symbol_types[index];
}


int pe_section (struct _pe * pe, struct _pe_section * section, int index)
{
    int offset;
    
    if (uint_t_get(pe_NumberOfSections(pe)) <= index)
        return 0;
    
    section->index = index;
    section->pe = pe;
    memcpy(&(section->SectionHeader),
           &(pe->bytes[pe->FileHeader_offset + sizeof(Pe_FileHeader) +
                       uint_t_get(pe_SizeOfOptionalHeader(pe)) +
                       (sizeof(Pe_SectionHeader) * index)]),
           sizeof(Pe_SectionHeader));

    uint_t_32_set(&(section->VirtualSize),
                  section->SectionHeader.VirtualSize);
    uint_t_32_set(&(section->VirtualAddress),
                  section->SectionHeader.VirtualAddress);
    uint_t_32_set(&(section->SizeOfRawData),
                  section->SectionHeader.SizeOfRawData);
    uint_t_32_set(&(section->PointerToRawData),
                  section->SectionHeader.PointerToRawData);
    uint_t_32_set(&(section->PointerToRelocations),
                  section->SectionHeader.PointerToRelocations);
    uint_t_32_set(&(section->PointerToLinenumbers),
                  section->SectionHeader.PointerToLinenumbers);
    uint_t_16_set(&(section->NumberOfRelocations),
                  section->SectionHeader.NumberOfRelocations);
    uint_t_16_set(&(section->NumberOfLinenumbers),
                  section->SectionHeader.NumberOfLinenumbers);
    uint_t_32_set(&(section->Characteristics),
                  section->SectionHeader.Characteristics);
    
    if (section->SectionHeader.Name[0] == '/') {
        offset = atoi(&(section->SectionHeader.Name[1]));
        strncpy(section->Name, pe_string(pe, offset), PE_NAME_SIZE);
        section->Name[PE_NAME_SIZE - 1] = '\0';
    }
    else {
        strncpy(section->Name, section->SectionHeader.Name, 8);
        section->Name[8] = '\0';
    }
    return 1;
}

PE_SECTION_ACCESSOR(VirtualSize)
PE_SECTION_ACCESSOR(VirtualAddress)
PE_SECTION_ACCESSOR(SizeOfRawData)
PE_SECTION_ACCESSOR(PointerToRawData)
PE_SECTION_ACCESSOR(PointerToRelocations)
PE_SECTION_ACCESSOR(PointerToLinenumbers)
PE_SECTION_ACCESSOR(NumberOfRelocations)
PE_SECTION_ACCESSOR(Characteristics)
char * pe_section_Name (struct _pe_section * section) { return section->Name; }

unsigned char * pe_section_data (struct _pe_section * section)
{
    return &(section->pe->bytes[uint_t_get(pe_section_PointerToRawData(section))]);
}

// this doesn't handle aux symbols. fuck you microsoft you fucking cocksuckers.
// We can either make iterating through all symbols a n^2 type deal, or be
// incorrect, do it in n, and come up with a better solution later
// (probably stored an array of offsets to correct symbol locations created in
//  pe_open)
int pe_symbol (struct _pe * pe, struct _pe_symbol * symbol, int index)
{
    int offset;
    
    if (uint_t_get(pe_NumberOfSymbols(pe)) <= index)
        return 0;
    
    symbol->index = index;
    symbol->pe = pe;
    offset = (int) uint_t_get(pe_PointerToSymbolTable(pe));
    offset += PE_SYMBOL_SIZE * index;
    memcpy(&(symbol->Symbol), &(pe->bytes[offset]),
           sizeof(Pe_Symbol));

    uint_t_32_set(&(symbol->Value),
                  symbol->Symbol.Value);
    uint_t_16_set(&(symbol->SectionNumber),
                  symbol->Symbol.SectionNumber);
    uint_t_16_set(&(symbol->Type),
                  symbol->Symbol.Type);
    uint_t_8_set (&(symbol->StorageClass),
                  symbol->Symbol.StorageClass);
    uint_t_8_set (&(symbol->NumberOfAuxSymbols),
                  symbol->Symbol.NumberOfAuxSymbols);
    
    if (symbol->Symbol.Name[0] == '\0') {
        offset = *((int *) &(symbol->Symbol.Name[4]));
        strncpy(symbol->Name, pe_string(pe, offset), PE_NAME_SIZE);
        symbol->Name[PE_NAME_SIZE - 1] = '\0';
    }
    else {
        strncpy(symbol->Name, symbol->Symbol.Name, 8);
        symbol->Name[8] = '\0';
    }
    
    return 1;
}

PE_SYMBOL_ACCESSOR(Value)
PE_SYMBOL_ACCESSOR(SectionNumber)
PE_SYMBOL_ACCESSOR(Type)
PE_SYMBOL_ACCESSOR(StorageClass)
PE_SYMBOL_ACCESSOR(NumberOfAuxSymbols)
char * pe_symbol_Name (struct _pe_symbol * symbol) { return symbol->Name; }


int pe_section_relocation (struct _pe_section * section,
                           struct _pe_relocation * relocation,
                           int index)
{
    unsigned int offset;
    
    if (index >= uint_t_get(pe_section_NumberOfRelocations(section)))
        return 0;
    
    relocation->index = index;
    relocation->pe = section->pe;
    
    offset = uint_t_get(pe_section_PointerToRelocations(section));
    offset += PE_RELOCATION_SIZE * index;
    
    memcpy(&(relocation->Relocation), &(section->pe->bytes[offset]),
           sizeof(Pe_Relocation));
    
    uint_t_32_set(&(relocation->VirtualAddress),
                relocation->Relocation.VirtualAddress);
    uint_t_32_set(&(relocation->SymbolTableIndex),
                relocation->Relocation.SymbolTableIndex);
    uint_t_16_set(&(relocation->Type), relocation->Relocation.Type);
    
    return 1;
}

PE_RELOCATION_ACCESSOR(VirtualAddress)
PE_RELOCATION_ACCESSOR(SymbolTableIndex)
PE_RELOCATION_ACCESSOR(Type)
