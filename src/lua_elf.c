#include "lua_elf.h"

#define LUA_ELF_SECTION_T_ACCESSOR(push_function, value_get) \
    struct _elf_section * section; \
    section = lua_check_elf_section(L, 1); \
    lua_pop(L, 1); \
    push_function(L, value_get); \
    return 1;

#define LUA_ELF_SYMBOL_T_ACCESSOR(push_function, value_get) \
    struct _elf_symbol * symbol; \
    symbol = lua_check_elf_symbol(L, 1); \
    lua_pop(L, 1); \
    push_function(L, value_get); \
    return 1;

#define LUA_ELF_RELOCATION_T_ACCESSOR(push_function, value_get) \
    struct _elf_relocation * relocation; \
    relocation = lua_check_elf_relocation(L, 1); \
    lua_pop(L, 1); \
    push_function(L, value_get); \
    return 1;

static const struct luaL_Reg elf_lib_f [] = {
    {"new", lua_elf_t_new},
    {NULL, NULL}
};

static const struct luaL_Reg elf_lib_m [] = {
    {"class",          lua_elf_t_class},
    {"shnum",          lua_elf_t_shnum},
    {"filename",       lua_elf_t_filename},
    {"section",        lua_elf_t_section},
    {"section_exists", lua_elf_t_section_exists},
    {"symbols",        lua_elf_t_symbols},
    {"__gc",           lua_elf_t_gc},
    {NULL, NULL}
};

static const struct luaL_Reg elf_section_lib_f [] = {
    {NULL, NULL}
};

static const struct luaL_Reg elf_section_lib_m [] = {
    {"name",           lua_elf_section_t_name},
    {"address",        lua_elf_section_t_address},
    {"exec",           lua_elf_section_t_exec},
    {"size",           lua_elf_section_t_size},
    {"offset",         lua_elf_section_t_offset},
    {"type",           lua_elf_section_t_type},
    {"num",            lua_elf_section_t_num},
    {"entsize",        lua_elf_section_t_entsize},
    {"link",           lua_elf_section_t_link},
    {"symbol",         lua_elf_section_t_symbol},
    {"symbols",        lua_elf_section_t_symbols},
    {"relocation",     lua_elf_section_t_relocation},
    {"relocations",    lua_elf_section_t_relocations},
    {"disassemble",    lua_elf_section_t_disassemble},
    {"rop_table",      lua_elf_section_t_rop_table},
    {"mem_at_address", lua_elf_section_t_mem_at_address},
    {"__gc",           lua_elf_section_t_gc},
    {NULL, NULL}
};

static const struct luaL_Reg elf_symbol_lib_f [] = {
    {NULL, NULL}
};

static const struct luaL_Reg elf_symbol_lib_m [] = {
    {"name",        lua_elf_symbol_t_name},
    {"value",       lua_elf_symbol_t_value},
    {"type",        lua_elf_symbol_t_type},
    {"size",        lua_elf_symbol_t_size},
    {"disassemble", lua_elf_symbol_t_disassemble},
    {"__gc",        lua_elf_symbol_t_gc},
    {NULL, NULL}
};

static const struct luaL_Reg elf_relocation_lib_f [] = {
    {NULL, NULL}
};

static const struct luaL_Reg elf_relocation_lib_m [] = {
    {"name",   lua_elf_relocation_t_name},
    {"offset", lua_elf_relocation_t_offset},
    {"type",   lua_elf_relocation_t_type},
    {"__gc",   lua_elf_relocation_t_gc},
    {NULL, NULL}
};

void lua_elf_t_collect (struct lua_elf_t * elf)
{
    elf->ref_count--;
    
    if (elf->ref_count <= 0) {
        elf_destroy(elf->elf);
        free(elf);
    }
}


int lua_open_elf_t (lua_State * L)
{
    luaL_newmetatable(L, "rop_tools.elf_t");
    lua_pushstring(L, "__index");
    lua_pushvalue(L, -2);
    lua_settable(L, -3);
    luaL_register(L, NULL, elf_lib_m);
    luaL_register(L, "elf_t", elf_lib_f);
    
    luaL_newmetatable(L, "rop_tools.elf_section_t");
    lua_pushstring(L, "__index");
    lua_pushvalue(L, -2);
    lua_settable(L, -3);
    luaL_register(L, NULL, elf_section_lib_m);
    luaL_register(L, "elf_section_t", elf_section_lib_f);
    
    luaL_newmetatable(L, "rop_tools.elf_symbol_t");
    lua_pushstring(L, "__index");
    lua_pushvalue(L, -2);
    lua_settable(L, -3);
    luaL_register(L, NULL, elf_symbol_lib_m);
    luaL_register(L, "elf_symbol_t", elf_symbol_lib_f);
    
    luaL_newmetatable(L, "rop_tools.elf_relocation_t");
    lua_pushstring(L, "__index");
    lua_pushvalue(L, -2);
    lua_settable(L, -3);
    luaL_register(L, NULL, elf_relocation_lib_m);
    luaL_register(L, "elf_relocation_t", elf_relocation_lib_f);
    
    return 1;
}


/********************************
*              ELF              *
********************************/

struct _elf * lua_check_elf (lua_State * L, int position)
{
    struct lua_elf_t ** elf;
    void * userdata = luaL_checkudata(L, position, "rop_tools.elf_t");
    luaL_argcheck(L, userdata != NULL, position, "elf_t expected");
    elf = (struct lua_elf_t **) userdata;
    return (struct _elf *) (*elf)->elf;
}


struct lua_elf_t * lua_check_elf_t (lua_State * L, int position)
{
    struct lua_elf_t ** elf;
    void * userdata = luaL_checkudata(L, position, "rop_tools.elf_t");
    luaL_argcheck(L, userdata != NULL, position, "elf_t expected");
    elf = (struct lua_elf_t **) userdata;
    return *elf;
}


void lua_push_elf  (lua_State * L, struct _elf * e)
{
    struct lua_elf_t ** new_elf;
    
    new_elf = lua_newuserdata(L, sizeof(struct lua_elf_t *));
    luaL_getmetatable(L, "rop_tools.elf_t");
    lua_setmetatable(L, -2);
    
    (*new_elf)->elf = NULL;
    elf_copy((*new_elf)->elf, e);
}


int lua_elf_t_new (lua_State * L)
{
    char * filename;
    struct lua_elf_t ** new_elf;
    
    filename = (char *) luaL_checkstring(L, 1);
    lua_pop(L, 1);
    
    new_elf = lua_newuserdata(L, sizeof(struct lua_elf_t **));
    luaL_getmetatable(L, "rop_tools.elf_t");
    lua_setmetatable(L, -2);
    
    *new_elf = (struct lua_elf_t *) malloc(sizeof(struct lua_elf_t));
    (*new_elf)->ref_count = 1;
    
    (*new_elf)->elf = elf_open(filename);
    if ((*new_elf)->elf == NULL)
        luaL_error(L, "error opening elf file %s", filename);
    
    return 1;
}


int lua_elf_t_gc (lua_State * L) 
{
    struct lua_elf_t ** elf;
    
    void * userdata = luaL_checkudata(L, 1, "rop_tools.elf_t");
    luaL_argcheck(L, userdata != NULL, 1, "elf_t expected");
    elf = (struct lua_elf_t **) userdata;
    
    lua_elf_t_collect(*elf);
    
    return 0;
}


int lua_elf_t_class (lua_State * L)
{
    struct _elf * elf;
    
    elf = lua_check_elf(L, 1);
    lua_pop(L, 1);
    
    if (elf_class(elf) == ELFCLASS32)
        lua_pushinteger(L, 32);
    else if (elf_class(elf) == ELFCLASS64)
        lua_pushinteger(L, 64);
    return 1;
}


int lua_elf_t_filename (lua_State * L)
{
    struct _elf * elf;
    
    elf = lua_check_elf(L, 1);
    lua_pop(L, 1);
    
    lua_pushstring(L, elf_filename(elf));
    
    return 1;
}


int lua_elf_t_shnum (lua_State * L)
{
    struct _elf * elf;
    
    elf = lua_check_elf(L, 1);
    lua_pop(L, 1);
    
    lua_push_int_t(L, elf_shnum(elf));
    return 1;
}


int lua_elf_t_section (lua_State * L)
{
    struct lua_elf_t * elf;
    struct _elf_section section;
    int section_i = -1;
    int found;
    struct lua_elf_section_t * section_t;
    
    elf = lua_check_elf_t(L, 1);
    if (lua_isnumber(L, 2))
        section_i = luaL_checkinteger(L, 2);
    else if (lua_isstring(L, 2)) {
        found = 0;
        for (section_i = 0; section_i < int_t_get(elf_shnum(elf->elf));
             section_i++) {
            elf_section(elf->elf, &section, section_i);
            if (strcmp(elf_section_name(&section), lua_tostring(L, 2)) == 0) {
                found = 1;
                break;
            }
        }
        if (! found)
            luaL_error(L, "no section found by name %s", lua_tostring(L, 2));
    }
    else
        luaL_error(L, "expected a string or number");
    lua_pop(L, 2);
    
    if (section_i >= int_t_get(elf_shnum(elf->elf)))
        luaL_error(L, "elf does not have %d sections", section_i);
    
    lua_push_elf_section_t(L);
    section_t = lua_check_elf_section_t(L, 1);
    elf_section(elf->elf, &(section_t->section), section_i);
    section_t->elf_t = elf;
    elf->ref_count++;
    
    return 1;
}


int lua_elf_t_section_exists (lua_State * L)
{
    struct _elf * elf;
    struct _elf_section section;
    int section_i;
    int found;
    char * name;
    
    elf = lua_check_elf(L, 1);
    name = (char *) luaL_checkstring(L, 2);
    lua_pop(L, 2);
    
    found = 0;
    for (section_i = 0; section_i < int_t_get(elf_shnum(elf)); section_i++) {
        elf_section(elf, &section, section_i);
        if (strcmp(elf_section_name(&section), name) == 0) {
            found = 1;
            break;
        }
    }
    
    if (found)
        lua_pushboolean(L, 1);
    else
        lua_pushboolean(L, 0);
    
    return 1;
}



int lua_elf_t_symbols (lua_State * L)
{
    struct lua_elf_t * elf_t;
    struct lua_elf_section_t * section_t;
    struct _elf_section section;
    struct _elf * elf;
    int section_i;
    int sym_i;
    int symbols_i = 1;
    struct lua_elf_symbol_t * symbol_t;
    
    elf_t = lua_check_elf_t(L, 1);
    lua_pop(L, 1);
    
    elf = elf_t->elf;
    
    lua_newtable(L);
    for (section_i = 0; section_i < int_t_get(elf_shnum(elf)); section_i++) {
        elf_section(elf, &section, section_i);
        if (    (int_t_get(elf_section_type(&section)) == SHT_SYMTAB)
             || (int_t_get(elf_section_type(&section)) == SHT_DYNSYM)) {
             
            lua_push_elf_section_t(L);
            section_t = lua_check_elf_section_t(L, -1);
            lua_pop(L, 1);
            elf_section(elf, &(section_t->section), section_i);
            section_t->elf_t = elf_t;
            elf_t->ref_count++;
            
            for (sym_i = 0; sym_i < elf_section_num(&section); sym_i++) {
                lua_pushinteger(L, (lua_Integer) symbols_i++);
                lua_push_elf_symbol_t(L);
                symbol_t = lua_check_elf_symbol_t(L, -1);
                elf_section_symbol(&section, &(symbol_t->symbol), sym_i);
                symbol_t->section_t = section_t;
                section_t->ref_count++;
                elf_t->ref_count++;
                lua_settable(L, -3);
            }
            
        }
    }
    
    return 1;
}



/********************************
*            SECTION            *
********************************/

void lua_elf_section_t_collect (struct lua_elf_section_t * section)
{
    lua_elf_t_collect(section->elf_t);

    section->ref_count--;
    
    if (section->ref_count <= 0)
        free(section);
}


struct lua_elf_section_t * lua_check_elf_section_t (lua_State * L, int position)
{
    struct lua_elf_section_t ** section_t;
    void * userdata = luaL_checkudata(L, position, "rop_tools.elf_section_t");
    luaL_argcheck(L, userdata != NULL, position, "elf_section_t expected");
    section_t = (struct lua_elf_section_t **) userdata;
    return *section_t;
}


struct _elf_section * lua_check_elf_section (lua_State * L, int position)
{
    struct lua_elf_section_t * section_t;
    
    section_t = lua_check_elf_section_t(L, position);
    return (struct _elf_section *) &(section_t->section);
}


void lua_push_elf_section_t  (lua_State * L)
{
    struct lua_elf_section_t ** new_section;
    
    new_section = lua_newuserdata(L, sizeof(struct lua_elf_section_t *));
    luaL_getmetatable(L, "rop_tools.elf_section_t");
    lua_setmetatable(L, -2);
    
    *new_section = (struct lua_elf_section_t *)
                   malloc(sizeof(struct lua_elf_section_t));
    (*new_section)->ref_count = 1;
}


int lua_elf_section_t_gc (lua_State * L) 
{
    struct lua_elf_section_t ** section;
    
    void * userdata = luaL_checkudata(L, 1, "rop_tools.elf_section_t");
    luaL_argcheck(L, userdata != NULL, 1, "elf_section_t expected");
    section = (struct lua_elf_section_t **) userdata;

    lua_elf_section_t_collect(*section);
    
    return 0;
}


int lua_elf_section_t_name (lua_State * L)
{
    LUA_ELF_SECTION_T_ACCESSOR(lua_pushstring, elf_section_name(section))
}


int lua_elf_section_t_address (lua_State * L)
{
    LUA_ELF_SECTION_T_ACCESSOR(lua_push_uint_t, elf_section_addr(section))
}


int lua_elf_section_t_exec (lua_State * L)
{
    LUA_ELF_SECTION_T_ACCESSOR(lua_pushboolean, elf_section_exec(section))
}


int lua_elf_section_t_size (lua_State * L)
{
    LUA_ELF_SECTION_T_ACCESSOR(lua_push_int_t, elf_section_size(section))
}


int lua_elf_section_t_offset (lua_State * L)
{
    LUA_ELF_SECTION_T_ACCESSOR(lua_push_uint_t, elf_section_offset(section))
}


int lua_elf_section_t_type (lua_State * L)
{
    LUA_ELF_SECTION_T_ACCESSOR(lua_pushstring,
        elf_section_type_strings[int_t_get(elf_section_type(section))])
}


int lua_elf_section_t_num (lua_State * L)
{
    LUA_ELF_SECTION_T_ACCESSOR(lua_pushinteger, elf_section_num(section))
}


int lua_elf_section_t_entsize (lua_State * L)
{
    LUA_ELF_SECTION_T_ACCESSOR(lua_push_int_t, elf_section_entsize(section))
}


int lua_elf_section_t_link (lua_State * L)
{
    LUA_ELF_SECTION_T_ACCESSOR(lua_push_int_t, elf_section_link(section))
}


int lua_elf_section_t_symbol (lua_State * L)
{
    struct lua_elf_section_t * section_t;
    struct _elf_symbol symbol;
    int symbol_i = 0;
    int found;
    struct lua_elf_symbol_t * symbol_t;
    
    section_t = lua_check_elf_section_t(L, 1);
    if (    (int_t_get(elf_section_type(&(section_t->section))) != SHT_SYMTAB)
         && (int_t_get(elf_section_type(&(section_t->section))) != SHT_DYNSYM))
        luaL_error(L, "can only call symbol() on section_t of type (symtab|dynsym)");
    if (lua_isnumber(L, 2))
        symbol_i = luaL_checkinteger(L, 2);
    else if (lua_isstring(L, 2)) {
        found = 0;
        for (symbol_i = 0; symbol_i < elf_section_num(&(section_t->section));
             symbol_i++) {
            elf_section_symbol(&(section_t->section), &symbol, symbol_i);
            if (strcmp(elf_symbol_name(&symbol), lua_tostring(L, 2)) == 0) {
                found = 1;
                break;
            }
        }
        if (! found)
            luaL_error(L, "no symbol found by name %s", lua_tostring(L, 2));
    }
    else
        luaL_error(L, "expected a string or number");
    lua_pop(L, 2);
    
    lua_push_elf_symbol_t(L);
    symbol_t = lua_check_elf_symbol_t(L, 1);
    elf_section_symbol(&(section_t->section), &(symbol_t->symbol), symbol_i);
    symbol_t->section_t = section_t;
    section_t->elf_t->ref_count++;
    section_t->ref_count++;
    
    return 1;
}


int lua_elf_section_t_symbols (lua_State * L)
{
    struct lua_elf_section_t * section_t;
    int symbol_i;
    struct lua_elf_symbol_t * symbol_t;
    
    section_t = lua_check_elf_section_t(L, 1);
    if (    (int_t_get(elf_section_type(&(section_t->section))) != SHT_SYMTAB)
         && (int_t_get(elf_section_type(&(section_t->section))) != SHT_DYNSYM))
        luaL_error(L, "can only call symbols() on section_t of type (symtab|dynsym)");
    lua_pop(L, 1);
    
    lua_newtable(L);
    for (symbol_i = 0; symbol_i < elf_section_num(&(section_t->section));
         symbol_i++) {
        lua_push_elf_symbol_t(L);
        symbol_t = lua_check_elf_symbol_t(L, -1);
        elf_section_symbol(&(section_t->section), &(symbol_t->symbol), symbol_i);
        symbol_t->section_t = section_t;
        section_t->elf_t->ref_count++;
        section_t->ref_count++;
        lua_pushinteger(L, symbol_i + 1);
        lua_insert(L, -2);
        lua_settable(L, -3);
    }
    
    return 1;
}


int lua_elf_section_t_relocation (lua_State * L)
{
    struct lua_elf_section_t * section_t;
    struct _elf_relocation relocation;
    int relocation_i = 0;
    int found;
    struct lua_elf_relocation_t * relocation_t;
    
    section_t = lua_check_elf_section_t(L, 1);
    if (    (int_t_get(elf_section_type(&(section_t->section))) != SHT_REL)
         && (int_t_get(elf_section_type(&(section_t->section))) != SHT_RELA))
        luaL_error(L, "can only call relocation() on section_t of type (rel|rela)");
        
    if (lua_isnumber(L, 2))
        relocation_i = luaL_checkinteger(L, 2);
    else if (lua_isstring(L, 2)) {
        found = 0;
        for (relocation_i = 0;
             relocation_i < elf_section_num(&(section_t->section));
             relocation_i++) {
            elf_section_relocation(&(section_t->section), &relocation,
                                   relocation_i);
            if (strcmp(elf_relocation_name(&relocation),
                       lua_tostring(L, 2)) == 0) {
                found = 1;
                break;
            }
        }
        if (! found)
            luaL_error(L, "no symbol found by name %s", lua_tostring(L, 2));
    }
    else
        luaL_error(L, "expected a string or number");
    lua_pop(L, 2);
    
    lua_push_elf_relocation_t(L);
    relocation_t = lua_check_elf_relocation_t(L, 1);
    elf_section_relocation(&(section_t->section), &(relocation_t->relocation),
                           relocation_i);
    relocation_t->section_t = section_t;
    section_t->elf_t->ref_count++;
    section_t->ref_count++;
    
    return 1;
}


int lua_elf_section_t_relocations (lua_State * L)
{
    struct lua_elf_section_t * section_t;
    int relocation_i;
    struct lua_elf_relocation_t * relocation_t;
    
    section_t = lua_check_elf_section_t(L, 1);
    if (    (int_t_get(elf_section_type(&(section_t->section))) != SHT_REL)
         && (int_t_get(elf_section_type(&(section_t->section))) != SHT_RELA))
        luaL_error(L, "can only call relocation() on section_t of type (rel|rela)");
    lua_pop(L, 1);
    
    lua_newtable(L);
    for (relocation_i = 0;
         relocation_i < elf_section_num(&(section_t->section));
         relocation_i++) {
        lua_push_elf_relocation_t(L);
        relocation_t = lua_check_elf_relocation_t(L, -1);
        elf_section_relocation(&(section_t->section),
                               &(relocation_t->relocation), relocation_i);
        relocation_t->section_t = section_t;
        section_t->elf_t->ref_count++;
        section_t->ref_count++;
        lua_pushinteger(L, relocation_i + 1);
        lua_insert(L, -2);
        lua_settable(L, -3);
    }
    
    return 1;
}


int lua_elf_section_t_disassemble (lua_State * L)
{
    struct _elf_section * section;
    uint_t * address;
    uint_t   address_tmp;
    unsigned char * data;
    int disassemble_address = 0;
    
    section = lua_check_elf_section(L, 1);
    if (lua_isuserdata(L, 2)) {
        address = lua_check_uint_t(L, 2);
        disassemble_address = 1;
        lua_pop(L, 2);
    }
    else
        lua_pop(L, 1);
    
    if (disassemble_address) {
        uint_t_set(&address_tmp, address);
        uint_t_sub(&address_tmp, elf_section_addr(section));
        data = elf_section_data(section);
        lua_dis_instruction(L, &(data[uint_t_get(&address_tmp)]),
                            ELF_CLASS(section->elf));
    }
    else
        lua_dis_table(L,
                      elf_section_addr(section),
                      elf_section_data(section),
                      int_t_get(elf_section_size(section)), 
                      ELF_CLASS(section->elf));

    return 1;
}


int lua_elf_section_t_rop_table (lua_State * L)
{
    struct _elf_section * section;
    struct _rop_list * rop_list;
    struct _rop_list * rop_list_first;
    struct _rop_ins  * rop_ins;
    int                rop_depth;
    int                rop_i;
    uint_t             address;
    
    section = lua_check_elf_section(L, 1);
    rop_depth = luaL_checkinteger(L, 2);
    lua_pop(L, 2);
    
    rop_list_first = rop_ret_rops(elf_section_data(section),
                                  int_t_get(elf_section_size(section)),
                                  rop_depth,
                                  ELF_CLASS(section->elf));
    rop_list = rop_list_first;
    
    rop_i = 1;
    lua_newtable(L);
    while (rop_list != NULL) {
        rop_ins = rop_list_ins(rop_list);
        uint_t_set(&address, elf_section_addr(section));
        uint_t_add_int(&address, rop_list->offset);
        lua_dis_table(L, &address, rop_list->bytes,
                      rop_list->bytes_size, ELF_CLASS(section->elf));
        lua_pushinteger(L, (lua_Integer) rop_i);
        lua_insert(L, -2);
        lua_settable(L, -3);
        rop_i++;
        rop_list = rop_list->next;
    }
    
    rop_list_destroy(rop_list_first);
    
    return 1;
}


int lua_elf_section_t_mem_at_address (lua_State * L)
{
    struct _elf_section * section;
    uint_t * address;
    uint_t   address_tmp;
    int      member_number;
    int      member_size;
    int      i;
    uint_t   uintt;
    unsigned char * data;
    int offset;
    
    section = lua_check_elf_section(L, 1);
    address = lua_check_uint_t(L, 2);
    member_number = luaL_checkinteger(L, 3);
    member_size = luaL_checkinteger(L, 4);
    lua_pop(L, 4);
    
    data = elf_section_data(section);
    uint_t_set(&address_tmp, address);
    uint_t_sub(&address_tmp, elf_section_addr(section));
    offset = uint_t_get(&address_tmp);
    uint_t_set(&address_tmp, address);
    lua_newtable(L);
    for (i = 0; i < member_number; i++) {
        switch (member_size) {
        case 8 :
            lua_pushinteger(L, (lua_Integer) i + 1);
            uint_t_8_set(&uintt, *((uint8_t *) &(data[offset + i])));
            lua_push_uint_t(L, &uintt);
            lua_settable(L, -3);
            uint_t_add_int(&address_tmp, 1);
            break;
        case 16 :
            lua_pushinteger(L, (lua_Integer) i + 1);
            uint_t_16_set(&uintt, *((uint16_t *) &(data[offset + i*2])));
            lua_push_uint_t(L, &uintt);
            lua_settable(L, -3);
            uint_t_add_int(&address_tmp, 2);
            break;
        case 32 :
            lua_pushinteger(L, (lua_Integer) i + 1);
            uint_t_32_set(&uintt, *((uint32_t *) &(data[offset + i*4])));
            lua_push_uint_t(L, &uintt);
            lua_settable(L, -3);
            uint_t_add_int(&address_tmp, 4);
            break;
        case 64 :
            lua_pushinteger(L, (lua_Integer) i + 1);
            uint_t_64_set(&uintt, *((uint64_t *) &(data[offset + i*8])));
            lua_push_uint_t(L, &uintt);
            lua_settable(L, -3);
            uint_t_add_int(&address_tmp, 8);
            break;
        }
    }
    
    return 1;
}


/********************************
*            SYMBOL             *
********************************/

void lua_elf_symbol_t_collect (struct lua_elf_symbol_t * symbol_t)
{
    lua_elf_section_t_collect(symbol_t->section_t);

    symbol_t->ref_count--;
    
    if (symbol_t->ref_count <= 0)
        free(symbol_t);
}


struct lua_elf_symbol_t * lua_check_elf_symbol_t (lua_State * L, int position)
{
    struct lua_elf_symbol_t ** symbol_t;
    void * userdata = luaL_checkudata(L, position, "rop_tools.elf_symbol_t");
    luaL_argcheck(L, userdata != NULL, position, "elf_symbol_t expected");
    symbol_t = (struct lua_elf_symbol_t **) userdata;
    return *symbol_t;
}


struct _elf_symbol * lua_check_elf_symbol (lua_State * L, int position)
{
    struct lua_elf_symbol_t * symbol_t;
    symbol_t = lua_check_elf_symbol_t(L, position);
    return (struct _elf_symbol *) &(symbol_t->symbol);
}


void lua_push_elf_symbol_t  (lua_State * L)
{
    struct lua_elf_symbol_t ** new_symbol;
    
    new_symbol = lua_newuserdata(L, sizeof(struct lua_elf_symbol_t **));
    luaL_getmetatable(L, "rop_tools.elf_symbol_t");
    lua_setmetatable(L, -2);
    
    *new_symbol = (struct lua_elf_symbol_t *)
                  malloc(sizeof(struct lua_elf_symbol_t));
    (*new_symbol)->ref_count = 1;
}


int lua_elf_symbol_t_gc (lua_State * L)
{
    struct lua_elf_symbol_t * symbol_t;
    
    symbol_t = lua_check_elf_symbol_t(L, 1);
    lua_elf_symbol_t_collect(symbol_t);
    
    return 0;
}


int lua_elf_symbol_t_name (lua_State * L)
{
    LUA_ELF_SYMBOL_T_ACCESSOR(lua_pushstring, elf_symbol_name(symbol))
}


int lua_elf_symbol_t_value (lua_State * L)
{
    LUA_ELF_SYMBOL_T_ACCESSOR(lua_push_uint_t, elf_symbol_value(symbol))
}


int lua_elf_symbol_t_type (lua_State * L)
{
    LUA_ELF_SYMBOL_T_ACCESSOR(lua_pushstring, \
        elf_symbol_type_strings[elf_symbol_type(symbol)])
}


int lua_elf_symbol_t_size (lua_State * L)
{
    LUA_ELF_SYMBOL_T_ACCESSOR(lua_push_int_t, elf_symbol_size(symbol))
}


int lua_elf_symbol_t_disassemble (lua_State * L)
{
    struct lua_elf_symbol_t * symbol_t;
    struct _elf * elf;
    struct _elf_symbol symbol;
    struct _elf_section section;
    unsigned char * data;
    int section_i;
    int symbol_i;
    uint_t offset;
    
    int end_address_set = 0;
    uint_t end_address;
    uint_t_8_set(&end_address, 0);
    
    symbol_t = lua_check_elf_symbol_t(L, 1);
    lua_pop(L, 1);
    
    elf = symbol_t->section_t->elf_t->elf;
    // find function sym at next highest address, save it's start address in
    // end_address
    for (section_i = 0; section_i < int_t_get(elf_shnum(elf)); section_i++) {
        elf_section(elf, &section, section_i);
        if (int_t_get(elf_section_type(&section)) == SHT_SYMTAB) {
            for (symbol_i = 0; symbol_i < elf_section_num(&section); symbol_i++) {
                elf_section_symbol(&section, &symbol, symbol_i);
                if (    (elf_symbol_type(&symbol) == STT_FUNC)
                     && (uint_t_cmp(elf_symbol_value(&(symbol_t->symbol)),
                                    elf_symbol_value(&symbol)) < 0)
                     && (uint_t_cmp(elf_symbol_shndx(&symbol),
                                    elf_symbol_shndx(&(symbol_t->symbol)))
                         == 0)
                     && (    (uint_t_cmp(elf_symbol_value(&symbol),
                                         &end_address) < 0)
                          || (end_address_set == 0)
                        )
                   ) {
                    uint_t_set(&end_address, elf_symbol_value(&symbol));
                    end_address_set = 1;
                }
            }
        }
    }
    
    // end address not set? set it to the end of the linked section
    if (end_address_set == 0) {
        elf_section(elf, &section, 
                    uint_t_get(elf_symbol_shndx(&(symbol_t->symbol))));
        uint_t_int_t(&end_address, elf_section_size(&section));
        uint_t_add(&end_address, elf_section_addr(&section));
    }
    
    
    // load the linked section
    elf_section(elf, &section, uint_t_get(elf_symbol_shndx(&(symbol_t->symbol))));
    data = elf_section_data(&section);
    uint_t_set(&offset, elf_symbol_value(&(symbol_t->symbol)));
    uint_t_sub(&offset, elf_section_addr(&section));
    
    // disassemble that function like it was 1980
    lua_dis_table(L,
                  elf_symbol_value(&(symbol_t->symbol)),
                  &(data[uint_t_get(&offset)]),
                  uint_t_get(&end_address)
                  - uint_t_get(elf_symbol_value(&(symbol_t->symbol))),
                  ELF_CLASS(elf));
                  
    return 1;
}



/********************************
*          RELOCATION           *
********************************/

void lua_elf_relocation_t_collect (struct lua_elf_relocation_t * relocation_t)
{
    lua_elf_section_t_collect(relocation_t->section_t);

    relocation_t->ref_count--;
    
    if (relocation_t->ref_count <= 0)
        free(relocation_t);
}


struct lua_elf_relocation_t * lua_check_elf_relocation_t (lua_State * L,
                                                          int position)
{
    struct lua_elf_relocation_t ** relocation_t;
    void * userdata = luaL_checkudata(L, position, "rop_tools.elf_relocation_t");
    luaL_argcheck(L, userdata != NULL, position, "elf_relocation_t expected");
    relocation_t = (struct lua_elf_relocation_t **) userdata;
    return *relocation_t;
}


struct _elf_relocation * lua_check_elf_relocation (lua_State * L, int position)
{
    struct lua_elf_relocation_t * relocation_t;
    relocation_t = lua_check_elf_relocation_t(L, 1);
    return (struct _elf_relocation *) &(relocation_t->relocation);
}


void lua_push_elf_relocation_t  (lua_State * L)
{
    struct lua_elf_relocation_t ** new_relocation;
    
    new_relocation = lua_newuserdata(L, sizeof(struct lua_elf_relocation_t **));
    luaL_getmetatable(L, "rop_tools.elf_relocation_t");
    lua_setmetatable(L, -2);
    
    *new_relocation = (struct lua_elf_relocation_t *)
                       malloc(sizeof(struct lua_elf_relocation_t));
    (*new_relocation)->ref_count = 1;
}


int lua_elf_relocation_t_gc (lua_State * L)
{
    struct lua_elf_relocation_t * relocation_t;
    relocation_t = lua_check_elf_relocation_t(L, 1);
    lua_elf_relocation_t_collect(relocation_t);
    
    return 0;
}


int lua_elf_relocation_t_name (lua_State * L)
{
    LUA_ELF_RELOCATION_T_ACCESSOR(lua_pushstring, elf_relocation_name(relocation))
}

int lua_elf_relocation_t_offset (lua_State * L)
{
    LUA_ELF_RELOCATION_T_ACCESSOR(lua_push_uint_t, elf_relocation_offset(relocation))
}

int lua_elf_relocation_t_type (lua_State * L)
{
    LUA_ELF_RELOCATION_T_ACCESSOR(lua_pushstring, 
                   elf_relocation_type_strings[elf_relocation_type(relocation)])
}
