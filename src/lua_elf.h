#ifndef lua_elf_HEADER
#define lua_elf_HEADER

#include <lua5.1/lua.h>
#include <lua5.1/lauxlib.h>
#include <lua5.1/lualib.h>

#include "elf.h"

int           lua_open_elf_t  (lua_State * L);
struct _elf * lua_check_elf_t (lua_State * L, int position);
void          lua_push_elf_t  (lua_State * L, struct _elf * i);

int lua_elf_t_new   (lua_State * L);
int lua_elf_t_gc    (lua_State * L);
int lua_elf_t_class (lua_State * L);
int lua_elf_t_shnum (lua_State * L);

#endif
