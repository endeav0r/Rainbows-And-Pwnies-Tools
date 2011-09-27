#include "types.h"


void int_t_8_set (int_t * intt, int8_t i)
{
    intt->ints.int8 = i;
    intt->type = INT_T_8;
}


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


void int_t_set (int_t * dst, int_t * src)
{
    switch (src->type) {
    case INT_T_8  : int_t_8_set (dst, src->ints.int8);  break;
    case INT_T_16 : int_t_16_set(dst, src->ints.int16); break;
    case INT_T_32 : int_t_32_set(dst, src->ints.int32); break;
    case INT_T_64 : int_t_64_set(dst, src->ints.int64); break;
    }
}

int8_t  int_t_8_get  (int_t * intt) { return intt->ints.int8;  }
int16_t int_t_16_get (int_t * intt) { return intt->ints.int16; }
int32_t int_t_32_get (int_t * intt) { return intt->ints.int32; }
int64_t int_t_64_get (int_t * intt) { return intt->ints.int64; }


int int_t_get (int_t * intt)
{
    switch (intt->type) {
    case INT_T_8  : return (int) intt->ints.int8;
    case INT_T_16 : return (int) intt->ints.int16;
    case INT_T_32 : return (int) intt->ints.int32;
    case INT_T_64 : return (int) intt->ints.int64;
    }
    return 0;
}


char * int_t_str (int_t * intt)
{
    switch (intt->type) {
    case INT_T_8 :
        snprintf(intt->string, INT_STRLEN, "%d", intt->ints.int8);
        break;
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
    case INT_T_8 :
        snprintf(intt->string, INT_STRLEN, "%02x", intt->ints.int8);
        break;
    case INT_T_16 :
        snprintf(intt->string, INT_STRLEN, "%04x", intt->ints.int16);
        break;
    case INT_T_32 :
        snprintf(intt->string, INT_STRLEN, "%08x", intt->ints.int32);
        break;
    case INT_T_64 :
        snprintf(intt->string, INT_STRLEN,
                 "%016llx", (long long int) intt->ints.int64);
        break;
    default :
        snprintf(intt->string, INT_STRLEN, "INVALID_INT_T");
    }
    
    return intt->string;
}


int int_t_cmp (int_t * a, int_t * b)
{
    if (a->type != b->type)
        return INT_T_TYPE_ERROR;
    
    switch (a->type) {
    case INT_T_8 :
        if (a->ints.int8 < b->ints.int8)
            return -1;
        else if (a->ints.int8 > b->ints.int8)
            return 1;
        return 0;
    case INT_T_16 :
        if (a->ints.int16 < b->ints.int16)
            return -1;
        else if (a->ints.int16 > b->ints.int16)
            return 1;
        return 0;
    case INT_T_32 :
        if (a->ints.int32 < b->ints.int32)
            return -1;
        else if (a->ints.int32 > b->ints.int32)
            return 1;
        return 0;
    case INT_T_64 :
        if (a->ints.int64 < b->ints.int64)
            return -1;
        else if (a->ints.int64 > b->ints.int64)
            return 1;
        return 0;
    }
    return INT_T_ERROR;
}


int int_t_add (int_t * dst, int_t * b)
{
    switch (dst->type) {
    case INT_T_8 :
        switch (b->type) {
            case INT_T_8  : dst->ints.int8 += b->ints.int8; break;
            default       : return INT_T_TYPE_ERROR;
        }
        break;
    case INT_T_16 :
        switch (b->type) {
            case INT_T_8  : dst->ints.int16 += (int16_t) b->ints.int8;  break;
            case INT_T_16 : dst->ints.int16 += b->ints.int16; break;
            default       : return INT_T_TYPE_ERROR;
        }
        break;
    case INT_T_32 :
        switch (b->type) {
            case INT_T_8  : dst->ints.int32 += (int32_t) b->ints.int8;  break;
            case INT_T_16 : dst->ints.int32 += (int32_t) b->ints.int16; break;
            case INT_T_32 : dst->ints.int32 += b->ints.int32; break;
            default       : return INT_T_TYPE_ERROR;
        }
        break;
    case INT_T_64 :
        switch (b->type) {
            case INT_T_8  : dst->ints.int64 += (int64_t) b->ints.int8;  break;
            case INT_T_16 : dst->ints.int64 += (int64_t) b->ints.int16; break;
            case INT_T_32 : dst->ints.int64 += (int64_t) b->ints.int32; break;
            case INT_T_64 : dst->ints.int64 += b->ints.int64; break;
            default       : return INT_T_TYPE_ERROR;
        }
        break;
    default :
        return INT_T_ERROR;
    }
    return 0;
}


int int_t_sub (int_t * dst, int_t * b)
{
    switch (dst->type) {
    case INT_T_8 :
        switch (b->type) {
            case INT_T_8  : dst->ints.int8 -= b->ints.int8; break;
            default       : return INT_T_TYPE_ERROR;
        }
        break;
    case INT_T_16 :
        switch (b->type) {
            case INT_T_8  : dst->ints.int16 -= (int16_t) b->ints.int8;  break;
            case INT_T_16 : dst->ints.int16 -= b->ints.int16; break;
            default       : return INT_T_TYPE_ERROR;
        }
        break;
    case INT_T_32 :
        switch (b->type) {
            case INT_T_8  : dst->ints.int32 -= (int32_t) b->ints.int8;  break;
            case INT_T_16 : dst->ints.int32 -= (int32_t) b->ints.int16; break;
            case INT_T_32 : dst->ints.int32 -= b->ints.int32; break;
            default       : return INT_T_TYPE_ERROR;
        }
        break;
    case INT_T_64 :
        switch (b->type) {
            case INT_T_8  : dst->ints.int64 -= (int64_t) b->ints.int8;  break;
            case INT_T_16 : dst->ints.int64 -= (int64_t) b->ints.int16; break;
            case INT_T_32 : dst->ints.int64 -= (int64_t) b->ints.int32; break;
            case INT_T_64 : dst->ints.int64 -= b->ints.int64; break;
            default       : return INT_T_TYPE_ERROR;
        }
        break;
    default :
        return INT_T_ERROR;
    }
    return 0;
}


int int_t_mul (int_t * dst, int_t * b)
{
    switch (dst->type) {
    case INT_T_8 :
        switch (b->type) {
            case INT_T_8  : dst->ints.int8 *= b->ints.int8; break;
            default       : return INT_T_TYPE_ERROR;
        }
        break;
    case INT_T_16 :
        switch (b->type) {
            case INT_T_8  : dst->ints.int16 *= (int16_t) b->ints.int8;  break;
            case INT_T_16 : dst->ints.int16 *= b->ints.int16; break;
            default       : return INT_T_TYPE_ERROR;
        }
        break;
    case INT_T_32 :
        switch (b->type) {
            case INT_T_8  : dst->ints.int32 *= (int32_t) b->ints.int8;  break;
            case INT_T_16 : dst->ints.int32 *= (int32_t) b->ints.int16; break;
            case INT_T_32 : dst->ints.int32 *= b->ints.int32; break;
            default       : return INT_T_TYPE_ERROR;
        }
        break;
    case INT_T_64 :
        switch (b->type) {
            case INT_T_8  : dst->ints.int64 *= (int64_t) b->ints.int8;  break;
            case INT_T_16 : dst->ints.int64 *= (int64_t) b->ints.int16; break;
            case INT_T_32 : dst->ints.int64 *= (int64_t) b->ints.int32; break;
            case INT_T_64 : dst->ints.int64 *= b->ints.int64; break;
            default       : return INT_T_TYPE_ERROR;
        }
        break;
    default :
        return INT_T_ERROR;
    }
    return 0;
}


int int_t_div (int_t * dst, int_t * b)
{
    switch (dst->type) {
    case INT_T_8 :
        switch (b->type) {
            case INT_T_8  : dst->ints.int8 /= b->ints.int8; break;
            default       : return INT_T_TYPE_ERROR;
        }
        break;
    case INT_T_16 :
        switch (b->type) {
            case INT_T_8  : dst->ints.int16 /= (int16_t) b->ints.int8;  break;
            case INT_T_16 : dst->ints.int16 /= b->ints.int16; break;
            default       : return INT_T_TYPE_ERROR;
        }
        break;
    case INT_T_32 :
        switch (b->type) {
            case INT_T_8  : dst->ints.int32 /= (int32_t) b->ints.int8;  break;
            case INT_T_16 : dst->ints.int32 /= (int32_t) b->ints.int16; break;
            case INT_T_32 : dst->ints.int32 /= b->ints.int32; break;
            default       : return INT_T_TYPE_ERROR;
        }
        break;
    case INT_T_64 :
        switch (b->type) {
            case INT_T_8  : dst->ints.int64 /= (int64_t) b->ints.int8;  break;
            case INT_T_16 : dst->ints.int64 /= (int64_t) b->ints.int16; break;
            case INT_T_32 : dst->ints.int64 /= (int64_t) b->ints.int32; break;
            case INT_T_64 : dst->ints.int64 /= b->ints.int64; break;
            default       : return INT_T_TYPE_ERROR;
        }
        break;
    default :
        return INT_T_ERROR;
    }
    return 0;
}



int int_t_mod (int_t * dst, int_t * b)
{
    switch (dst->type) {
    case INT_T_8 :
        switch (b->type) {
            case INT_T_8  : dst->ints.int8 %= b->ints.int8; break;
            default       : return INT_T_TYPE_ERROR;
        }
        break;
    case INT_T_16 :
        switch (b->type) {
            case INT_T_8  : dst->ints.int16 %= (int16_t) b->ints.int8;  break;
            case INT_T_16 : dst->ints.int16 %= b->ints.int16; break;
            default       : return INT_T_TYPE_ERROR;
        }
        break;
    case INT_T_32 :
        switch (b->type) {
            case INT_T_8  : dst->ints.int32 %= (int32_t) b->ints.int8;  break;
            case INT_T_16 : dst->ints.int32 %= (int32_t) b->ints.int16; break;
            case INT_T_32 : dst->ints.int32 %= b->ints.int32; break;
            default       : return INT_T_TYPE_ERROR;
        }
        break;
    case INT_T_64 :
        switch (b->type) {
            case INT_T_8  : dst->ints.int64 %= (int64_t) b->ints.int8;  break;
            case INT_T_16 : dst->ints.int64 %= (int64_t) b->ints.int16; break;
            case INT_T_32 : dst->ints.int64 %= (int64_t) b->ints.int32; break;
            case INT_T_64 : dst->ints.int64 %= b->ints.int64; break;
            default       : return INT_T_TYPE_ERROR;
        }
        break;
    default :
        return INT_T_ERROR;
    }
    return 0;
}



void uint_t_8_set  (uint_t * uintt, uint8_t i)
{
    uintt->uints.uint8 = i;
    uintt->type = UINT_T_8;
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
    case UINT_T_8  : uint_t_8_set (dst, src->uints.uint8);  break;
    case UINT_T_16 : uint_t_16_set(dst, src->uints.uint16); break;
    case UINT_T_32 : uint_t_32_set(dst, src->uints.uint32); break;
    case UINT_T_64 : uint_t_64_set(dst, src->uints.uint64); break;
    }
}


uint8_t  uint_t_8_get  (uint_t * uintt) { return uintt->uints.uint8;  }
uint16_t uint_t_16_get (uint_t * uintt) { return uintt->uints.uint16; }
uint32_t uint_t_32_get (uint_t * uintt) { return uintt->uints.uint32; }
uint64_t uint_t_64_get (uint_t * uintt) { return uintt->uints.uint64; }


unsigned int uint_t_get (uint_t * uintt)
{
    switch (uintt->type) {
    case UINT_T_8  : return (unsigned int) uintt->uints.uint8  & 0x000000ff;
    case UINT_T_16 : return (unsigned int) uintt->uints.uint16 & 0x0000ffff;
    case UINT_T_32 : return (unsigned int) uintt->uints.uint32;
    case UINT_T_64 : return (unsigned int) uintt->uints.uint64;
    }
    return 0;
}


char * uint_t_str (uint_t * uintt)
{
    switch (uintt->type) {
    case UINT_T_8 :
        snprintf(uintt->string, INT_STRLEN, "%d", uintt->uints.uint8);
        break;
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
    case UINT_T_8 :
        snprintf(uintt->string, INT_STRLEN, "%02x", uintt->uints.uint8);
        break;
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
    if (a->type != b->type)
        return UINT_T_TYPE_ERROR;
    switch (a->type) {
    case UINT_T_8 :
        if (a->uints.uint8 < b->uints.uint8)
            return -1;
        else if (a->uints.uint8 > b->uints.uint8)
            return 1;
        return 0;
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
    return UINT_T_ERROR;
}
    

void uint_t_add_int (uint_t * dst, int i)
{
    switch (dst->type) {
    case UINT_T_8  : dst->uints.uint8  += (uint8_t)  i; break;
    case UINT_T_16 : dst->uints.uint16 += (uint16_t) i; break;
    case UINT_T_32 : dst->uints.uint32 += (uint32_t) i; break;
    case UINT_T_64 : dst->uints.uint64 += (uint64_t) i; break;
    }
}


int uint_t_add (uint_t * dst, uint_t * b)
{
    switch (dst->type) {
    case UINT_T_8 :
        switch (b->type) {
            case UINT_T_8  : dst->uints.uint8 += b->uints.uint8; break;
            default        : return UINT_T_TYPE_ERROR;
        }
    case UINT_T_16 :
        switch (b->type) {
            case UINT_T_8  : dst->uints.uint16 += (uint16_t) b->uints.uint8; break;
            case UINT_T_16 : dst->uints.uint16 += b->uints.uint16; break;
            default        : return UINT_T_TYPE_ERROR;
        }
        break;
    case UINT_T_32 :
        switch (b->type) {
            case UINT_T_8  : dst->uints.uint16 += (uint32_t) b->uints.uint8; break;
            case UINT_T_16 : dst->uints.uint32 += (uint32_t) b->uints.uint16; break;
            case UINT_T_32 : dst->uints.uint32 += b->uints.uint32; break;
            default        : return INT_T_TYPE_ERROR;
        }
        break;
    case UINT_T_64 :
        switch (b->type) {
            case UINT_T_8  : dst->uints.uint16 += (uint64_t) b->uints.uint8; break;
            case UINT_T_16 : dst->uints.uint64 += (uint64_t) b->uints.uint16; break;
            case UINT_T_32 : dst->uints.uint64 += (uint64_t) b->uints.uint32; break;
            case UINT_T_64 : dst->uints.uint64 += b->uints.uint64; break;
            default        : return UINT_T_TYPE_ERROR;
        }
        break;
    default :
        return UINT_T_ERROR;
    }
    return 0;
}


int uint_t_sub (uint_t * dst, uint_t * b)
{
    switch (dst->type) {
    case UINT_T_8 :
        switch (b->type) {
            case UINT_T_8  : dst->uints.uint8 -= b->uints.uint8; break;
            default        : return UINT_T_TYPE_ERROR;
        }
    case UINT_T_16 :
        switch (b->type) {
            case UINT_T_8  : dst->uints.uint16 -= (uint16_t) b->uints.uint8; break;
            case UINT_T_16 : dst->uints.uint16 -= b->uints.uint16; break;
            default        : return UINT_T_TYPE_ERROR;
        }
        break;
    case UINT_T_32 :
        switch (b->type) {
            case UINT_T_8  : dst->uints.uint16 -= (uint32_t) b->uints.uint8; break;
            case UINT_T_16 : dst->uints.uint32 -= (uint32_t) b->uints.uint16; break;
            case UINT_T_32 : dst->uints.uint32 -= b->uints.uint32; break;
            default        : return INT_T_TYPE_ERROR;
        }
        break;
    case UINT_T_64 :
        switch (b->type) {
            case UINT_T_8  : dst->uints.uint16 -= (uint64_t) b->uints.uint8; break;
            case UINT_T_16 : dst->uints.uint64 -= (uint64_t) b->uints.uint16; break;
            case UINT_T_32 : dst->uints.uint64 -= (uint64_t) b->uints.uint32; break;
            case UINT_T_64 : dst->uints.uint64 -= b->uints.uint64; break;
            default        : return UINT_T_TYPE_ERROR;
        }
        break;
    default :
        return UINT_T_ERROR;
    }
    return 0;
}


int uint_t_mul (uint_t * dst, uint_t * b)
{
    switch (dst->type) {
    case UINT_T_8 :
        switch (b->type) {
            case UINT_T_8  : dst->uints.uint8 *= b->uints.uint8; break;
            default        : return UINT_T_TYPE_ERROR;
        }
    case UINT_T_16 :
        switch (b->type) {
            case UINT_T_8  : dst->uints.uint16 *= (uint16_t) b->uints.uint8; break;
            case UINT_T_16 : dst->uints.uint16 *= b->uints.uint16; break;
            default        : return UINT_T_TYPE_ERROR;
        }
        break;
    case UINT_T_32 :
        switch (b->type) {
            case UINT_T_8  : dst->uints.uint16 *= (uint32_t) b->uints.uint8; break;
            case UINT_T_16 : dst->uints.uint32 *= (uint32_t) b->uints.uint16; break;
            case UINT_T_32 : dst->uints.uint32 *= b->uints.uint32; break;
            default        : return INT_T_TYPE_ERROR;
        }
        break;
    case UINT_T_64 :
        switch (b->type) {
            case UINT_T_8  : dst->uints.uint16 *= (uint64_t) b->uints.uint8; break;
            case UINT_T_16 : dst->uints.uint64 *= (uint64_t) b->uints.uint16; break;
            case UINT_T_32 : dst->uints.uint64 *= (uint64_t) b->uints.uint32; break;
            case UINT_T_64 : dst->uints.uint64 *= b->uints.uint64; break;
            default        : return UINT_T_TYPE_ERROR;
        }
        break;
    default :
        return UINT_T_ERROR;
    }
    return 0;
}


int uint_t_div (uint_t * dst, uint_t * b)
{
    switch (dst->type) {
    case UINT_T_8 :
        switch (b->type) {
            case UINT_T_8  : dst->uints.uint8 /= b->uints.uint8; break;
            default        : return UINT_T_TYPE_ERROR;
        }
    case UINT_T_16 :
        switch (b->type) {
            case UINT_T_8  : dst->uints.uint16 /= (uint16_t) b->uints.uint8; break;
            case UINT_T_16 : dst->uints.uint16 /= b->uints.uint16; break;
            default        : return UINT_T_TYPE_ERROR;
        }
        break;
    case UINT_T_32 :
        switch (b->type) {
            case UINT_T_8  : dst->uints.uint16 /= (uint32_t) b->uints.uint8; break;
            case UINT_T_16 : dst->uints.uint32 /= (uint32_t) b->uints.uint16; break;
            case UINT_T_32 : dst->uints.uint32 /= b->uints.uint32; break;
            default        : return INT_T_TYPE_ERROR;
        }
        break;
    case UINT_T_64 :
        switch (b->type) {
            case UINT_T_8  : dst->uints.uint16 /= (uint64_t) b->uints.uint8; break;
            case UINT_T_16 : dst->uints.uint64 /= (uint64_t) b->uints.uint16; break;
            case UINT_T_32 : dst->uints.uint64 /= (uint64_t) b->uints.uint32; break;
            case UINT_T_64 : dst->uints.uint64 /= b->uints.uint64; break;
            default        : return UINT_T_TYPE_ERROR;
        }
        break;
    default :
        return UINT_T_ERROR;
    }
    return 0;
}


int uint_t_mod (uint_t * dst, uint_t * b)
{
    switch (dst->type) {
    case UINT_T_8 :
        switch (b->type) {
            case UINT_T_8  : dst->uints.uint8 %= b->uints.uint8; break;
            default        : return UINT_T_TYPE_ERROR;
        }
    case UINT_T_16 :
        switch (b->type) {
            case UINT_T_8  : dst->uints.uint16 %= (uint16_t) b->uints.uint8; break;
            case UINT_T_16 : dst->uints.uint16 %= b->uints.uint16; break;
            default        : return UINT_T_TYPE_ERROR;
        }
        break;
    case UINT_T_32 :
        switch (b->type) {
            case UINT_T_8  : dst->uints.uint16 %= (uint32_t) b->uints.uint8; break;
            case UINT_T_16 : dst->uints.uint32 %= (uint32_t) b->uints.uint16; break;
            case UINT_T_32 : dst->uints.uint32 %= b->uints.uint32; break;
            default        : return INT_T_TYPE_ERROR;
        }
        break;
    case UINT_T_64 :
        switch (b->type) {
            case UINT_T_8  : dst->uints.uint16 %= (uint64_t) b->uints.uint8; break;
            case UINT_T_16 : dst->uints.uint64 %= (uint64_t) b->uints.uint16; break;
            case UINT_T_32 : dst->uints.uint64 %= (uint64_t) b->uints.uint32; break;
            case UINT_T_64 : dst->uints.uint64 %= b->uints.uint64; break;
            default        : return UINT_T_TYPE_ERROR;
        }
        break;
    default :
        return UINT_T_ERROR;
    }
    return 0;
}
