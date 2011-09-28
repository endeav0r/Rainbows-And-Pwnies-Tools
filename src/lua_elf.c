#include "lua_elf.h"

/********************************
*              ELF              *
********************************/

static const struct luaL_Reg elf_lib_f [] = {
    {"new", lua_elf_t_new},
    {NULL, NULL}
};


static const struct luaL_Reg elf_lib_m [] = {
    {"class",    lua_elf_t_class},
    {"shnum",    lua_elf_t_shnum},
    {"filename", lua_elf_t_filename},
    {"section",  lua_elf_t_section},
    {"__gc",     lua_elf_t_gc},
    {NULL, NULL}
};

static const struct luaL_Reg section_lib_f [] = {
    {NULL, NULL}
};


static const struct luaL_Reg section_lib_m [] = {
    {"name", lua_section_t_name},
    {"__gc", lua_section_t_gc},
    {NULL, NULL}
};


void lua_elf_t_collect (struct lua_elf_t * elf_t)
{
    elf_t->ref_count--;
    
    if (elf_t->ref_count <= 0) {
        elf_destroy(elf_t->elf);
        free(elf_t);
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
    
    return 1;
}


struct _elf * lua_check_elf (lua_State * L, int position)
{
    struct lua_elf_t ** elf_t;
    void * userdata = luaL_checkudata(L, position, "rop_tools.elf_t");
    luaL_argcheck(L, userdata != NULL, position, "elf_t expected");
    elf_t = (struct lua_elf_t **) userdata;
    return (struct _elf *) (*elf_t)->elf;
}


struct lua_elf_t * lua_check_elf_t (lua_State * L, int position)
{
    struct lua_elf_t ** elf_t;
    void * userdata = luaL_checkudata(L, position, "rop_tools.elf_t");
    luaL_argcheck(L, userdata != NULL, position, "elf_t expected");
    elf_t = (struct lua_elf_t **) userdata;
    return *elf_t;
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
    struct lua_elf_t ** elf_t;
    
    void * userdata = luaL_checkudata(L, 1, "rop_tools.elf_t");
    luaL_argcheck(L, userdata != NULL, 1, "elf_t expected");
    elf_t = (struct lua_elf_t **) userdata;
    
    lua_elf_t_collect(*elf_t);
    
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
    struct lua_elf_t * elf_t;
    int section_index;
    struct lua_section_t * section;
    
    elf_t = lua_check_elf_t(L, 1);
    section_index = luaL_checkinteger(L, 2);
    lua_pop(L, 2);
    
    lua_push_section_t(L);
    section = lua_check_section_t(L, 1);
    elf_shdr(elf_t->elf, &(section->shdr), section_index);
    section->elf_t = elf_t;
    elf_t->ref_count++;
    
    return 1;
}
    




/********************************
*            SECTION            *
********************************/

void lua_section_t_collect (struct lua_section_t * section_t)
{
    lua_elf_t_collect(section_t->elf_t);

    section_t->ref_count--;
    
    if (section_t->ref_count <= 0)
        free(section_t);
}


struct _elf_shdr * lua_check_shdr (lua_State * L, int position)
{
    struct lua_section_t ** section_t;
    void * userdata = luaL_checkudata(L, position, "rop_tools.section_t");
    luaL_argcheck(L, userdata != NULL, position, "section_t expected");
    section_t = (struct lua_section_t **) userdata;
    return (struct _elf_shdr *) &((*section_t)->shdr);
}


struct lua_section_t * lua_check_section_t (lua_State * L, int position)
{
    struct lua_section_t ** section_t;
    void * userdata = luaL_checkudata(L, position, "rop_tools.section_t");
    luaL_argcheck(L, userdata != NULL, position, "section_t expected");
    section_t = (struct lua_section_t **) userdata;
    return *section_t;
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
    struct lua_section_t ** section_t;
    
    void * userdata = luaL_checkudata(L, 1, "rop_tools.section_t");
    luaL_argcheck(L, userdata != NULL, 1, "section_t expected");
    section_t = (struct lua_section_t **) userdata;

    lua_section_t_collect(*section_t);
    
    return 0;
}


int lua_section_t_name (lua_State * L)
{
    struct _elf_shdr * shdr;
    
    shdr = lua_check_shdr(L, 1);
    lua_pop(L, 1);
    
    lua_pushstring(L, shdr_name(shdr));
    return 1;
}
