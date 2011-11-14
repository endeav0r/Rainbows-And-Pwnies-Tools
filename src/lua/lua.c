#include "lua.h"

int lua_rt_init (lua_State * L, int argc, char * argv[])
{
    int args_i;

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

    return 0;
}


int lua_interactive (int argc, char * argv[])
{
    int error = 0;
    char * line;
    lua_State * L;
    char history_filename[256];

    snprintf(history_filename, 256, "%s/%s", 
             getenv("HOME"), LUA_HISTORY_FILENAME);

    L = luaL_newstate();
    lua_rt_init(L, argc, argv);

    read_history(history_filename);

    while (1) {
        line = readline("X) ");
        add_history(line);
        stifle_history(LUA_HISTORY_LINES);
        write_history(history_filename);

        error = luaL_dostring(L, line);
        if (error) {
            line = (char *) luaL_checkstring(L, -1);
            printf("%s\n", line);
        }
        fflush(stdout);
    }

    return 0;
}


int lua_run_file (char * filename, int argc, char * argv[])
{
    int error;
    lua_State * L;
    
    L = luaL_newstate();
    lua_rt_init(L, argc, argv);
    
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
