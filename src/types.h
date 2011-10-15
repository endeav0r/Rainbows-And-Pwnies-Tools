#ifndef types_HEADER
#define types_HEADER

#include <stdint.h>
#include <stdio.h>

#define INT_T_8  1
#define INT_T_16 2
#define INT_T_32 3
#define INT_T_64 4

#define INT_T_TYPE_ERROR -2
#define INT_T_ERROR      -3

#define UINT_T_8  17
#define UINT_T_16 18
#define UINT_T_32 19
#define UINT_T_64 20

#define UINT_T_TYPE_ERROR -4
#define UINT_T_ERROR      -5

#define INT_STRLEN 24

#define INT_T_BITS(x) (x->type == INT_T_8 ? 8 : \
                       (x->type == INT_T_16 ? 16 : \
                        (x->type == INT_T_32 ? 32 : 64) \
                       ) \
                      )
#define UINT_T_BITS(x) (x->type == UINT_T_8 ? 8 : \
                        (x->type == UINT_T_16 ? 16 : \
                         (x->type == UINT_T_32 ? 32 : 64) \
                        ) \
                       )


typedef struct int_s {
    union {
        int8_t  int8;
        int16_t int16;
        int32_t int32;
        int64_t int64;
    } ints;
    char type;
    char string[INT_STRLEN];
} int_t;

typedef struct uint_s {
    union {
        uint8_t  uint8;
        uint16_t uint16;
        uint32_t uint32;
        uint64_t uint64;
    } uints;
    char type;
    char string[INT_STRLEN];
} uint_t;

void    int_t_8_set   (int_t * intt, int8_t i);
void    int_t_16_set  (int_t * intt, int16_t i);
void    int_t_32_set  (int_t * intt, int32_t i);
void    int_t_64_set  (int_t * intt, int64_t i);
void    int_t_set     (int_t * dst, int_t * src);
int8_t  int_t_8_get   (int_t * intt);
int16_t int_t_16_get  (int_t * intt);
int32_t int_t_32_get  (int_t * intt);
int64_t int_t_64_get  (int_t * intt);
int     int_t_get     (int_t * intt);
char *  int_t_str     (int_t * i);
char *  int_t_strx    (int_t * i);
char *  int_t_str0x   (int_t * i);
int     int_t_cmp     (int_t * a, int_t * b);
int     int_t_add     (int_t * dst, int_t * b);
int     int_t_sub     (int_t * dst, int_t * b);
int     int_t_mul     (int_t * dst, int_t * b);
int     int_t_div     (int_t * dst, int_t * b);
int     int_t_mod     (int_t * dst, int_t * b);
void    int_t_uint_t  (int_t * dst, uint_t * src);
void    int_t_add_int (int_t * dst, int i);

void         uint_t_8_set   (uint_t * uintt, uint8_t  i);
void         uint_t_16_set  (uint_t * uintt, uint16_t i);
void         uint_t_32_set  (uint_t * uintt, uint32_t i);
void         uint_t_64_set  (uint_t * uintt, uint64_t i);
void         uint_t_set     (uint_t * dst, uint_t * src);
uint8_t      uint_t_8_get   (uint_t * uintt);
uint16_t     uint_t_16_get  (uint_t * uintt);
uint32_t     uint_t_32_get  (uint_t * uintt);
uint64_t     uint_t_64_get  (uint_t * uintt);
unsigned int uint_t_get     (uint_t * uintt);
char *       uint_t_str     (uint_t * i);
char *       uint_t_strx    (uint_t * i);
char *       uint_t_str0x   (uint_t * i);
int          uint_t_cmp     (uint_t * a, uint_t * b);
void         uint_t_add_int (uint_t * dst, int i);
int          uint_t_sub     (uint_t * dst, uint_t * b);
int          uint_t_add     (uint_t * dst, uint_t * b);
int          uint_t_sub     (uint_t * dst, uint_t * b);
int          uint_t_mul     (uint_t * dst, uint_t * b);
int          uint_t_div     (uint_t * dst, uint_t * b);
int          uint_t_mod     (uint_t * dst, uint_t * b);
int          uint_t_and     (uint_t * dst, uint_t * b);
int          uint_t_or      (uint_t * dst, uint_t * b);
int          uint_t_xor     (uint_t * dst, uint_t * b);
int          uint_t_rotr    (uint_t * dst, int bits);
int          uint_t_rotl    (uint_t * dst, int bits);
void         uint_t_int_t   (uint_t * dst, int_t * src);

#endif
