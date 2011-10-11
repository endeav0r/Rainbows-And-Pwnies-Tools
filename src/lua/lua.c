#include "lua.h"



int lua_run_file (char * filename, char * argv[], int argc)
{
    int error;
    int args_i;
    
    lua_State * L = luaL_newstate();
    luaL_openlibs(L);
    lua_open_int_t(L);
    lua_open_uint_t(L);
    lua_open_elf_t(L);
    lua_open_exec_t(L);
        
    lua_newtable(L);
    for (args_i = 0; args_i < argc; args_i++) {
        lua_pushinteger(L, (lua_Integer) args_i + 1);
        lua_pushstring(L, argv[args_i]);
        lua_settable(L, -3);
    }
    lua_setglobal(L, "argv");
    
    
    error = luaL_loadfile(L, filename);
    switch (error) {
    case LUA_ERRSYNTAX :
        lua_error(L);
        break;
    case LUA_ERRMEM :
        printf("lua memory error\n");
        break;
    case LUA_ERRFILE :
        printf("lua couldn't open file %s\n", filename);
        break;
    }
    
    error = lua_pcall(L, 0, 0, 0);
    switch (error) {
    case LUA_ERRRUN :
        lua_error(L);
        break;
    case LUA_ERRMEM :
        printf("lua memory error\n");
        break;
    case LUA_ERRERR :
        printf("lua error\n");
        break;
    }
    
    lua_close(L);

    return 0;
}
