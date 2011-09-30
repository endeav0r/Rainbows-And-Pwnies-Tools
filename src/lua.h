#ifndef lua_HEADER
#define lua_HEADER

#include <stdio.h>
#include <string.h>
#include <lua5.1/lua.h>
#include <lua5.1/lauxlib.h>
#include <lua5.1/lualib.h>
#include <udis86.h>

#include "aux.h"
#include "elf.h"
#include "lua_dis.h"
#include "lua_elf.h"
#include "lua_types.h"
#include "rop.h"
#include "strings.h"
#include "types.h"

int lua_run_file (char * filename, char * args[], int argc);

#endif
