#include "lua.h"

void lua_operands_table (lua_State * L, unsigned char * data, int size, int mode)
{
    int  op_i;
    ud_t ud_obj;

    // table for operands
    lua_newtable(L);

    ud_init(&ud_obj);
    ud_set_mode(&ud_obj, mode);
    ud_set_input_buffer(&ud_obj, data, size);
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
}


void lua_dis_table (lua_State * L, uint_t * address, 
                    unsigned char * data, int size, int mode)
{
    int ins_i = 1;
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
        lua_pushinteger(L, (lua_Integer) uint_t_get(address) 
                                         + ud_insn_off(&ud_obj));
        lua_settable(L, -3);
        lua_pushstring(L, "mnemonic");
        lua_pushstring(L, mnemonic_strings[ud_obj.mnemonic]);
        lua_settable(L, -3);
        lua_pushstring(L, "description");
        lua_pushstring(L, ud_insn_asm(&ud_obj));
        lua_settable(L, -3);
        lua_operands_table(L, ud_insn_ptr(&ud_obj), ud_insn_len(&ud_obj), mode);
        lua_pushstring(L, "operands");
        lua_insert(L, -2);
        lua_settable(L, -3);
        lua_pushinteger(L, (lua_Integer) ins_i);
        lua_insert(L, -2);
        lua_settable(L, -3);
        ins_i++;
    }
}


static int lua_dis_by_function (lua_State * L)
{
    struct _elf * elf;
    struct _elf_shdr shdr;
    struct _elf_shdr text_shdr;
    struct _elf_sym sym;
    struct _sym_list * sym_list;
    struct _sym_list * sym_list_first;
    int mode;
    uint_t data_offset;
    unsigned char * data;
    char * filename;

    filename  = (char *) luaL_checkstring(L, 1);
    lua_pop(L, 1);
    
    elf = elf_open(filename);
    switch (elf_class(elf)) {
    case ELFCLASS32 : mode = 32; break;
    case ELFCLASS64 : mode = 64; break;
    }
    
    sym_list_first = aux_func_syms(elf);
    sym_list = sym_list_first;
    
    lua_newtable(L);
    while (sym_list != NULL) {
        elf_shdr(elf, &shdr, sym_list->shdr_i);
        shdr_sym(&shdr, &sym, sym_list->sym_i);
        elf_shdr(elf, &text_shdr, uint_t_get(sym_shndx(&sym)));
        uint_t_set(&data_offset, sym_value(&sym));
        uint_t_sub(&data_offset, shdr_addr(&text_shdr));
        data = shdr_data(&text_shdr);
        data = &(data[uint_t_get(&data_offset)]);
        lua_dis_table(L, sym_value(&sym), data, int_t_get(sym_size(&sym)), mode);
        lua_pushstring(L, sym_name(&sym));
        lua_insert(L, -2);
        lua_settable(L, -3);
        sym_list = sym_list->next;
    }
    
    sym_list_destroy(sym_list_first);
    elf_destroy(elf);
    
    return 1;
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


int lua_elf_open_shdr_syms (lua_State * L, struct _elf_shdr * shdr)
{
    struct _elf_sym sym;
    int sym_i;
    
    // table for all symbols
    lua_newtable(L);
    for (sym_i = 0; sym_i < shdr_num(shdr); sym_i++) {
        shdr_sym(shdr, &sym, sym_i);
        // table for this symbol
        lua_newtable(L);
        
        lua_pushstring(L, "address");
        lua_pushinteger(L, (lua_Integer) uint_t_get(sym_value(&sym)));
        lua_settable(L, -3);
        
        lua_pushstring(L, "type");
        if (sym_type(&sym) < SYM_TYPE_STRINGS_SIZE)
            lua_pushstring(L, sym_type_strings[sym_type(&sym)]);
        else
            lua_pushstring(L, "undefined");
        lua_settable(L, -3);
        
        if (sym_name(&sym) == NULL)
            lua_pushinteger(L, (lua_Integer) sym_i + 1);
        else
            lua_pushstring(L, sym_name(&sym));
        lua_insert(L, -2);
        lua_settable(L, -3);
    }
    
    return sym_i;
}


int lua_elf_open_shdr_rels (lua_State * L, struct _elf_shdr * shdr)
{
    struct _elf_rel rel;
    int rel_i;
    
    // table for all relocations
    lua_newtable(L);
    for (rel_i = 0; rel_i < shdr_num(shdr); rel_i++) {
        shdr_rel(shdr, &rel, rel_i);
        // table for this relocation
        lua_newtable(L);
        
        lua_pushstring(L, "type");
        lua_pushinteger(L, (lua_Integer) rel_type(&rel));
        lua_settable(L, -3);
        
        lua_pushstring(L, "offset");
        lua_pushinteger(L, (lua_Integer) uint_t_get(rel_offset(&rel)));
        lua_settable(L, -3);
        
        lua_pushstring(L, rel_name(&rel));
        lua_insert(L, -2);
        lua_settable(L, -3);
    }
    
    return rel_i;
}


int lua_elf_open_shdrs (lua_State * L, struct _elf * elf)
{
    struct _elf_shdr shdr;
    int shdr_i;
    
    // table for all shdrs
    lua_newtable(L);
    for (shdr_i = 0; shdr_i < int_t_get(elf_shnum(elf)); shdr_i++) {
        elf_shdr(elf, &shdr, shdr_i);
        // create table for this shdr
        lua_newtable(L);
        
        lua_pushstring(L, "address");
        lua_pushinteger(L, (lua_Integer) uint_t_get(shdr_addr(&shdr)));
        lua_settable(L, -3);
        
        lua_pushstring(L, "executable");
        lua_pushboolean(L, shdr_exec(&shdr));
        lua_settable(L, -3);
        
        lua_pushstring(L, "size");
        lua_pushinteger(L, (lua_Integer) int_t_get(shdr_size(&shdr)));
        lua_settable(L, -3);
        
        lua_pushstring(L, "type");
        if (int_t_get(shdr_type(&shdr)) > SHDR_TYPE_STRINGS_SIZE)
            lua_pushstring(L, "unknown");
        else
            lua_pushstring(L, shdr_type_strings[int_t_get(shdr_type(&shdr))]);
        lua_settable(L, -3);
        
        // is this a symbol table? well then let's LOAD SOME FUCKING SYMBOLS
        if (    (int_t_get(shdr_type(&shdr)) == SHT_SYMTAB)
             || (int_t_get(shdr_type(&shdr)) == SHT_DYNSYM))
            lua_elf_open_shdr_syms(L, &shdr);
        else
            lua_pushnil(L);
        lua_pushstring(L, "symbols");
        lua_insert(L, -2);
        lua_settable(L, -3);
        
        if (    (int_t_get(shdr_type(&shdr)) == SHT_REL)
             || (int_t_get(shdr_type(&shdr)) == SHT_RELA))
            lua_elf_open_shdr_rels(L, &shdr);
        else
            lua_pushnil(L);
        lua_pushstring(L, "relocations");
        lua_insert(L, -2);
        lua_settable(L, -3);
        
        if (shdr_name(&shdr) == NULL)
            lua_pushinteger(L, (lua_Integer) shdr_i + 1);
        else
            lua_pushstring(L, shdr_name(&shdr));
        lua_insert(L, -2);
        lua_settable(L, -3);
    }
    
    return shdr_i;
}


static int lua_elf_open (lua_State * L)
{
    char * filename;
    struct _elf * elf;
    
    filename  = (char *) luaL_checkstring(L, 1);
    lua_pop(L, 1);
    
    elf = elf_open(filename);
    if (elf == NULL)
        return luaL_error(L, "Failed opening elf %s\n", filename);
    
    lua_newtable(L);
    lua_pushstring(L, "shnum");
    lua_pushinteger(L, (lua_Integer) elf_shnum(elf));
    lua_settable(L, -3);
    
    /* set info for section header */
    lua_elf_open_shdrs(L, elf);
    lua_pushstring(L, "sections");
    lua_insert(L, -2);
    lua_settable(L, -3);

    elf_destroy(elf);

    return 1;
}


int lua_run_file (char * filename)
{
    int error;
    
    lua_State * L = lua_open();
    luaL_openlibs(L);
    
    lua_pushcfunction(L, lua_make_rop_table);
    lua_setglobal(L, "make_rop_table");
    lua_pushcfunction(L, lua_elf_open);
    lua_setglobal(L, "elf_open");
    lua_pushcfunction(L, lua_dis_by_function);
    lua_setglobal(L, "dis_by_function");
    
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
