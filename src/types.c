#include "types.h"

void int_t_16_set (int_t * intt, int16_t i)
{
    intt->ints.int16 = i;
    intt->type = INT_T_16;
}


void int_t_32_set (int_t * intt, int32_t i)
{
    intt->ints.int32 = i;
    intt->type = INT_T_32;
}


void int_t_64_set (int_t * intt, int64_t i)
{
    intt->ints.int64 = i;
    intt->type = INT_T_64;
}


int16_t int_t_16_get (int_t * intt) { return intt->ints.int16; }
int32_t int_t_32_get (int_t * intt) { return intt->ints.int32; }
int64_t int_t_64_get (int_t * intt) { return intt->ints.int64; }


int int_t_get (int_t * intt)
{
    switch (intt->type) {
    case INT_T_16 :
        return (int) intt->ints.int16;
    case INT_T_32 :
        return (int) intt->ints.int32;
    case INT_T_64 :
        return (int) intt->ints.int64;
    }
    return 0;
}


char * int_t_str (int_t * intt)
{
    switch (intt->type) {
    case INT_T_16 :
        snprintf(intt->string, INT_STRLEN, "%d", intt->ints.int16);
        break;
    case INT_T_32 :
        snprintf(intt->string, INT_STRLEN, "%d", intt->ints.int32);
        break;
    case INT_T_64 :
        snprintf(intt->string, INT_STRLEN,
                 "%lld", (long long int) intt->ints.int64);
        break;
    default :
        snprintf(intt->string, INT_STRLEN, "INVALID_INT_T");
    }
    
    return intt->string;
}


char * int_t_strx (int_t * intt)
{
    switch (intt->type) {
    case INT_T_16 :
        snprintf(intt->string, INT_STRLEN, "%04x", intt->ints.int16);
        break;
    case INT_T_32 :
        snprintf(intt->string, INT_STRLEN, "%08x", intt->ints.int32);
        break;
    case INT_T_64 :
        snprintf(intt->string, INT_STRLEN,
                 "%16llx", (long long int) intt->ints.int64);
        break;
    default :
        snprintf(intt->string, INT_STRLEN, "INVALID_INT_T");
    }
    
    return intt->string;
}




void uint_t_16_set (uint_t * uintt, uint16_t i)
{
    uintt->uints.uint16 = i;
    uintt->type = UINT_T_16;
}


void uint_t_32_set (uint_t * uintt, uint32_t i)
{
    uintt->uints.uint32 = i;
    uintt->type = UINT_T_32;
}


void uint_t_64_set (uint_t * uintt, uint64_t i)
{
    uintt->uints.uint64 = i;
    uintt->type = UINT_T_64;
}


void uint_t_set (uint_t * dst, uint_t * src)
{
    switch (src->type) {
    case UINT_T_16 :
        uint_t_16_set(dst, src->uints.uint16);
        break;
    case UINT_T_32 :
        uint_t_32_set(dst, src->uints.uint32);
        break;
    case UINT_T_64 :
        uint_t_64_set(dst, src->uints.uint64);
        break;
    }
}


uint16_t uint_t_16_get (uint_t * uintt) { return uintt->uints.uint16; }
uint32_t uint_t_32_get (uint_t * uintt) { return uintt->uints.uint32; }
uint64_t uint_t_64_get (uint_t * uintt) { return uintt->uints.uint64; }


unsigned int uint_t_get (uint_t * uintt)
{
    switch (uintt->type) {
    case UINT_T_16 :
        return (unsigned int) uintt->uints.uint16 & 0x0000ffff;
    case UINT_T_32 :
        return (unsigned int) uintt->uints.uint32;
    case UINT_T_64 :
        return (unsigned int) uintt->uints.uint64;
    }
    return 0;
}


char * uint_t_str (uint_t * uintt)
{
    switch (uintt->type) {
    case UINT_T_16 :
        snprintf(uintt->string, INT_STRLEN, "%d", uintt->uints.uint16);
        break;
    case UINT_T_32 :
        snprintf(uintt->string, INT_STRLEN, "%d", uintt->uints.uint32);
        break;
    case UINT_T_64 :
        snprintf(uintt->string, INT_STRLEN, 
                 "%llu", (unsigned long long int) uintt->uints.uint64);
        break;
    default :
        snprintf(uintt->string, INT_STRLEN, "INVALID_uint_T");
    }
    
    return uintt->string;
}


char * uint_t_strx (uint_t * uintt)
{
    switch (uintt->type) {
    case UINT_T_16 :
        snprintf(uintt->string, INT_STRLEN, "%04x", uintt->uints.uint16);
        break;
    case UINT_T_32 :
        snprintf(uintt->string, INT_STRLEN, "%08x", uintt->uints.uint32);
        break;
    case UINT_T_64 :
        snprintf(uintt->string, INT_STRLEN,
                 "%016llx", (unsigned long long int) uintt->uints.uint64);
        break;
    default :
        snprintf(uintt->string, INT_STRLEN, "INVALID_uint_T");
    }
    
    return uintt->string;
}


int uint_t_cmp (uint_t * a, uint_t * b)
{
    switch (a->type) {
    case UINT_T_16 :
        if (a->uints.uint16 < b->uints.uint16)
            return -1;
        else if (a->uints.uint16 > b->uints.uint16)
            return 1;
        return 0;
    case UINT_T_32 :
        if (a->uints.uint32 < b->uints.uint32)
            return -1;
        else if (a->uints.uint32 > b->uints.uint32)
            return 1;
        return 0;
    case UINT_T_64 :
        if (a->uints.uint64 < b->uints.uint64)
            return -1;
        else if (a->uints.uint64 > b->uints.uint64)
            return 1;
        return 0;
    }
    return 0;
}
    

void uint_t_add_int (uint_t * dst, int i)
{
    switch (dst->type) {
    case UINT_T_16 :
        dst->uints.uint16 += (uint16_t) i;
        break;
    case UINT_T_32 :
        dst->uints.uint32 += (uint32_t) i;
        break;
    case UINT_T_64 :
        dst->uints.uint64 += (uint64_t) i;
        break;
    }
}


void uint_t_make (uint_t * dst, uint_t * type_src)
{
    switch (type_src->type) {
    case UINT_T_16 :
        uint_t_16_set(dst, 0);
        break;
    case UINT_T_32 :
        uint_t_32_set(dst, 0);
        break;
    case UINT_T_64 :
        uint_t_64_set(dst, 0);
        break;
    }
}
