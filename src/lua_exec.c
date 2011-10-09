#include "lua_exec.h"

static const struct luaL_Reg exec_lib_f [] = {
    {"new", lua_exec_t_new},
    {NULL, NULL}
};

static const struct luaL_Reg exec_lib_m [] = {
    {"__gc",         lua_exec_t_gc},
    {"num_sections", lua_exec_num_sections},
    {"size",         lua_exec_size},
    {"type",         lua_exec_type},
    {"section",      lua_exec_section},
    {"sections",     lua_exec_sections},
    {NULL, NULL}
};

static const struct luaL_Reg exec_section_lib_f [] = {
    {NULL, NULL}
};

static const struct luaL_Reg exec_section_lib_m [] = {
    {"__gc", lua_exec_section_t_gc},
    {"name", lua_exec_section_name},  
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
    
    section = lua_check_exec_section(L, 1);
    lua_pop(L, 1);
    lua_pushstring(L, exec_section_name(section));
    
    return 1;
}
