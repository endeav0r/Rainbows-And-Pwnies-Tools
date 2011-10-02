#include "lua_elf.h"

/********************************
*              ELF              *
********************************/

#define LUA_SECTION_T_ACCESSOR(push_function, value_get) \
    struct _elf_shdr * shdr; \
    shdr = lua_check_shdr(L, 1); \
    lua_pop(L, 1); \
    push_function(L, value_get); \
    return 1;

#define LUA_SYMBOL_T_ACCESSOR(push_function, value_get) \
    struct _elf_sym * sym; \
    sym = lua_check_sym(L, 1); \
    lua_pop(L, 1); \
    push_function(L, value_get); \
    return 1;

#define LUA_RELOCATION_T_ACCESSOR(push_function, value_get) \
    struct _elf_rel * rel; \
    rel = lua_check_rel(L, 1); \
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
    {"__gc",           lua_elf_t_gc},
    {NULL, NULL}
};

static const struct luaL_Reg section_lib_f [] = {
    {NULL, NULL}
};

static const struct luaL_Reg section_lib_m [] = {
    {"name",        lua_section_t_name},
    {"address",     lua_section_t_address},
    {"exec",        lua_section_t_exec},
    {"size",        lua_section_t_size},
    {"offset",      lua_section_t_offset},
    {"type",        lua_section_t_type},
    {"num",         lua_section_t_num},
    {"entsize",     lua_section_t_entsize},
    {"link",        lua_section_t_link},
    {"symbol",      lua_section_t_symbol},
    {"relocation",  lua_section_t_relocation},
    {"disassemble", lua_section_t_disassemble},
    {"rop_table",   lua_section_t_rop_table},
    {"mem_at_address", lua_section_t_mem_at_address},
    {"__gc",        lua_section_t_gc},
    {NULL, NULL}
};

static const struct luaL_Reg symbol_lib_f [] = {
    {NULL, NULL}
};

static const struct luaL_Reg symbol_lib_m [] = {
    {"name",  lua_symbol_t_name},
    {"value", lua_symbol_t_value},
    {"type",  lua_symbol_t_type},
    {"size",  lua_symbol_t_size},
    {"__gc",  lua_symbol_t_gc},
    {NULL, NULL}
};

static const struct luaL_Reg relocation_lib_f [] = {
    {NULL, NULL}
};

static const struct luaL_Reg relocation_lib_m [] = {
    {"name",   lua_relocation_t_name},
    {"offset", lua_relocation_t_offset},
    {"type",   lua_relocation_t_type},
    {"__gc",   lua_relocation_t_gc},
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
    
    luaL_newmetatable(L, "rop_tools.section_t");
    lua_pushstring(L, "__index");
    lua_pushvalue(L, -2);
    lua_settable(L, -3);
    luaL_register(L, NULL, section_lib_m);
    luaL_register(L, "section_t", section_lib_f);
    
    luaL_newmetatable(L, "rop_tools.symbol_t");
    lua_pushstring(L, "__index");
    lua_pushvalue(L, -2);
    lua_settable(L, -3);
    luaL_register(L, NULL, symbol_lib_m);
    luaL_register(L, "symbol_t", symbol_lib_f);
    
    luaL_newmetatable(L, "rop_tools.relocation_t");
    lua_pushstring(L, "__index");
    lua_pushvalue(L, -2);
    lua_settable(L, -3);
    luaL_register(L, NULL, relocation_lib_m);
    luaL_register(L, "relocation_t", relocation_lib_f);
    
    return 1;
}


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
    struct _elf_shdr shdr;
    int shdr_i = -1;
    int found;
    struct lua_section_t * section;
    
    elf = lua_check_elf_t(L, 1);
    if (lua_isnumber(L, 2))
        shdr_i = luaL_checkinteger(L, 2);
    else if (lua_isstring(L, 2)) {
        found = 0;
        for (shdr_i = 0; shdr_i < int_t_get(elf_shnum(elf->elf)); shdr_i++) {
            elf_shdr(elf->elf, &shdr, shdr_i);
            if (strcmp(shdr_name(&shdr), lua_tostring(L, 2)) == 0) {
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
    
    lua_push_section_t(L);
    section = lua_check_section_t(L, 1);
    elf_shdr(elf->elf, &(section->shdr), shdr_i);
    section->elf_t = elf;
    elf->ref_count++;
    
    return 1;
}


int lua_elf_t_section_exists (lua_State * L)
{
    struct _elf * elf;
    struct _elf_shdr shdr;
    int shdr_i;
    int found;
    char * name;
    
    elf = lua_check_elf(L, 1);
    name = (char *) luaL_checkstring(L, 2);
    lua_pop(L, 2);
    
    found = 0;
    for (shdr_i = 0; shdr_i < int_t_get(elf_shnum(elf)); shdr_i++) {
        elf_shdr(elf, &shdr, shdr_i);
        if (strcmp(shdr_name(&shdr), name) == 0) {
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



/********************************
*            SECTION            *
********************************/

void lua_section_t_collect (struct lua_section_t * section)
{
    lua_elf_t_collect(section->elf_t);

    section->ref_count--;
    
    if (section->ref_count <= 0)
        free(section);
}


struct _elf_shdr * lua_check_shdr (lua_State * L, int position)
{
    struct lua_section_t ** section;
    void * userdata = luaL_checkudata(L, position, "rop_tools.section_t");
    luaL_argcheck(L, userdata != NULL, position, "section_t expected");
    section = (struct lua_section_t **) userdata;
    return (struct _elf_shdr *) &((*section)->shdr);
}


struct lua_section_t * lua_check_section_t (lua_State * L, int position)
{
    struct lua_section_t ** section;
    void * userdata = luaL_checkudata(L, position, "rop_tools.section_t");
    luaL_argcheck(L, userdata != NULL, position, "section_t expected");
    section = (struct lua_section_t **) userdata;
    return *section;
}


void lua_push_section_t  (lua_State * L)
{
    struct lua_section_t ** new_section;
    
    new_section = lua_newuserdata(L, sizeof(struct lua_section_t *));
    luaL_getmetatable(L, "rop_tools.section_t");
    lua_setmetatable(L, -2);
    
    *new_section = (struct lua_section_t *) malloc(sizeof(struct lua_section_t));
    (*new_section)->ref_count = 1;
}


int lua_section_t_gc (lua_State * L) 
{
    struct lua_section_t ** section;
    
    void * userdata = luaL_checkudata(L, 1, "rop_tools.section_t");
    luaL_argcheck(L, userdata != NULL, 1, "section_t expected");
    section = (struct lua_section_t **) userdata;

    lua_section_t_collect(*section);
    
    return 0;
}


int lua_section_t_name (lua_State * L)
{
    LUA_SECTION_T_ACCESSOR(lua_pushstring, shdr_name(shdr))
}


int lua_section_t_address (lua_State * L)
{
    LUA_SECTION_T_ACCESSOR(lua_push_uint_t, shdr_addr(shdr))
}


int lua_section_t_exec (lua_State * L)
{
    LUA_SECTION_T_ACCESSOR(lua_pushboolean, shdr_exec(shdr))
}


int lua_section_t_size (lua_State * L)
{
    LUA_SECTION_T_ACCESSOR(lua_push_int_t, shdr_size(shdr))
}


int lua_section_t_offset (lua_State * L)
{
    LUA_SECTION_T_ACCESSOR(lua_push_uint_t, shdr_offset(shdr))
}


int lua_section_t_type (lua_State * L)
{
    LUA_SECTION_T_ACCESSOR(lua_pushstring,
        shdr_type_strings[int_t_get(shdr_type(shdr))])
}


int lua_section_t_num (lua_State * L)
{
    LUA_SECTION_T_ACCESSOR(lua_pushinteger, shdr_num(shdr))
}


int lua_section_t_entsize (lua_State * L)
{
    LUA_SECTION_T_ACCESSOR(lua_push_int_t, shdr_entsize(shdr))
}


int lua_section_t_link (lua_State * L)
{
    LUA_SECTION_T_ACCESSOR(lua_push_int_t, shdr_link(shdr))
}


int lua_section_t_symbol (lua_State * L)
{
    struct lua_section_t * section;
    struct _elf_sym sym;
    int sym_i = 0;
    int found;
    struct lua_symbol_t * symbol;
    
    section = lua_check_section_t(L, 1);
    if (lua_isnumber(L, 2))
        sym_i = luaL_checkinteger(L, 2);
    else if (lua_isstring(L, 2)) {
        found = 0;
        for (sym_i = 0; sym_i < shdr_num(&(section->shdr)); sym_i++) {
            shdr_sym(&(section->shdr), &sym, sym_i);
            if (strcmp(sym_name(&sym), lua_tostring(L, 2)) == 0) {
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
    
    lua_push_symbol_t(L);
    symbol = lua_check_symbol_t(L, 1);
    shdr_sym(&(section->shdr), &(symbol->sym), sym_i);
    symbol->section_t = section;
    section->elf_t->ref_count++;
    section->ref_count++;
    
    return 1;
}


int lua_section_t_relocation (lua_State * L)
{
    struct lua_section_t * section;
    struct _elf_rel rel;
    int rel_i = 0;
    int found;
    struct lua_relocation_t * relocation;
    
    section = lua_check_section_t(L, 1);
    if (lua_isnumber(L, 2))
        rel_i = luaL_checkinteger(L, 2);
    else if (lua_isstring(L, 2)) {
        found = 0;
        for (rel_i = 0; rel_i < shdr_num(&(section->shdr)); rel_i++) {
            shdr_rel(&(section->shdr), &rel, rel_i);
            if (strcmp(rel_name(&rel), lua_tostring(L, 2)) == 0) {
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
    
    lua_push_relocation_t(L);
    relocation = lua_check_relocation_t(L, 1);
    shdr_rel(&(section->shdr), &(relocation->rel), rel_i);
    relocation->section_t = section;
    section->elf_t->ref_count++;
    section->ref_count++;
    
    return 1;
}


int lua_section_t_disassemble (lua_State * L)
{
    struct _elf_shdr * shdr;
    uint_t * address;
    uint_t   address_tmp;
    unsigned char * data;
    int disassemble_address = 0;
    
    shdr = lua_check_shdr(L, 1);
    if (lua_isuserdata(L, 2)) {
        address = lua_check_uint_t(L, 2);
        disassemble_address = 1;
        lua_pop(L, 2);
    }
    else
        lua_pop(L, 1);
    
    if (disassemble_address) {
        uint_t_set(&address_tmp, address);
        uint_t_sub(&address_tmp, shdr_addr(shdr));
        data = shdr_data(shdr);
        lua_dis_instruction(L, &(data[uint_t_get(&address_tmp)]),
                            ELF_CLASS(shdr->elf));
    }
    else
        lua_dis_table(L, shdr_addr(shdr), shdr_data(shdr),
                      int_t_get(shdr_size(shdr)), ELF_CLASS(shdr->elf));

    return 1;
}


int lua_section_t_rop_table (lua_State * L)
{
    struct _elf_shdr * shdr;
    struct _rop_list * rop_list;
    struct _rop_list * rop_list_first;
    struct _rop_ins  * rop_ins;
    int                rop_depth;
    int                rop_i;
    uint_t             address;
    
    shdr = lua_check_shdr(L, 1);
    rop_depth = luaL_checkinteger(L, 2);
    lua_pop(L, 2);
    
    rop_list_first = rop_ret_rops(shdr_data(shdr), shdr_size(shdr),
                                  rop_depth, ELF_CLASS(shdr->elf));
    rop_list = rop_list_first;
    
    rop_i = 1;
    lua_newtable(L);
    while (rop_list != NULL) {
        rop_ins = rop_list_ins(rop_list);
        uint_t_set(&address, shdr_addr(shdr));
        uint_t_add_int(&address, rop_list->offset);
        lua_dis_table(L, &address, rop_list->bytes,
                      rop_list->bytes_size, ELF_CLASS(shdr->elf));
        lua_pushinteger(L, (lua_Integer) rop_i);
        lua_insert(L, -2);
        lua_settable(L, -3);
        rop_i++;
        rop_list = rop_list->next;
    }
    
    rop_list_destroy(rop_list_first);
    
    return 1;
}


int lua_section_t_mem_at_address (lua_State * L)
{
    struct _elf_shdr * shdr;
    uint_t * address;
    uint_t   address_tmp;
    int      member_number;
    int      member_size;
    int      i;
    uint_t   uintt;
    unsigned char * data;
    int offset;
    
    shdr = lua_check_shdr(L, 1);
    address = lua_check_uint_t(L, 2);
    member_number = luaL_checkinteger(L, 3);
    member_size = luaL_checkinteger(L, 4);
    lua_pop(L, 4);
    
    data = shdr_data(shdr);
    offset = uint_t_get(shdr_addr(shdr)) - uint_t_get(address);
    uint_t_set(&address_tmp, address);
    lua_newtable(L);
    for (i = 0; i < member_number; i++) {
        switch (member_size) {
        case 8 :
            lua_push_uint_t(L, &address_tmp);
            uint_t_8_set(&uintt, *((uint8_t *) &(data[offset + i])));
            lua_push_uint_t(L, &uintt);
            lua_settable(L, -3);
            uint_t_add_int(&address_tmp, 1);
            break;
        case 16 :
            lua_push_uint_t(L, &address_tmp);
            uint_t_16_set(&uintt, *((uint16_t *) &(data[offset + i])));
            lua_push_uint_t(L, &uintt);
            lua_settable(L, -3);
            uint_t_add_int(&address_tmp, 2);
            break;
        case 32 :
            lua_push_uint_t(L, &address_tmp);
            uint_t_32_set(&uintt, *((uint32_t *) &(data[offset + i])));
            lua_push_uint_t(L, &uintt);
            lua_settable(L, -3);
            uint_t_add_int(&address_tmp, 4);
            break;
        case 64 :
            lua_push_uint_t(L, &address_tmp);
            uint_t_64_set(&uintt, *((uint64_t *) &(data[offset + i])));
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

void lua_symbol_t_collect (struct lua_symbol_t * symbol)
{
    lua_section_t_collect(symbol->section_t);

    symbol->ref_count--;
    
    if (symbol->ref_count <= 0)
        free(symbol);
}


struct _elf_sym * lua_check_sym (lua_State * L, int position)
{
    struct lua_symbol_t ** symbol;
    void * userdata = luaL_checkudata(L, position, "rop_tools.symbol_t");
    luaL_argcheck(L, userdata != NULL, position, "symbol_t expected");
    symbol = (struct lua_symbol_t **) userdata;
    return (struct _elf_sym *) &((*symbol)->sym);
}


struct lua_symbol_t * lua_check_symbol_t (lua_State * L, int position)
{
    struct lua_symbol_t ** symbol;
    void * userdata = luaL_checkudata(L, position, "rop_tools.symbol_t");
    luaL_argcheck(L, userdata != NULL, position, "section_t expected");
    symbol = (struct lua_symbol_t **) userdata;
    return *symbol;
}


void lua_push_symbol_t  (lua_State * L)
{
    struct lua_symbol_t ** new_symbol;
    
    new_symbol = lua_newuserdata(L, sizeof(struct lua_symbol_t **));
    luaL_getmetatable(L, "rop_tools.symbol_t");
    lua_setmetatable(L, -2);
    
    *new_symbol = (struct lua_symbol_t *) malloc(sizeof(struct lua_symbol_t));
    (*new_symbol)->ref_count = 1;
}


int lua_symbol_t_gc (lua_State * L)
{
    struct lua_symbol_t ** symbol;
    
    void * userdata = luaL_checkudata(L, 1, "rop_tools.symbol_t");
    luaL_argcheck(L, userdata != NULL, 1, "section_t expected");
    symbol = (struct lua_symbol_t **) userdata;

    lua_symbol_t_collect(*symbol);
    
    return 0;
}


int lua_symbol_t_name (lua_State * L)
{
    LUA_SYMBOL_T_ACCESSOR(lua_pushstring, sym_name(sym))
}


int lua_symbol_t_value (lua_State * L)
{
    LUA_SYMBOL_T_ACCESSOR(lua_push_uint_t, sym_value(sym))
}


int lua_symbol_t_type (lua_State * L)
{
    LUA_SYMBOL_T_ACCESSOR(lua_pushstring, \
        sym_type_strings[sym_type(sym)])
}


int lua_symbol_t_size (lua_State * L)
{
    LUA_SYMBOL_T_ACCESSOR(lua_push_int_t, sym_size(sym))
}



/********************************
*          RELOCATION           *
********************************/

void lua_relocation_t_collect (struct lua_relocation_t * relocation)
{
    lua_section_t_collect(relocation->section_t);

    relocation->ref_count--;
    
    if (relocation->ref_count <= 0)
        free(relocation);
}


struct _elf_rel * lua_check_rel (lua_State * L, int position)
{
    struct lua_relocation_t ** relocation;
    void * userdata = luaL_checkudata(L, position, "rop_tools.relocation_t");
    luaL_argcheck(L, userdata != NULL, position, "relocation_t expected");
    relocation = (struct lua_relocation_t **) userdata;
    return (struct _elf_rel *) &((*relocation)->rel);
}


struct lua_relocation_t * lua_check_relocation_t (lua_State * L, int position)
{
    struct lua_relocation_t ** relocation;
    void * userdata = luaL_checkudata(L, position, "rop_tools.relocation_t");
    luaL_argcheck(L, userdata != NULL, position, "relocation_t expected");
    relocation = (struct lua_relocation_t **) userdata;
    return *relocation;
}


void lua_push_relocation_t  (lua_State * L)
{
    struct lua_relocation_t ** new_relocation;
    
    new_relocation = lua_newuserdata(L, sizeof(struct lua_relocation_t **));
    luaL_getmetatable(L, "rop_tools.relocation_t");
    lua_setmetatable(L, -2);
    
    *new_relocation = (struct lua_relocation_t *)
                       malloc(sizeof(struct lua_relocation_t));
    (*new_relocation)->ref_count = 1;
}


int lua_relocation_t_gc (lua_State * L)
{
    struct lua_relocation_t ** relocation;
    
    void * userdata = luaL_checkudata(L, 1, "rop_tools.relocation_t");
    luaL_argcheck(L, userdata != NULL, 1, "relocation_t expected");
    relocation = (struct lua_relocation_t **) userdata;

    lua_relocation_t_collect(*relocation);
    
    return 0;
}


int lua_relocation_t_name (lua_State * L)
{
    LUA_RELOCATION_T_ACCESSOR(lua_pushstring, rel_name(rel))
}

int lua_relocation_t_offset (lua_State * L)
{
    LUA_RELOCATION_T_ACCESSOR(lua_push_uint_t, rel_offset(rel))
}

int lua_relocation_t_type (lua_State * L)
{
    LUA_RELOCATION_T_ACCESSOR(lua_pushstring, rel_type_strings[rel_type(rel)])
}
