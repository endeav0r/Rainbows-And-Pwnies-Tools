#ifndef types_HEADER
#define types_HEADER

#include <stdint.h>
#include <stdio.h>

#define INT_T_16 0
#define INT_T_32 1
#define INT_T_64 2

#define UINT_T_16 16
#define UINT_T_32 17
#define UINT_T_64 18

#define INT_STRLEN 24

typedef struct int_s {
    union {
        int16_t int16;
        int32_t int32;
        int64_t int64;
    } ints;
    char type;
    char string[INT_STRLEN];
} int_t;

typedef struct uint_s {
    union {
        uint16_t uint16;
        uint32_t uint32;
        uint64_t uint64;
    } uints;
    char type;
    char string[INT_STRLEN];
} uint_t;

void    int_t_16_set (int_t * intt, int16_t i);
void    int_t_32_set (int_t * intt, int32_t i);
void    int_t_64_set (int_t * intt, int64_t i);
int16_t int_t_16_get (int_t * intt);
int32_t int_t_32_get (int_t * intt);
int64_t int_t_64_get (int_t * intt);
int     int_t_get    (int_t * intt);
char *  int_t_str    (int_t * i);
char *  int_t_strx   (int_t * i);

void         uint_t_16_set  (uint_t * uintt, uint16_t i);
void         uint_t_32_set  (uint_t * uintt, uint32_t i);
void         uint_t_64_set  (uint_t * uintt, uint64_t i);
void         uint_t_set     (uint_t * dst, uint_t * src);
uint16_t     uint_t_16_get  (uint_t * uintt);
uint32_t     uint_t_32_get  (uint_t * uintt);
uint64_t     uint_t_64_get  (uint_t * uintt);
unsigned int uint_t_get     (uint_t * uintt);
char *       uint_t_str     (uint_t * i);
char *       uint_t_strx    (uint_t * i);
int          uint_t_cmp     (uint_t * a, uint_t * b);
void         uint_t_add_int (uint_t * dst, int i);
void         uint_t_make    (uint_t * dst, uint_t * type_src);

#endif
