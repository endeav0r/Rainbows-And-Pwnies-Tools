#include "lua_dis.h"

void lua_operands_table (lua_State * L, ud_t * ud_obj)
{
    int  op_i;
    
    // table for operands
    lua_newtable(L);
    
    for (op_i = 0; op_i < 3; op_i++) {
        // table for this operand
        lua_newtable(L);
        lua_pushstring(L, "type");
        lua_pushstring(L, types_strings[ud_obj->operand[op_i].type]);
        lua_settable(L, -3);
        lua_pushstring(L, "base");
        lua_pushstring(L, types_strings[ud_obj->operand[op_i].base]);
        lua_settable(L, -3);
        lua_pushstring(L, "index");
        lua_pushstring(L, types_strings[ud_obj->operand[op_i].index]);
        lua_settable(L, -3);
        lua_pushstring(L, "offset");
        lua_pushstring(L, types_strings[ud_obj->operand[op_i].offset]);
        lua_settable(L, -3);
        lua_pushstring(L, "scale");
        lua_pushstring(L, types_strings[ud_obj->operand[op_i].scale]);
        lua_settable(L, -3);
        lua_pushstring(L, "size");
        lua_pushinteger(L, (lua_Integer) ud_obj->operand[op_i].size);
        lua_settable(L, -3);
            
        lua_pushstring(L, "lval");
        if (    (ud_obj->operand[op_i].type == UD_OP_IMM)
             || (ud_obj->operand[op_i].offset)) {
            switch (ud_obj->operand[op_i].size) {
            case 32 :
                lua_pushinteger(L, (lua_Integer) ud_obj->operand[op_i].lval.udword);
                break;
            case 8 :
                lua_pushinteger(L, (lua_Integer) ud_obj->operand[op_i].lval.sbyte);
                break;
            case 16 :
                lua_pushinteger(L, (lua_Integer) ud_obj->operand[op_i].lval.uword);
                break;
            case 64 :
                lua_pushinteger(L, (lua_Integer) ud_obj->operand[op_i].lval.uqword);
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
}


void lua_dis_table (lua_State * L, uint_t * address, 
                    unsigned char * data, int size, int mode)
{
    int ins_i = 1;
    uint_t address_tmp;
    ud_t ud_obj;
    
    ud_init(&ud_obj);
    ud_set_mode(&ud_obj, mode);
    ud_set_syntax(&ud_obj, UD_SYN_INTEL);
    
    ud_set_input_buffer(&ud_obj, data, size);
    // table for all instructions
    lua_newtable(L);
    while (ud_disassemble(&ud_obj)) {
        // table for this instruction
        lua_newtable(L);
        lua_pushstring(L, "address");
        uint_t_set(&address_tmp, address);
        uint_t_add_int(&address_tmp, ud_insn_off(&ud_obj));
        lua_push_uint_t(L, &address_tmp);
        lua_settable(L, -3);
        lua_pushstring(L, "mnemonic");
        lua_pushstring(L, mnemonic_strings[ud_obj.mnemonic]);
        lua_settable(L, -3);
        lua_pushstring(L, "description");
        lua_pushstring(L, ud_insn_asm(&ud_obj));
        lua_settable(L, -3);
        lua_operands_table(L, &ud_obj);
        lua_pushstring(L, "operands");
        lua_insert(L, -2);
        lua_settable(L, -3);
        lua_pushinteger(L, (lua_Integer) ins_i);
        lua_insert(L, -2);
        lua_settable(L, -3);
        ins_i++;
    }
}
