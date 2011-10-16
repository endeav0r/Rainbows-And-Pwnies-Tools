#ifndef lua_elf_HEADER
#define lua_elf_HEADER

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "../bin/elf.h"
#include "lua_dis.h"
#include "lua_types.h"
#include "../rop.h"
#include "../strings.h"

/*
    Because lua does automatic garbage collection and our elf library has
    everything maintain pointers to struct * _elf, we need to do do some
    manual garbage-collection trickery ourselves.
    
    We're not going to slow down our elf library by requiring mallocing memory
    for objects, but we're going to make our lua library as safe as possible.
    
    Therefore, we wrap our elf library objects and garbage collect ourselves. We
    maintain the speed of our elf library, keep our lua library safe, and
    separate ourself from the hazardous part of lua garbage collection we can't
    control (and may change in future versions)
    
    We'll let lua's userdata be pointers. Lua will free the pointers. We'll
    hook the garbage collection methods and free the structs ourselves as
    needed.
*/

struct lua_elf_t {
    int ref_count;
    struct _elf * elf;
};

struct lua_elf_section_t {
    int ref_count;
    struct lua_elf_t * elf_t;
    struct _elf_section section;
};

struct lua_elf_symbol_t {
    int ref_count;
    struct lua_elf_section_t * section_t;
    struct _elf_symbol symbol;
};

struct lua_elf_relocation_t {
    int ref_count;
    struct lua_elf_section_t * section_t;
    struct _elf_relocation relocation;
};


int                lua_open_elf_t    (lua_State * L);

void               lua_elf_t_collect (struct lua_elf_t * elf_t);
struct _elf *      lua_check_elf     (lua_State * L, int position);
struct lua_elf_t * lua_check_elf_t   (lua_State * L, int position);
void               lua_push_elf      (lua_State * L, struct _elf * elf);
int lua_elf_t_new            (lua_State * L);
int lua_elf_t_gc             (lua_State * L);
int lua_elf_t_class          (lua_State * L);
int lua_elf_t_filename       (lua_State * L);
int lua_elf_t_shnum          (lua_State * L);
int lua_elf_t_section        (lua_State * L);
int lua_elf_t_section_exists (lua_State * L);
int lua_elf_t_symbols        (lua_State * L);


void           lua_elf_section_t_collect (struct lua_elf_section_t * section_t);
int                        lua_open_elf_section_t    (lua_State * L);
struct lua_elf_section_t * lua_check_elf_section_t   (lua_State * L, int position);
struct _elf_section *      lua_check_elf_section     (lua_State * L, int position);
// the lua_section_t will be empty and needs to be filled with valid fields from
// a lua_elf_t inside lua
// ref_count for the section will be set to 1
void lua_push_elf_section_t (lua_State * L);

int lua_elf_section_t_gc             (lua_State * L);
int lua_elf_section_t_name           (lua_State * L);
int lua_elf_section_t_address        (lua_State * L);
int lua_elf_section_t_exec           (lua_State * L);
int lua_elf_section_t_size           (lua_State * L);
int lua_elf_section_t_offset         (lua_State * L);
int lua_elf_section_t_type           (lua_State * L);
int lua_elf_section_t_num            (lua_State * L);
int lua_elf_section_t_entsize        (lua_State * L);
int lua_elf_section_t_link           (lua_State * L);
int lua_elf_section_t_symbol         (lua_State * L);
int lua_elf_section_t_symbols        (lua_State * L);
int lua_elf_section_t_relocation     (lua_State * L);
int lua_elf_section_t_relocations    (lua_State * L);
int lua_elf_section_t_disassemble    (lua_State * L);
int lua_elf_section_t_rop_table      (lua_State * L);
int lua_elf_section_t_mem_at_address (lua_State * L);

void          lua_elf_symbol_t_collect (struct lua_elf_symbol_t * symbol_t);
struct _elf_symbol *      lua_check_elf_symbol     (lua_State * L, int position);
struct lua_elf_symbol_t * lua_check_elf_symbol_t   (lua_State * L, int position);
int                       lua_elf_symbol_t_gc      (lua_State * L);
// like lua_section_t, contents need to be filled. sets ref_count = 1
void                      lua_push_elf_symbol_t    (lua_State * L);

int lua_elf_symbol_t_name        (lua_State * L);
int lua_elf_symbol_t_value       (lua_State * L);
int lua_elf_symbol_t_type        (lua_State * L);
int lua_elf_symbol_t_size        (lua_State * L);
int lua_elf_symbol_t_disassemble (lua_State * L);


void  lua_elf_relocation_t_collect (struct lua_elf_relocation_t * relocation_t);
struct _elf_relocation *      lua_check_elf_relocation   (lua_State * L, int position);
struct lua_elf_relocation_t * lua_check_elf_relocation_t (lua_State * L, int position);
void                          lua_push_elf_relocation_t  (lua_State * L);
int                           lua_elf_relocation_t_gc    (lua_State * L);

int lua_elf_relocation_t_name   (lua_State * L);
int lua_elf_relocation_t_offset (lua_State * L);
int lua_elf_relocation_t_type   (lua_State * L);
#endif
