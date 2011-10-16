#ifndef lua_HEADER
#define lua_HEADER

#include <stdio.h>
#include <string.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <udis86.h>

#include "lua_dis.h"
#include "lua_elf.h"
#include "lua_exec.h"
#include "lua_types.h"

int lua_run_file (char * filename, char * args[], int argc);

#endif
