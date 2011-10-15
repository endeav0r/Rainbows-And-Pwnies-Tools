#ifndef rta_mem_HEADER
#define rta_mem_HEADER

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#define RTA_MEM_BASE_ADDRESS 0x10000000

struct _rta_mem_malloc {
	int level;
    uint64_t address;
    uint64_t size;
    unsigned char * data;
    struct _rta_mem_malloc * left;
    struct _rta_mem_malloc * right;
};

struct _rta_mem {
    struct _rta_mem_malloc * mem_malloc;
};

/**
 * @return a valid struct _rta_mem
 */
struct _rta_mem * rta_mem_create ();

/**
 * @param mem a valid struct _rta_mem to be freed and destroyed
 */
void rta_mem_destroy (struct _rta_mem * mem);

/**
 * @param mem a valid struct _rta_mem
 * @param address address of memory to get
 * @param size the size of the memory to load in bytes
 * @return a pointer to the memory, or NULL on memory access violation
 */
uint64_t rta_mem_load (struct _rta_mem * mem, 
                       uint64_t address,
                       int size);
                                
/**
 * @param mem a valid struct _rta_mem
 * @param address the address to store the value at
 * @param value value to store in memry
 * @param size of value to store in bits: 8, 16, 32, 64
 * @return 0 on succes, -1 on memory access violation
 */
int  rta_mem_store  (struct _rta_mem * mem,
                     uint64_t address,
                     uint64_t value,
                     int size);
                                
/**
 * @param mem a valid struct _rta_mem
 * @param size the size, in bytes, of the memory to allocate
 * @return the address of the newly allocated memory
 */
uint64_t rta_mem_malloc (struct _rta_mem * mem, uint64_t size);

/**
 * @param mem a valid struct _rta_mem
 * @param address the address of the memory to free. should be the same
 *                address that malloc was called with
 * @return 0 on success, -1 on memory access violation
 */
int             rta_mem_free   (struct _rta_mem * mem,
                                uint64_t address);

struct _rta_mem_malloc * rta_mem_malloc_skew 
                                  (struct _rta_mem_malloc * mem_malloc);
struct _rta_mem_malloc * rta_mem_malloc_split
                                  (struct _rta_mem_malloc * mem_malloc);
struct _rta_mem_malloc * rta_mem_malloc_find
                (struct _rta_mem_malloc * mem_malloc, uint64_t address);
struct _rta_mem_malloc * rta_mem_malloc_insert
                                  (struct _rta_mem_malloc * mem_malloc,
                                   struct _rta_mem_malloc * new_malloc);

#endif
