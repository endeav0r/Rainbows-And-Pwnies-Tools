#ifndef lua_HEADER
#define lua_HEADER

#include <stdio.h>
#include <string.h>
#include <lua5.1/lua.h>
#include <lua5.1/lauxlib.h>
#include <lua5.1/lualib.h>

#include "elf.h"
#include "mnemonic_strings.h"
#include "rop.h"

int lua_run_file (char * filename);

//static int lua_make_rop_table (lua_State * L);

#endif
