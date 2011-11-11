#ifndef lua_HEADER
#define lua_HEADER

#include <stdio.h>
#include <string.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <udis86.h>

#include <readline/readline.h>
#include <readline/history.h>

#include "lua_dis.h"
#include "lua_elf.h"
#include "lua_exec.h"
#include "lua_types.h"

int lua_rt_init     (lua_State * L, int argc, char * argv[]);
int lua_interactive (int argc, char * argv[]);
int lua_run_file    (char * filename, int argc, char * argv[]);

#endif
