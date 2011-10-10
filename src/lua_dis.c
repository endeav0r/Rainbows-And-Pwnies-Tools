#include "lua_dis.h"

void lua_operands_table (lua_State * L, ud_t * ud_obj)
{
    int  op_i;
    int_t intt;
    
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
        lua_pushinteger(L, ud_obj->operand[op_i].scale);
        lua_settable(L, -3);
        lua_pushstring(L, "size");
        lua_pushinteger(L, (lua_Integer) ud_obj->operand[op_i].size);
        lua_settable(L, -3);
            
        lua_pushstring(L, "lval");

        switch (ud_obj->operand[op_i].size) {
        case 32 :
            int_t_32_set(&intt, ud_obj->operand[op_i].lval.udword);
            lua_push_int_t(L, &intt);
            break;
        case 8 :
            int_t_8_set(&intt, ud_obj->operand[op_i].lval.ubyte);
            lua_push_int_t(L, &intt);
            break;
        case 16 :
            int_t_16_set(&intt, ud_obj->operand[op_i].lval.uword);
            lua_push_int_t(L, &intt);
            break;
        case 64 :
            int_t_64_set(&intt, ud_obj->operand[op_i].lval.uqword);
            lua_push_int_t(L, &intt);
            break;
        default :
            int_t_32_set(&intt, 0);
            lua_push_int_t(L, &intt);
            break;
        }
        lua_settable(L, -3);
            
        lua_pushinteger(L, (lua_Integer) op_i + 1);
        lua_insert(L, -2);
        lua_settable(L, -3);
    }
}


// returns size of instruction in bytes
int lua_dis_instruction (lua_State * L, unsigned char * data, int mode)
{
    ud_t ud_obj;
    
    ud_init(&ud_obj);
    ud_set_mode(&ud_obj, mode);
    ud_set_syntax(&ud_obj, UD_SYN_INTEL);
    
    ud_set_input_buffer(&ud_obj, data, 32);
    ud_disassemble(&ud_obj);
    
    lua_newtable(L);
    lua_pushstring(L, "mnemonic");
    lua_pushstring(L, mnemonic_strings[ud_obj.mnemonic]);
    lua_settable(L, -3);
    lua_pushstring(L, "description");
    lua_pushstring(L, ud_insn_asm(&ud_obj));
    lua_settable(L, -3);
    lua_pushstring(L, "size");
    lua_pushinteger(L, ud_insn_len(&ud_obj));
    lua_settable(L, -3);
    lua_operands_table(L, &ud_obj);
    lua_pushstring(L, "operands");
    lua_insert(L, -2);
    lua_settable(L, -3);

    return ud_insn_len(&ud_obj);
}


void lua_dis_table (lua_State * L, uint_t * address, 
                    unsigned char * data, int size, int mode)
{
    int ins_i = 1;
    int ins_size;
    int total_size = 0;
    uint_t address_tmp;
    
    // table for all instructions
    lua_newtable(L);
    while (total_size < size) {
        uint_t_set(&address_tmp, address);
        uint_t_add_int(&address_tmp, total_size);
        
        ins_size = lua_dis_instruction(L, &(data[total_size]), mode);
        total_size += ins_size;
        
        lua_pushstring(L, "address");
        lua_push_uint_t(L, &address_tmp);
        lua_settable(L, -3);

        lua_pushinteger(L, (lua_Integer) ins_i);
        lua_insert(L, -2);
        lua_settable(L, -3);
        ins_i++;
    }
}
