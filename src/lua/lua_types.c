#include "lua_types.h"

static const struct luaL_Reg int_t_lib_f [] = {
    {"new", lua_int_t_new},
    {NULL, NULL}
};


static const struct luaL_Reg int_t_lib_m [] = {
    {"__tostring", lua_int_t_str},
    {"int",        lua_int_t_int},
    {"uint_t",     lua_int_t_uint_t},
    {"__add",      lua_int_t_add},
    {"__sub",      lua_int_t_sub},
    {"__mul",      lua_int_t_mul},
    {"__div",      lua_int_t_div},
    {"__mod",      lua_int_t_mod},
    {"__eq",       lua_int_t_eq},
    {"__lt",       lua_int_t_lt},
    {"__le",       lua_int_t_le},
    {"strx",       lua_int_t_strx},
    {"str0x",      lua_int_t_strx},
    {"size",       lua_int_t_size},
    {NULL, NULL}
};


static const struct luaL_Reg uint_t_lib_f [] = {
    {"new", lua_uint_t_new},
    {NULL, NULL}
};


static const struct luaL_Reg uint_t_lib_m [] = {
    {"__tostring", lua_uint_t_str},
    {"int",   lua_uint_t_int},
    {"int_t", lua_uint_t_int_t},
    {"__add", lua_uint_t_add},
    {"__sub", lua_uint_t_sub},
    {"__mul", lua_uint_t_mul},
    {"__div", lua_uint_t_div},
    {"__mod", lua_uint_t_mod},
    {"__eq",  lua_uint_t_eq},
    {"__lt",  lua_uint_t_lt},
    {"__le",  lua_uint_t_le},
    {"strx",  lua_uint_t_strx},
    {"str0x", lua_uint_t_str0x},
    {"size",  lua_uint_t_size},
    {NULL, NULL}
};


int lua_open_int_t (lua_State * L)
{
    luaL_newmetatable(L, "rop_tools.int_t");
    
    lua_pushstring(L, "__index");
    lua_pushvalue(L, -2);
    lua_settable(L, -3);
    
    luaL_register(L, NULL, int_t_lib_m);
    luaL_register(L, "int_t", int_t_lib_f);
    return 1;
}


int_t * lua_check_int_t (lua_State * L, int position)
{
    void * userdata = luaL_checkudata(L, position, "rop_tools.int_t");
    luaL_argcheck(L, userdata != NULL, position, "int_t expected");
    return (int_t *) userdata;
}


void lua_push_int_t (lua_State * L, int_t * i)
{
    int_t * new_intt;
    
    lua_pushinteger(L, INT_T_BITS(i));
    lua_pushinteger(L, 0);
    lua_int_t_new(L);
    
    new_intt = lua_check_int_t(L, -1);
    int_t_set(new_intt, i);
}


int lua_int_t_new (lua_State * L)
{
    int bits;
    int value = 0;
    int_t * new_intt;
    
    if (lua_isnumber(L, -2)) {
        bits = luaL_checkinteger(L, -2);
        value = luaL_checkinteger(L, -1);
        lua_pop(L, 2);
    }
    else {
        bits = luaL_checkinteger(L, -1);
        lua_pop(L, 1);
    }

    if (    (bits != 8)
         && (bits != 16)
         && (bits != 32)
         && (bits != 64))
        luaL_error(L, "uint_t accepts bit sizes of 8, 16, 32 or 64");
    
    new_intt = lua_newuserdata(L, sizeof(int_t));
    luaL_getmetatable(L, "rop_tools.int_t");
    lua_setmetatable(L, -2);
    
    switch (bits) {
        case 8  : int_t_8_set (new_intt, value); break;
        case 16 : int_t_16_set(new_intt, value); break;
        case 32 : int_t_32_set(new_intt, value); break;
        case 64 : int_t_64_set(new_intt, value); break;
    }
    
    return 1;
}


int lua_int_t_str (lua_State * L)
{
    int_t * intt;
    
    intt = lua_check_int_t(L, 1);
    lua_pop(L, 1);
    
    lua_pushstring(L, int_t_str(intt));
    
    return 1;
}


int lua_int_t_int (lua_State * L)
{
    int_t * intt;
    
    intt = lua_check_int_t(L, 1);
    lua_pop(L, 1);
    
    lua_pushinteger(L, (lua_Integer) int_t_get(intt));
    
    return 1;
}


int lua_int_t_uint_t (lua_State * L)
{
    int_t * src;
    uint_t dst;
    
    src = lua_check_int_t(L, 1);
    lua_pop(L, 1);
    
    uint_t_int_t(&dst, src);
    lua_push_uint_t(L, &dst);
    
    return 1;
}


int lua_int_t_strx (lua_State * L)
{
    int_t * intt;
    
    intt = lua_check_int_t(L, 1);
    lua_pop(L, 1);
    
    lua_pushstring(L, int_t_strx(intt));
    
    return 1;
}


int lua_int_t_str0x (lua_State * L)
{
    int_t * intt;
    
    intt = lua_check_int_t(L, 1);
    lua_pop(L, 1);
    
    lua_pushstring(L, int_t_str0x(intt));
    
    return 1;
}


int lua_int_t_add (lua_State * L)
{
    int_t * a;
    int_t * b;
    int_t * result;
    int error;
    
    a = lua_check_int_t(L, 1);
    b = lua_check_int_t(L, 2);
    lua_pop(L, 2);
    
    lua_pushinteger(L, 32);
    lua_pushinteger(L, 0);
    lua_int_t_new(L);
    result = lua_check_int_t(L, 1);
    if (a->type >= b->type) {
        int_t_set(result, a);
        error = int_t_add(result, b);
    }
    else {
        int_t_set(result, b);
        error = int_t_add(result, a);
    }
    
    switch (error) {
    case INT_T_TYPE_ERROR : luaL_error(L, "int_t type error. You actually "\
        "shouldn't be getting this and something is wrong with int_t.");
    case INT_T_ERROR : luaL_error(L, "int_t error... this is bad");
    }
    
    return 1;
}


int lua_int_t_sub (lua_State * L)
{
    int_t * a;
    int_t * b;
    int_t * result;
    int error;
    
    a = lua_check_int_t(L, 1);
    b = lua_check_int_t(L, 2);
    lua_pop(L, 2);
    
    lua_pushinteger(L, 32);
    lua_pushinteger(L, 0);
    lua_int_t_new(L);
    result = lua_check_int_t(L, 1);
    int_t_set(result, a);
    error = int_t_sub(result, b);
    
    switch (error) {
    case INT_T_TYPE_ERROR : luaL_error(L, "int_t type error. Are you " \
        "subtracting a larger int_t from a smaller int_t?");
    case INT_T_ERROR : luaL_error(L, "int_t error... this is bad");
    }
    
    return 1;
}


int lua_int_t_mul (lua_State * L)
{
    int_t * a;
    int_t * b;
    int_t * result;
    int error;
    
    a = lua_check_int_t(L, 1);
    b = lua_check_int_t(L, 2);
    lua_pop(L, 2);
    
    lua_pushinteger(L, 32);
    lua_pushinteger(L, 0);
    lua_int_t_new(L);
    result = lua_check_int_t(L, 1);
    if (a->type > b->type) {
        int_t_set(result, a);
        error = int_t_mul(result, b);
    }
    else {
        int_t_set(result, b);
        error = int_t_mul(result, a);
    }
    
    switch (error) {
    case INT_T_TYPE_ERROR : luaL_error(L, "int_t type error. You actually "\
        "shoudln't be getting this and there's something wrong with int_t.");
    case INT_T_ERROR : luaL_error(L, "int_t error... this is bad");
    }
    
    return 1;
}


int lua_int_t_div (lua_State * L)
{
    int_t * a;
    int_t * b;
    int_t * result;
    int error;
    
    a = lua_check_int_t(L, 1);
    b = lua_check_int_t(L, 2);
    lua_pop(L, 2);
    
    lua_pushinteger(L, 32);
    lua_pushinteger(L, 0);
    lua_int_t_new(L);
    result = lua_check_int_t(L, 1);
    int_t_set(result, a);
    error = int_t_div(result, b);
    
    switch (error) {
    case INT_T_TYPE_ERROR : luaL_error(L, "int_t type error. Are you trying "\
        "to divide a smaller int_t by a larger int_t?");
    case INT_T_ERROR : luaL_error(L, "int_t error... this is bad");
    }
    
    return 1;
}


int lua_int_t_mod (lua_State * L)
{
    int_t * a;
    int_t * b;
    int_t * result;
    int error;
    
    a = lua_check_int_t(L, 1);
    b = lua_check_int_t(L, 2);
    lua_pop(L, 2);
    
    lua_pushinteger(L, 32);
    lua_pushinteger(L, 0);
    lua_int_t_new(L);
    result = lua_check_int_t(L, 1);
    int_t_set(result, a);
    error = int_t_mod(result, b);
    
    switch (error) {
    case INT_T_TYPE_ERROR : luaL_error(L, "int_t type error. Are you trying "\
        "to mod a smaller int_t by a larger int_t?");
    case INT_T_ERROR : luaL_error(L, "int_t error... this is bad");
    }
    
    return 1;
}


int lua_int_t_eq (lua_State * L)
{
    int_t * a;
    int_t * b;
    int cmp;
    
    a = lua_check_int_t(L, 1);
    b = lua_check_int_t(L, 2);
    lua_pop(L, 2);
    
    cmp = int_t_cmp(a, b);
    switch (cmp) {
    case 0 :
        lua_pushboolean(L, 1);
        break;
    case -1 :
    case  1 :
        lua_pushboolean(L, 0);
        break;
    case INT_T_TYPE_ERROR :
        luaL_error(L, "tried to compare int_t of different size");
        break;
    case INT_T_ERROR :
        luaL_error(L, "int_t error... this is bad");
        break;
    }
    
    return 1;
}


int lua_int_t_lt (lua_State * L)
{
    int_t * a;
    int_t * b;
    int cmp;
    
    a = lua_check_int_t(L, 1);
    b = lua_check_int_t(L, 2);
    lua_pop(L, 2);
    
    cmp = int_t_cmp(a, b);
    switch (cmp) {
    case -1 :
        lua_pushboolean(L, 1);
        break;
    case 0 :
    case 1 :
        lua_pushboolean(L, 0);
        break;
    case INT_T_TYPE_ERROR :
        luaL_error(L, "tried to compare int_t of different size");
        break;
    case INT_T_ERROR :
        luaL_error(L, "int_t error... this is bad");
        break;
    }
    
    return 1;
}


int lua_int_t_le (lua_State * L)
{
    int_t * a;
    int_t * b;
    int cmp;
    
    a = lua_check_int_t(L, 1);
    b = lua_check_int_t(L, 2);
    lua_pop(L, 2);
    
    cmp = int_t_cmp(a, b);
    switch (cmp) {
    case  0 :
    case -1 :
        lua_pushboolean(L, 1);
        break;
    case 1 :
        lua_pushboolean(L, 0);
        break;
    case INT_T_TYPE_ERROR :
        luaL_error(L, "tried to compare int_t of different size");
        break;
    case INT_T_ERROR :
        luaL_error(L, "int_t error... this is bad");
        break;
    }
    
    return 1;
}


int lua_int_t_size (lua_State * L)
{
    int_t * intt;

    intt = lua_check_int_t(L, -1);
    lua_pop(L, 1);

    lua_pushinteger(L, INT_T_BITS(intt));

    return 1;
}


/********************
* BEGIN UINT_T      *
********************/
int lua_open_uint_t (lua_State * L)
{
    luaL_newmetatable(L, "rop_tools.uint_t");
    
    lua_pushstring(L, "__index");
    lua_pushvalue(L, -2);
    lua_settable(L, -3);
    
    luaL_register(L, NULL, uint_t_lib_m);
    luaL_register(L, "uint_t", uint_t_lib_f);
    return 1;
}


uint_t * lua_check_uint_t (lua_State * L, int position)
{
    void * userdata = luaL_checkudata(L, position, "rop_tools.uint_t");
    luaL_argcheck(L, userdata != NULL, position, "uint_t expected");
    return (uint_t *) userdata;
}


void lua_push_uint_t (lua_State * L, uint_t * u)
{
    uint_t * new_uintt;
    
    lua_pushinteger(L, UINT_T_BITS(u));
    lua_pushinteger(L, 0);
    lua_uint_t_new(L);
    
    new_uintt = lua_check_uint_t(L, -1);
    uint_t_set(new_uintt, u);
}


int lua_uint_t_new (lua_State * L)
{
    int bits;
    int value = 0;
    uint_t * new_uintt;
    
    if (lua_isnumber(L, -2)) {
        bits = luaL_checkinteger(L, -2);
        value = luaL_checkinteger(L, -1);
        lua_pop(L, 2);
    }
    else {
        bits = luaL_checkinteger(L, -1);
        lua_pop(L, 1);
    }

    if (    (bits != 8)
         && (bits != 16)
         && (bits != 32)
         && (bits != 64))
        luaL_error(L, "uint_t accepts bit sizes of 8, 16, 32 or 64");
    
    new_uintt = lua_newuserdata(L, sizeof(uint_t));
    luaL_getmetatable(L, "rop_tools.uint_t");
    lua_setmetatable(L, -2);
    
    switch (bits) {
        case 8 :  uint_t_8_set (new_uintt, value); break;
        case 16 : uint_t_16_set(new_uintt, value); break;
        case 32 : uint_t_32_set(new_uintt, value); break;
        case 64 : uint_t_64_set(new_uintt, value); break;
    }
    
    return 1;
}


int lua_uint_t_str (lua_State * L)
{
    uint_t * uintt;
    
    uintt = lua_check_uint_t(L, 1);
    lua_pop(L, 1);
    
    lua_pushstring(L, uint_t_str(uintt));
    
    return 1;
}


int lua_uint_t_int (lua_State * L)
{
    uint_t * uintt;
    
    uintt = lua_check_uint_t(L, 1);
    lua_pop(L, 1);
    
    lua_pushinteger(L, (lua_Integer) uint_t_get(uintt));
    
    return 1;
}


int lua_uint_t_int_t (lua_State * L)
{
    uint_t * src;
    int_t dst;
    
    src = lua_check_uint_t(L, 1);
    lua_pop(L, 1);
    
    int_t_uint_t(&dst, src);
    lua_push_int_t(L, &dst);
    
    return 1;
}


int lua_uint_t_strx (lua_State * L)
{
    uint_t * uintt;
    
    uintt = lua_check_uint_t(L, 1);
    lua_pop(L, 1);
    
    lua_pushstring(L, uint_t_strx(uintt));
    
    return 1;
}


int lua_uint_t_str0x (lua_State * L)
{
    uint_t * uintt;
    
    uintt = lua_check_uint_t(L, 1);
    lua_pop(L, 1);
    
    lua_pushstring(L, uint_t_str0x(uintt));
    
    return 1;
}


int lua_uint_t_add (lua_State * L)
{
    uint_t * a;
    uint_t * b;
    uint_t * result;
    int error;
    
    a = lua_check_uint_t(L, 1);
    b = lua_check_uint_t(L, 2);
    lua_pop(L, 2);
    
    lua_pushinteger(L, 32);
    lua_pushinteger(L, 0);
    lua_uint_t_new(L);
    result = lua_check_uint_t(L, 1);
    if (a->type >= b->type) {
        uint_t_set(result, a);
        error = uint_t_add(result, b);
    }
    else {
        uint_t_set(result, b);
        error = uint_t_add(result, a);
    }
    
    switch (error) {
    case UINT_T_TYPE_ERROR : luaL_error(L, "uint_t type error. You actually "\
        "shouldn't be getting this and something is wrong with uint_t.");
    case UINT_T_ERROR : luaL_error(L, "uint_t error... this is bad");
    }
    
    return 1;
}


int lua_uint_t_sub (lua_State * L)
{
    uint_t * a;
    uint_t * b;
    uint_t * result;
    int error;
    
    a = lua_check_uint_t(L, 1);
    b = lua_check_uint_t(L, 2);
    lua_pop(L, 2);
    
    lua_pushinteger(L, 32);
    lua_pushinteger(L, 0);
    lua_uint_t_new(L);
    result = lua_check_uint_t(L, 1);
    uint_t_set(result, a);
    error = uint_t_sub(result, b);
    
    switch (error) {
    case UINT_T_TYPE_ERROR : luaL_error(L, "uint_t type error. Are you " \
        "subtracting a larger uint_t from a smaller uint_t?");
    case UINT_T_ERROR : luaL_error(L, "uint_t error... this is bad");
    }
    
    return 1;
}


int lua_uint_t_mul (lua_State * L)
{
    uint_t * a;
    uint_t * b;
    uint_t * result;
    int error;
    
    a = lua_check_uint_t(L, 1);
    b = lua_check_uint_t(L, 2);
    lua_pop(L, 2);
    
    lua_pushinteger(L, 32);
    lua_pushinteger(L, 0);
    lua_uint_t_new(L);
    result = lua_check_uint_t(L, 1);
    if (a->type > b->type) {
        uint_t_set(result, a);
        error = uint_t_mul(result, b);
    }
    else {
        uint_t_set(result, b);
        error = uint_t_mul(result, a);
    }
    
    switch (error) {
    case UINT_T_TYPE_ERROR : luaL_error(L, "uint_t type error. You actually "\
        "shoudln't be getting this and there's something wrong with uint_t.");
    case UINT_T_ERROR : luaL_error(L, "uint_t error... this is bad");
    }
    
    return 1;
}


int lua_uint_t_div (lua_State * L)
{
    uint_t * a;
    uint_t * b;
    uint_t * result;
    int error;
    
    a = lua_check_uint_t(L, 1);
    b = lua_check_uint_t(L, 2);
    lua_pop(L, 2);
    
    lua_pushinteger(L, 32);
    lua_pushinteger(L, 0);
    lua_uint_t_new(L);
    result = lua_check_uint_t(L, 1);
    uint_t_set(result, a);
    error = uint_t_div(result, b);
    
    switch (error) {
    case UINT_T_TYPE_ERROR : luaL_error(L, "uint_t type error. Are you trying "\
        "to divide a smaller uint_t by a larger uint_t?");
    case UINT_T_ERROR : luaL_error(L, "uint_t error... this is bad");
    }
    
    return 1;
}


int lua_uint_t_mod (lua_State * L)
{
    uint_t * a;
    uint_t * b;
    uint_t * result;
    int error;
    
    a = lua_check_uint_t(L, 1);
    b = lua_check_uint_t(L, 2);
    lua_pop(L, 2);
    
    lua_pushinteger(L, 32);
    lua_pushinteger(L, 0);
    lua_uint_t_new(L);
    result = lua_check_uint_t(L, 1);
    uint_t_set(result, a);
    error = uint_t_mod(result, b);
    
    switch (error) {
    case UINT_T_TYPE_ERROR : luaL_error(L, "uint_t type error. Are you trying "\
        "to mod a smaller uint_t by a larger uint_t?");
    case UINT_T_ERROR : luaL_error(L, "uint_t error... this is bad");
    }
    
    return 1;
}


int lua_uint_t_eq (lua_State * L)
{
    uint_t * a;
    uint_t * b;
    int cmp;
    
    a = lua_check_uint_t(L, 1);
    b = lua_check_uint_t(L, 2);
    lua_pop(L, 2);
    
    cmp = uint_t_cmp(a, b);
    switch (cmp) {
    case 0 :
        lua_pushboolean(L, 1);
        break;
    case -1 :
    case  1 :
        lua_pushboolean(L, 0);
        break;
    case UINT_T_TYPE_ERROR :
        luaL_error(L, "tried to compare uint_t of different size");
        break;
    case UINT_T_ERROR :
        luaL_error(L, "uint_t error... this is bad");
        break;
    }
    
    return 1;
}


int lua_uint_t_lt (lua_State * L)
{
    uint_t * a;
    uint_t * b;
    int cmp;
    
    a = lua_check_uint_t(L, 1);
    b = lua_check_uint_t(L, 2);
    lua_pop(L, 2);
    
    cmp = uint_t_cmp(a, b);
    switch (cmp) {
    case -1 :
        lua_pushboolean(L, 1);
        break;
    case 0 :
    case 1 :
        lua_pushboolean(L, 0);
        break;
    case UINT_T_TYPE_ERROR :
        luaL_error(L, "tried to compare uint_t of different size");
        break;
    case UINT_T_ERROR :
        luaL_error(L, "uint_t error... this is bad");
        break;
    }
    
    return 1;
}


int lua_uint_t_le (lua_State * L)
{
    uint_t * a;
    uint_t * b;
    int cmp;
    
    a = lua_check_uint_t(L, 1);
    b = lua_check_uint_t(L, 2);
    lua_pop(L, 2);
    
    cmp = uint_t_cmp(a, b);
    switch (cmp) {
    case  0 :
    case -1 :
        lua_pushboolean(L, 1);
        break;
    case 1 :
        lua_pushboolean(L, 0);
        break;
    case UINT_T_TYPE_ERROR :
        luaL_error(L, "tried to compare uint_t of different size");
        break;
    case UINT_T_ERROR :
        luaL_error(L, "uint_t error... this is bad");
        break;
    }
    
    return 1;
}


int lua_uint_t_size (lua_State * L)
{
    uint_t * uintt;

    uintt = lua_check_uint_t(L, -1);
    lua_pop(L, 1);

    lua_pushinteger(L, UINT_T_BITS(uintt));

    return 1;
}
