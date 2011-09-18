#include "lua.h"


void lua_rop_ins_operand_table (lua_State * L, struct _rop_ins * rop_ins)
{
    int                op_i;
    ud_t ud_obj;

    // table for operands
    lua_newtable(L);

    ud_init(&ud_obj);
    ud_set_mode(&ud_obj, 32);
    ud_set_input_buffer(&ud_obj, rop_ins->bytes, rop_ins->bytes_size);
    ud_set_syntax(&ud_obj, NULL);
    ud_disassemble(&ud_obj);
    for (op_i = 0; op_i < 3; op_i++) {
        // table for this operand
        lua_newtable(L);
        lua_pushstring(L, "type");
        lua_pushstring(L, types_strings[ud_obj.operand[op_i].type]);
        lua_settable(L, -3);
        lua_pushstring(L, "base");
        lua_pushstring(L, types_strings[ud_obj.operand[op_i].base]);
        lua_settable(L, -3);
        lua_pushstring(L, "index");
        lua_pushstring(L, types_strings[ud_obj.operand[op_i].index]);
        lua_settable(L, -3);
        lua_pushstring(L, "offset");
        lua_pushstring(L, types_strings[ud_obj.operand[op_i].offset]);
        lua_settable(L, -3);
        lua_pushstring(L, "scale");
        lua_pushstring(L, types_strings[ud_obj.operand[op_i].scale]);
        lua_settable(L, -3);
        lua_pushstring(L, "size");
        lua_pushinteger(L, (lua_Integer) ud_obj.operand[op_i].size);
        lua_settable(L, -3);
            
        lua_pushstring(L, "lval");
        if (    (ud_obj.operand[op_i].type == UD_OP_IMM)
             || (ud_obj.operand[op_i].offset)) {
            switch (ud_obj.operand[op_i].size) {
            case 8 :
                lua_pushinteger(L, (lua_Integer) ud_obj.operand[op_i].lval.sbyte);
                break;
            case 16 :
                lua_pushinteger(L, (lua_Integer) ud_obj.operand[op_i].lval.uword);
                break;
            case 32 :
                lua_pushinteger(L, (lua_Integer) ud_obj.operand[op_i].lval.udword);
                break;
            case 64 :
                lua_pushinteger(L, (lua_Integer) ud_obj.operand[op_i].lval.uqword);
                break;
            default :
                lua_pushinteger(L, (lua_Integer) 0);
                break;
            }
        }
        else
            lua_pushinteger(L, (lua_Integer) 0);
        lua_settable(L, -3);
            
        lua_pushinteger(L, (lua_Integer) op_i + 1);
        lua_insert(L, -2);
        lua_settable(L, -3);
    }
    lua_pushstring(L, "operands");
    lua_insert(L, -2);
    lua_settable(L, -3);
}


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
    
    filename  = (char *) luaL_checkstring(L, 1);
    rop_depth = luaL_checkint(L, 2);
    lua_pop(L, 2);
    lua_newtable(L);
    
    elf = elf_open(filename);
    
    for (shdr_i = 0; shdr_i < elf_shnum(elf); shdr_i++) {
        elf_shdr(elf, &shdr, shdr_i);
        // for each executable shdr
        if (shdr_exec(&shdr)) {
            rop_list = rop_ret_rops(shdr_data(&shdr), shdr_size(&shdr), rop_depth);
            rop_list_first = rop_list;
            // for each rop sequence
            while (rop_list != NULL) {
                rop_ins_count = 1;
                // create a table for these rop instructions
                lua_newtable(L);
                lua_pushstring(L, "address");
                lua_pushinteger(L, (lua_Integer) (shdr_addr(&shdr) + rop_list->offset));
                lua_settable(L, -3);
                rop_ins = rop_list_ins(rop_list);
                while (rop_ins != NULL) {
                    lua_newtable(L);
                    lua_pushstring(L, "address");
                    lua_pushinteger(L, (lua_Integer) (shdr_addr(&shdr) + rop_ins->offset));
                    lua_settable(L, -3);
                    lua_pushstring(L, "description");
                    lua_pushstring(L, rop_ins->description);
                    lua_settable(L, -3);
                    lua_pushstring(L, "mnemonic");
                    lua_pushstring(L, mnemonic_strings[rop_ins->mnemonic]);
                    lua_settable(L, -3);
                    lua_rop_ins_operand_table(L, rop_ins); 
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
    
    return 1;
}


int lua_run_file (char * filename)
{
    int error;
    
    lua_State * L = lua_open();
    luaL_openlibs(L);
    
    lua_pushcfunction(L, lua_make_rop_table);
    lua_setglobal(L, "make_rop_table");
    
    error =luaL_loadfile(L, filename);
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
