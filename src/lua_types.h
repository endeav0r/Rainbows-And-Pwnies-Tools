#ifndef lua_types_HEADER
#define lua_types_HEADER

#include <lua5.1/lua.h>
#include <lua5.1/lauxlib.h>
#include <lua5.1/lualib.h>
#include <string.h>

#include "types.h"

int     lua_open_int_t  (lua_State * L);
int_t * lua_check_int_t (lua_State * L, int position);
void    lua_push_int_t  (lua_State * L, int_t * i);

int lua_int_t_new  (lua_State * L);
int lua_int_t_str  (lua_State * L);
int lua_int_t_strx (lua_State * L);
int lua_int_t_add  (lua_State * L);
int lua_int_t_sub  (lua_State * L);
int lua_int_t_mul  (lua_State * L);
int lua_int_t_div  (lua_State * L);
int lua_int_t_mod  (lua_State * L);
int lua_int_t_eq   (lua_State * L);
int lua_int_t_lt   (lua_State * L);
int lua_int_t_le   (lua_State * L);

int      lua_open_uint_t  (lua_State * L);
uint_t * lua_check_uint_t (lua_State * L, int position);
void     lua_push_uint_t  (lua_State * L, uint_t * u);

int lua_uint_t_new  (lua_State * L);
int lua_uint_t_str  (lua_State * L);
int lua_uint_t_strx (lua_State * L);
int lua_uint_t_add  (lua_State * L);
int lua_uint_t_sub  (lua_State * L);
int lua_uint_t_mul  (lua_State * L);
int lua_uint_t_div  (lua_State * L);
int lua_uint_t_mod  (lua_State * L);
int lua_uint_t_eq   (lua_State * L);
int lua_uint_t_lt   (lua_State * L);
int lua_uint_t_le   (lua_State * L);

#endif
