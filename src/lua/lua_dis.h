#ifndef lua_dis_HEADER
#define lua_dis_HEADER

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <udis86.h>

#include "lua_types.h"
#include "../strings.h"
#include "../types.h"

void lua_dis_table      (lua_State * L, uint_t * address,
                         unsigned char * data, int size, int mode);
void lua_operands_table (lua_State * L, ud_t * ud_obj);
int lua_dis_instruction (lua_State * L, unsigned char * data, int mode);
#endif
