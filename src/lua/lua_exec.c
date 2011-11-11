#include "lua_exec.h"

static const struct luaL_Reg exec_lib_f [] = {
    {"new", lua_exec_t_new},
    {NULL, NULL}
};

static const struct luaL_Reg exec_lib_m [] = {
    {"__gc",           lua_exec_t_gc},
    {"num_sections",   lua_exec_num_sections},
    {"size",           lua_exec_size},
    {"type",           lua_exec_type},
    {"section",        lua_exec_section},
    {"sections",       lua_exec_sections},
    {"section_exists", lua_exec_section_exists},
    {"find_functions", lua_exec_find_functions},
    {"symbol",         lua_exec_symbol},
    {"symbols",        lua_exec_symbols},
    {"mem_at_address", lua_exec_mem_at_address},
    {NULL, NULL}
};

static const struct luaL_Reg exec_section_lib_f [] = {
    {NULL, NULL}
};

static const struct luaL_Reg exec_section_lib_m [] = {
    {"__gc",        lua_exec_section_t_gc},
    {"name",        lua_exec_section_name},
    {"address",     lua_exec_section_address},
    {"types",       lua_exec_section_types},
    {"size",        lua_exec_section_size},
    {"disassemble", lua_exec_section_disassemble},
    {NULL, NULL}
};

static const struct luaL_Reg exec_symbol_lib_f [] = {
    {NULL, NULL}
};

static const struct luaL_Reg exec_symbol_lib_m [] = {
    {"__gc",        lua_exec_symbol_t_gc},
    {"name",        lua_exec_symbol_name},
    {"address",     lua_exec_symbol_address},
    {"type",        lua_exec_symbol_type},
    {"size",        lua_exec_symbol_size},
    {NULL, NULL}
};


int lua_open_exec_t (lua_State * L)
{
    luaL_newmetatable(L, "rop_tools.exec_t");
    lua_pushstring(L, "__index");
    lua_pushvalue(L, -2);
    lua_settable(L, -3);
    luaL_register(L, NULL, exec_lib_m);
    luaL_register(L, "exec_t", exec_lib_f);
    
    luaL_newmetatable(L, "rop_tools.exec_section_t");
    lua_pushstring(L, "__index");
    lua_pushvalue(L, -2);
    lua_settable(L, -3);
    luaL_register(L, NULL, exec_section_lib_m);
    luaL_register(L, "exec_section_t", exec_section_lib_f);
    
    luaL_newmetatable(L, "rop_tools.exec_symbol_t");
    lua_pushstring(L, "__index");
    lua_pushvalue(L, -2);
    lua_settable(L, -3);
    luaL_register(L, NULL, exec_symbol_lib_m);
    luaL_register(L, "exec_symbol_t", exec_symbol_lib_f);
    
    return 1;
}


/********************************
*             EXEC              *
********************************/

void lua_exec_t_collect (struct lua_exec_t * exec_t)
{
    exec_t->ref_count--;
    
    if (exec_t->ref_count <= 0) {
        exec_destroy(exec_t->exec);
        free(exec_t);
    }
}


struct lua_exec_t * lua_check_exec_t (lua_State * L, int position)
{
    struct lua_exec_t ** exec_t;
    void * userdata = luaL_checkudata(L, position, "rop_tools.exec_t");
    luaL_argcheck(L, userdata != NULL, position, "exec_t expected");
    exec_t = (struct lua_exec_t **) userdata;
    return *exec_t;
}


struct _exec * lua_check_exec (lua_State * L, int position)
{
    struct lua_exec_t * exec_t;
    exec_t = lua_check_exec_t(L, position);
    return exec_t->exec;
}


void lua_push_exec (lua_State * L, struct _exec * exec)
{
    struct lua_exec_t ** new_exec;
    
    new_exec = lua_newuserdata(L, sizeof(struct lua_exec_t *));
    luaL_getmetatable(L, "rop_tools.exec_t");
    lua_setmetatable(L, -2);
    
    (*new_exec)->exec = NULL;
    exec_copy((*new_exec)->exec, exec);
}


int lua_exec_t_new (lua_State * L)
{
    char * filename;
    struct lua_exec_t ** new_exec;
    
    filename = (char *) luaL_checkstring(L, 1);
    lua_pop(L, 1);
    
    new_exec = lua_newuserdata(L, sizeof(struct lua_exec_t **));
    luaL_getmetatable(L, "rop_tools.exec_t");
    lua_setmetatable(L, -2);
    
    *new_exec = (struct lua_exec_t *) malloc(sizeof(struct lua_exec_t));
    (*new_exec)->ref_count = 1;
    
    (*new_exec)->exec = exec_open(filename);
    if ((*new_exec)->exec == NULL)
        luaL_error(L, "error opening exec file %s", filename);
    
    return 1;
}


int lua_exec_t_gc (lua_State * L) 
{
    struct lua_exec_t ** exec;
    
    void * userdata = luaL_checkudata(L, 1, "rop_tools.exec_t");
    luaL_argcheck(L, userdata != NULL, 1, "exec_t expected");
    exec = (struct lua_exec_t **) userdata;
    
    lua_exec_t_collect(*exec);
    
    return 0;
}


int lua_exec_num_sections (lua_State * L)
{
    struct _exec * exec;
    
    exec = lua_check_exec(L, 1);
    lua_pop(L, 1);
    lua_pushinteger(L, exec_num_sections(exec));
    
    return 1;
}


int lua_exec_size (lua_State * L)
{
    struct _exec * exec;
    
    exec = lua_check_exec(L, 1);
    lua_pop(L, 1);
    lua_pushinteger(L, exec_size(exec));
    
    return 1;
}


int lua_exec_type (lua_State * L)
{
    struct _exec * exec;
    
    exec = lua_check_exec(L, 1);
    lua_pop(L, 1);
    lua_pushstring(L, exec_type_strings[exec_type(exec)]);
    
    return 1;
}


int lua_exec_find_functions (lua_State * L)
{
    struct _exec * exec;
    struct _exec_section section;
    int section_i;
    int function_i;
    struct _analyze_function analyze_function;
    struct _analyze_function * analyze_function_p;

    _aatree * tree = NULL;
    _aatree * tree_tmp = NULL;
    _list   * list;
    _list_iterator it;
    
    exec = lua_check_exec(L, -1);
    lua_pop(L, 1);
    
    // grab the functions in each executable section
    for (section_i = 0; section_i < exec_num_sections(exec); section_i++) {
        exec_section(exec, &section, section_i);
        if (exec_section_types(&section) & EXEC_SECTION_TYPE_EXECUTABLE) {
            tree_tmp = analyze_find_functions(exec_section_data(&section),
                                              exec_section_size(&section),
                                              exec_mode(exec),
                                              exec_section_address(&section));
            if (tree == NULL)
                tree = tree_tmp;
            else {
                aatree_merge(tree, tree_tmp);
                aatree_destroy(tree_tmp);
            }
        }
    }

    // the first function typically isn't called from the loader and not from
    // within the binary. we're going to find the start functions here and make
    // sure they're added.
    uint_t_8_set(&(analyze_function.address), 0);
    switch (exec_type(exec)) {
    case EXEC_TYPE_ELF :
        // for ELFs, the first function is at the start of .text
        for (section_i = 0; section_i < exec_num_sections(exec); section_i++) {
            exec_section(exec, &section, section_i);
            if (strcmp(exec_section_name(&section), ".text") == 0) {
                uint_t_set(&(analyze_function.address), exec_section_address(&section));
                break;
            }
        }
        break;
    case EXEC_TYPE_PE :
        // for PEs, the first function can be found at AddressOfEntryPoint in
        // the optional header, or for us pe_AddressOfEntryPoint
        uint_t_set(&(analyze_function.address), pe_AddressOfEntryPoint(exec->e.pe));
        break;
    }
    aatree_insert(tree, &analyze_function);
   
    // make tree list, and add to lua table
    list = list_copy_aatree(tree);
    aatree_destroy(tree);
    list_iterator(list, &it);
    function_i = 1;
    lua_newtable(L);
    while ((analyze_function_p = (struct _analyze_function *) list_next(&it)) != NULL) {
        lua_pushinteger(L, function_i++);
        lua_push_uint_t(L, &(analyze_function_p->address));
        lua_settable(L, -3);
    }
    
    list_destroy(list);

    return 1;
}


int lua_exec_section_exists (lua_State * L)
{
    struct _exec * exec;
    struct _exec_section section;
    int section_i;
    
    exec = lua_check_exec(L, -2);
    for (section_i = 0; section_i < exec_num_sections(exec); section_i++) {
        exec_section(exec, &section, section_i);
        if (strcmp(exec_section_name(&section), luaL_checkstring(L, -1))) {
            lua_pop(L, 2);
            lua_pushboolean(L, 1);
            return 1;
        }
    }
    
    lua_pop(L, 2);
    lua_pushboolean(L, 0);
    
    return 1;
}
    


int lua_exec_section (lua_State * L)
{
    struct lua_exec_t * exec_t;
    struct lua_exec_section_t * section_t;
    struct _exec_section section;
    int section_i = -1;
    
    exec_t = lua_check_exec_t(L, -2);
    if (lua_isnumber(L, -1)) {
        section_i = luaL_checkinteger(L, -1);
    }
    else if (lua_isstring(L, -1)) {
        for (section_i = 0;
             section_i < exec_num_sections(exec_t->exec);
             section_i++) {
            exec_section(exec_t->exec, &section, section_i);
            if (strcmp(exec_section_name(&section),
                       luaL_checkstring(L, -1)) == 0)
                break;
        }
    }
    else
        luaL_error(L, "method expected a string or integer");
        
    lua_pop(L, 2);
    
    if (section_i >= exec_num_sections(exec_t->exec))
        luaL_error(L, "requested a section not found in exec_t");
    
    lua_push_exec_section_t(L);
    section_t = lua_check_exec_section_t(L, -1);
    
    exec_section(exec_t->exec, &(section_t->section), section_i);
    section_t->exec_t = exec_t;
    exec_t->ref_count++;
    
    return 1;
}


int lua_exec_sections (lua_State * L)
{
    struct lua_exec_t * exec_t;
    int section_i;
    
    exec_t = lua_check_exec_t(L, 1);
    // don't take exec_t off the stack yet. we're going to use it to do
    // terrible, terrible things
    
    lua_newtable(L);
    for (section_i = 0; 
         section_i < exec_num_sections(exec_t->exec);
         section_i++) {
        lua_pushvalue(L, 1);
        lua_pushinteger(L, section_i);
        lua_exec_section(L);
        lua_pushinteger(L, section_i + 1);
        lua_insert(L, -2);
        lua_settable(L, -3);
    }
    
    lua_insert(L, -2);
    lua_pop(L, 1);
    
    return 1;
}


int lua_exec_symbol (lua_State * L)
{
    struct lua_exec_t * exec_t;
    struct lua_exec_symbol_t * symbol_t;
    struct _exec_symbol symbol;
    int symbol_i = -1;
    
    exec_t = lua_check_exec_t(L, -2);
    if (lua_isnumber(L, -1)) {
        symbol_i = luaL_checkinteger(L, -1);
    }
    else if (lua_isstring(L, -1)) {
        for (symbol_i = 0;
             symbol_i < exec_num_symbols(exec_t->exec);
             symbol_i++) {
            exec_symbol(exec_t->exec, &symbol, symbol_i);
            if (strcmp(exec_symbol_name(&symbol),
                       luaL_checkstring(L, -1)) == 0)
                break;
        }
    }
    else
        luaL_error(L, "method expected a string or integer");
        
    lua_pop(L, 2);
    
    
    lua_push_exec_symbol_t(L);
    symbol_t = lua_check_exec_symbol_t(L, -1);
    
    exec_symbol(exec_t->exec, &(symbol_t->symbol), symbol_i);
    symbol_t->exec_t = exec_t;
    exec_t->ref_count++;
    
    return 1;
}


int lua_exec_symbols (lua_State * L)
{
    struct lua_exec_t * exec_t;
    int symbol_i;
    
    exec_t = lua_check_exec_t(L, 1);
    
    lua_newtable(L);
    for (symbol_i = 0; symbol_i < exec_num_symbols(exec_t->exec); symbol_i++) {
        lua_pushvalue(L, 1);
        lua_pushinteger(L, symbol_i);
        lua_exec_symbol(L);
        lua_pushinteger(L, symbol_i + 1);
        lua_insert(L, -2);
        lua_settable(L, -3);
    }
    
    lua_insert(L, -2);
    lua_pop(L, 1);
    
    return 1;
}


int lua_exec_mem_at_address (lua_State * L)
{
    struct _exec * exec;
    struct _exec_section section;
    uint_t address;
    uint_t address_end;
    uint_t section_end_addr;
    uint_t mem;
    uint_t mem_offset;
    unsigned char * data;
    int mem_size;
    int mem_num;
    int section_i;
    int mem_i;
    int mem_found = 0;

    exec = lua_check_exec(L, 1);
    uint_t_set(&address, lua_check_uint_t(L, 2));
    mem_size = luaL_checkinteger(L, 3);
    mem_num = luaL_checkinteger(L, 4);

    if (    (mem_size != 8)
         && (mem_size != 16)
         && (mem_size != 32)
         && (mem_size != 64))
        luaL_error(L, "mem_size for lua_exec_mem_at_address must be one of 8,16,32,64");

    lua_pop(L, 4);

    // find the section which holds this memory
    for (section_i = 0; section_i < exec_num_sections(exec); section_i++) {
        exec_section(exec, &section, section_i);
        uint_t_set(&section_end_addr, exec_section_address(&section));
        uint_t_add_int(&section_end_addr, exec_section_size(&section));
        if (    (uint_t_cmp(exec_section_address(&section), &address) <= 0)
             && (uint_t_cmp(&section_end_addr, &address) > 0)) {
            data = exec_section_data(&section);
            uint_t_set(&mem_offset, &address);
            uint_t_sub(&mem_offset, exec_section_address(&section));

            mem_found = 1;
            // create the table for the memory
            lua_newtable(L);
            for (mem_i = 0; mem_i < mem_num; mem_i++) {
                // make sure this memory is held completely within the section
                uint_t_set(&address_end, &address);
                uint_t_add_int(&address_end, (mem_size / 8) * mem_i);
                if (uint_t_cmp(&address_end, &section_end_addr) >= 0)
                    break;
                // add mem to table
                lua_pushinteger(L, mem_i);
                switch (mem_size) {
                case 8 :
                    uint_t_8_set(&mem,  *((uint8_t *)  &(data[uint_t_get(&mem_offset)])));
                    uint_t_add_int(&mem_offset, 1);
                    uint_t_add_int(&address, 1);
                    break;
                case 16 :
                    uint_t_16_set(&mem, *((uint16_t *) &(data[uint_t_get(&mem_offset)])));
                    uint_t_add_int(&mem_offset, 2);
                    uint_t_add_int(&address, 2);
                    break;
                case 32 :
                    uint_t_32_set(&mem, *((uint32_t *) &(data[uint_t_get(&mem_offset)])));
                    uint_t_add_int(&mem_offset, 4);
                    uint_t_add_int(&address, 4);
                    break;
                case 64 :
                    uint_t_64_set(&mem, *((uint64_t *) &(data[uint_t_get(&mem_offset)])));
                    uint_t_add_int(&mem_offset, 16);
                    uint_t_add_int(&address, 8);
                    break;
                }
                lua_push_uint_t(L, &mem);
                lua_settable(L, -3);
            }
            break;
        }
    }

    return mem_found;
}



/********************************
*         EXEC_SECTION          *
********************************/

void lua_exec_section_t_collect (struct lua_exec_section_t * section_t)
{
    section_t->ref_count--;
    
    lua_exec_t_collect(section_t->exec_t);
    
    if (section_t->ref_count <= 0) {
        free(section_t);
    }
}


struct lua_exec_section_t * lua_check_exec_section_t (lua_State * L, int position)
{
    struct lua_exec_section_t ** section_t;
    void * userdata = luaL_checkudata(L, position, "rop_tools.exec_section_t");
    luaL_argcheck(L, userdata != NULL, position, "exec_section_t expected");
    section_t = (struct lua_exec_section_t **) userdata;
    return *section_t;
}


struct _exec_section * lua_check_exec_section (lua_State * L, int position)
{
    struct lua_exec_section_t * section_t;
    section_t = lua_check_exec_section_t(L, position);
    return &(section_t->section);
}


void lua_push_exec_section_t (lua_State * L)
{
    struct lua_exec_section_t ** new_section;
    
    new_section = lua_newuserdata(L, sizeof(struct lua_exec_section_t *));
    luaL_getmetatable(L, "rop_tools.exec_section_t");
    lua_setmetatable(L, -2);
    
    *new_section = (struct lua_exec_section_t *)
                   malloc(sizeof(struct lua_exec_section_t));
    (*new_section)->ref_count = 1;
}


int lua_exec_section_t_gc (lua_State * L) 
{
    struct lua_exec_section_t ** section_t;
    
    void * userdata = luaL_checkudata(L, 1, "rop_tools.exec_section_t");
    luaL_argcheck(L, userdata != NULL, 1, "exec_section_t expected");
    section_t = (struct lua_exec_section_t **) userdata;
    
    lua_exec_section_t_collect(*section_t);
    
    return 0;
}

int lua_exec_section_name (lua_State * L)
{
    struct _exec_section * section;
    
    section = lua_check_exec_section(L, -1);
    lua_pop(L, 1);
    lua_pushstring(L, exec_section_name(section));
    
    return 1;
}

int lua_exec_section_address (lua_State * L)
{
    struct _exec_section * section;
    
    section = lua_check_exec_section(L, -1);
    lua_pop(L, 1);
    lua_push_uint_t(L, exec_section_address(section));
    
    return 1;
}

int lua_exec_section_types (lua_State * L)
{
    struct _exec_section * section;
    int types;
    int index = 1;
    
    section = lua_check_exec_section(L, -1);
    lua_pop(L, 1);
    types = exec_section_types(section);
    
    lua_newtable(L);
    if (types & EXEC_SECTION_TYPE_SYMBOL) {
        lua_pushinteger(L, index++);
        lua_pushstring(L, "symbol");
        lua_settable(L, -3);
    }
    if (types & EXEC_SECTION_TYPE_RELOCATION) {
        lua_pushinteger(L, index++);
        lua_pushstring(L, "relocation");
        lua_settable(L, -3);
    }
    if (types & EXEC_SECTION_TYPE_TEXT) {
        lua_pushinteger(L, index++);
        lua_pushstring(L, "text");
        lua_settable(L, -3);
    }
    if (types & EXEC_SECTION_TYPE_EXECUTABLE) {
        lua_pushinteger(L, index++);
        lua_pushstring(L, "executable");
        lua_settable(L, -3);
    }
    
    return 1;
}

int lua_exec_section_size (lua_State * L)
{
    struct _exec_section * section;
    
    section = lua_check_exec_section(L, -1);
    lua_pop(L, 1);
    lua_pushinteger(L, exec_section_size(section));
    
    return 1;
}

int lua_exec_section_disassemble (lua_State * L)
{
    struct _exec_section * section;
    uint_t * address = NULL;
    uint_t address_tmp;
    unsigned char * data;
    
    section = lua_check_exec_section(L, -1);
    if (lua_isuserdata(L, -2)) {
        address = lua_check_uint_t(L, 2);
        lua_pop(L, 2);
    }
    else
        lua_pop(L, 1);

    if (address != NULL) {
        uint_t_set(&address_tmp, address);
        uint_t_sub(&address_tmp, exec_section_address(section));
        data = exec_section_data(section);
        lua_dis_instruction(L, &(data[uint_t_get(&address_tmp)]),
                            exec_mode(section->exec));
    }
    else
        lua_dis_table(L,
                      exec_section_address(section),
                      exec_section_data(section),
                      exec_section_size(section),
                      exec_mode(section->exec));

    return 1;
}



/********************************
*         EXEC_SYMBOL           *
********************************/

void lua_exec_symbol_t_collect (struct lua_exec_symbol_t * symbol_t)
{
    symbol_t->ref_count--;
    
    lua_exec_t_collect(symbol_t->exec_t);
    
    if (symbol_t->ref_count <= 0) {
        free(symbol_t);
    }
}


struct lua_exec_symbol_t * lua_check_exec_symbol_t (lua_State * L, int position)
{
    struct lua_exec_symbol_t ** symbol_t;
    void * userdata = luaL_checkudata(L, position, "rop_tools.exec_symbol_t");
    luaL_argcheck(L, userdata != NULL, position, "exec_symbol_t expected");
    symbol_t = (struct lua_exec_symbol_t **) userdata;
    return *symbol_t;
}


struct _exec_symbol * lua_check_exec_symbol (lua_State * L, int position)
{
    struct lua_exec_symbol_t * symbol_t;
    symbol_t = lua_check_exec_symbol_t(L, position);
    return &(symbol_t->symbol);
}


void lua_push_exec_symbol_t (lua_State * L)
{
    struct lua_exec_symbol_t ** new_symbol;
    
    new_symbol = lua_newuserdata(L, sizeof(struct lua_exec_symbol_t *));
    luaL_getmetatable(L, "rop_tools.exec_symbol_t");
    lua_setmetatable(L, -2);
    
    *new_symbol = (struct lua_exec_symbol_t *)
                   malloc(sizeof(struct lua_exec_symbol_t));
    (*new_symbol)->ref_count = 1;
}


int lua_exec_symbol_t_gc (lua_State * L) 
{
    struct lua_exec_symbol_t ** symbol_t;
    
    void * userdata = luaL_checkudata(L, 1, "rop_tools.exec_symbol_t");
    luaL_argcheck(L, userdata != NULL, 1, "exec_symbol_t expected");
    symbol_t = (struct lua_exec_symbol_t **) userdata;
    
    lua_exec_symbol_t_collect(*symbol_t);
    
    return 0;
}


int lua_exec_symbol_name (lua_State * L)
{
    struct _exec_symbol * symbol;
    
    symbol = lua_check_exec_symbol(L, -1);
    lua_pop(L, 1);
    lua_pushstring(L, exec_symbol_name(symbol));
    
    return 1;
}


int lua_exec_symbol_address (lua_State * L)
{
    struct _exec_symbol * symbol;
    
    symbol = lua_check_exec_symbol(L, -1);
    lua_pop(L, 1);
    lua_push_uint_t(L, exec_symbol_address(symbol));
    
    return 1;
}


int lua_exec_symbol_type (lua_State * L)
{
    struct _exec_symbol * symbol;
    
    symbol = lua_check_exec_symbol(L, -1);
    lua_pop(L, 1);
    lua_pushstring(L, exec_symbol_type_strings[exec_symbol_type(symbol)]);
    
    return 1;
}


int lua_exec_symbol_size (lua_State * L)
{
    struct _exec_symbol * symbol;
    
    symbol = lua_check_exec_symbol(L, -1);
    lua_pop(L, 1);
    lua_pushinteger(L, exec_symbol_size(symbol));
    
    return 1;
}

