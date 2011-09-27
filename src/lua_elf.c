#include "lua_elf.h"

static const struct luaL_Reg elf_lib_f [] = {
    {"new", lua_elf_t_new},
    {NULL, NULL}
};


static const struct luaL_Reg elf_lib_m [] = {
    {"class", lua_elf_t_class},
    {"shnum", lua_elf_t_shnum},
    {"__gc", lua_elf_t_gc},
    {NULL, NULL}
};


int lua_open_elf_t (lua_State * L)
{
    luaL_newmetatable(L, "rop_tools.elf_t");
    
    lua_pushstring(L, "__index");
    lua_pushvalue(L, -2);
    lua_settable(L, -3);
    
    luaL_register(L, NULL, elf_lib_m);
    luaL_register(L, "elf_t", elf_lib_f);
    return 1;
}


struct _elf * lua_check_elf_t (lua_State * L, int position)
{
    void * userdata = luaL_checkudata(L, position, "rop_tools.elf_t");
    luaL_argcheck(L, userdata != NULL, position, "elf_t expected");
    return (struct _elf *) userdata;
}


void lua_push_elf_t  (lua_State * L, struct _elf * e)
{
    struct _elf * new_elf;
    
    new_elf = lua_newuserdata(L, sizeof(struct _elf));
    luaL_getmetatable(L, "rop_tools.elf_t");
    lua_setmetatable(L, -2);
    
    elf_copy(new_elf, e);
}


int lua_elf_t_new (lua_State * L)
{
    char * filename;
    struct _elf * tmp_elf;
    struct _elf * new_elf;
    
    filename = (char *) luaL_checkstring(L, 1);
    lua_pop(L, 1);
    
    new_elf = lua_newuserdata(L, sizeof(struct _elf));
    luaL_getmetatable(L, "rop_tools.elf_t");
    lua_setmetatable(L, -2);
    
    tmp_elf = elf_open(filename);
    if (tmp_elf == NULL)
        luaL_error(L, "error opening elf file %s", filename);
    
    elf_copy(new_elf, tmp_elf);
    elf_destroy(tmp_elf);
    
    return 1;
}


int lua_elf_t_gc (lua_State * L) 
{
    return 0;
}


int lua_elf_t_class (lua_State * L)
{
    return 0;
}



int lua_elf_t_shnum (lua_State * L)
{
    return 0;
}

