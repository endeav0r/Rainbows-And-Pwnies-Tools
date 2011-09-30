#include "lua.h"

/*
static int lua_make_rop_table (lua_State * L)
{
    struct _rop_list * rop_list;
    struct _rop_list * rop_list_first;
    struct _rop_ins  * rop_ins;
    struct _elf      * elf;
    struct _elf_shdr   shdr;
    char             * filename;
    int                rop_depth;
    int                shdr_i;
    int                rop_list_count = 1;
    int                rop_ins_count;
    int                mode;
    
    filename  = (char *) luaL_checkstring(L, 1);
    rop_depth = luaL_checkint(L, 2);
    lua_pop(L, 2);
    lua_newtable(L);
    
    elf = elf_open(filename);
    
    for (shdr_i = 0; shdr_i < int_t_get(elf_shnum(elf)); shdr_i++) {
        elf_shdr(elf, &shdr, shdr_i);
        // for each executable shdr
        if (shdr_exec(&shdr)) {
            switch (elf_class(elf)) {
            case ELFCLASS32 : mode = 32; break;
            case ELFCLASS64 : mode = 64; break;
            default: mode = 32;
            }
            rop_list = rop_ret_rops(shdr_data(&shdr),
                                    int_t_get(shdr_size(&shdr)),
                                    rop_depth, mode);
            rop_list_first = rop_list;
            // for each rop sequence
            while (rop_list != NULL) {
                rop_ins_count = 1;
                // create a table for these rop instructions
                lua_newtable(L);
                rop_ins = rop_list_ins(rop_list);
                while (rop_ins != NULL) {
                    lua_newtable(L);
                    lua_pushstring(L, "address");
                    lua_pushinteger(L, (lua_Integer) (uint_t_get(shdr_addr(&shdr))
                                                     + rop_ins->offset));
                    lua_settable(L, -3);
                    lua_pushstring(L, "description");
                    lua_pushstring(L, rop_ins->description);
                    lua_settable(L, -3);
                    lua_pushstring(L, "mnemonic");
                    lua_pushstring(L, mnemonic_strings[rop_ins->mnemonic]);
                    lua_settable(L, -3);
                    lua_operands_table(L, rop_ins->bytes,
                                       rop_ins->bytes_size, mode);
                    lua_pushstring(L, "operands");
                    lua_insert(L, -2);
                    lua_settable(L, -3);
                    lua_pushinteger(L, (lua_Integer) rop_ins_count);
                    lua_insert(L, -2);
                    lua_settable(L, -3);
                    rop_ins_count++;
                    rop_ins = rop_ins->next;
                }
                // add these rop instructions to the rop table
                lua_pushinteger(L, (lua_Integer) rop_list_count);
                lua_insert(L, -2);
                lua_settable(L, -3);
                rop_list_count++;
                rop_list = rop_list->next;
            }
            rop_list_destroy(rop_list_first);
        }
    }
    
    elf_destroy(elf);
    
    return 1;
}
*/


int lua_run_file (char * filename, char * argv[], int argc)
{
    int error;
    int args_i;
    
    lua_State * L = lua_open();
    luaL_openlibs(L);
    lua_open_int_t(L);
    lua_open_uint_t(L);
    lua_open_elf_t(L);
        
    lua_newtable(L);
    for (args_i = 0; args_i < argc; args_i++) {
        lua_pushinteger(L, (lua_Integer) args_i + 1);
        lua_pushstring(L, argv[args_i]);
        lua_settable(L, -3);
    }
    lua_setglobal(L, "argv");
    
    
    //lua_pushcfunction(L, lua_make_rop_table);
    //lua_setglobal(L, "make_rop_table");
    
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
