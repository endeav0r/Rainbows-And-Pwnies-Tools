#ifndef lua_exec_HEADER
#define lua_exec_HEADER

#include <lua5.1/lua.h>
#include <lua5.1/lauxlib.h>
#include <lua5.1/lualib.h>

#include "analyze.h"
#include "exec.h"
#include "lua_dis.h"
#include "lua_types.h"
#include "rop.h"
#include "strings.h"

// see the giant comment in lua_elf.h for why we garbage collect these on our
// own

struct lua_exec_t {
    int ref_count;
    struct _exec * exec;
};

struct lua_exec_section_t {
    int ref_count;
    struct lua_exec_t * exec_t;
    struct _exec_section section;
};

struct lua_exec_symbol_t {
    int ref_count;
    struct lua_exec_t * exec_t;
    struct _exec_symbol symbol;
};

int                 lua_open_exec_t    (lua_State * L);
struct lua_exec_t * lua_check_exec_t   (lua_State * L, int position);
struct _exec *      lua_check_exec     (lua_State * L, int position);
void                lua_push_exec      (lua_State * L, struct _exec * exec);
void                lua_exec_t_collect (struct lua_exec_t * exec_t);

int lua_exec_t_new          (lua_State * L);
int lua_exec_t_gc           (lua_State * L);
int lua_exec_num_sections   (lua_State * L);
int lua_exec_size           (lua_State * L);
int lua_exec_type           (lua_State * L);
int lua_exec_section        (lua_State * L);
int lua_exec_sections       (lua_State * L);
int lua_exec_find_functions (lua_State * L);

struct lua_exec_section_t * lua_check_exec_section_t (lua_State * L, int position);
struct _exec_section *      lua_check_exec_section (lua_State * L, int position);
void lua_exec_section_t_collect (struct lua_exec_section_t * section_t);
// the lua_exec_section_t will be empty and needs to be filled with valid fields
// from a lua_exec_t inside lua
// ref_count for the section will be set to 1
void lua_push_exec_section_t (lua_State * L);

int lua_exec_section_t_gc        (lua_State * L);
int lua_exec_section_name        (lua_State * L);
int lua_exec_section_address     (lua_State * L);
int lua_exec_section_types       (lua_State * L);
int lua_exec_section_size        (lua_State * L);
int lua_exec_section_disassemble (lua_State * L);

struct lua_exec_symbol_t * lua_check_exec_symbol_t (lua_State * L, int position);
struct _exec_symbol *      lua_check_exec_symbol (lua_State * L, int position);
void lua_exec_symbol_t_collect (struct lua_exec_symbol_t * symbol_t);
// same as lua_push_exec_section_t but for symbols :p
void lua_push_exec_symbol_t (lua_State * L);

int lua_exec_symbol_t_gc    (lua_State * L);
int lua_exec_symbol_name    (lua_State * L);
int lua_exec_symbol_address (lua_State * L);
int lua_exec_symbol_type    (lua_State * L);

#endif
