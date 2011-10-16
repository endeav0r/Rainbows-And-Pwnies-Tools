#include "mem.h"


struct _rta_mem * rta_mem_create ()
{
    struct _rta_mem * mem;
    
    mem = (struct _rta_mem *) malloc(sizeof(struct _rta_mem));
    mem->mem_malloc = NULL;

    return mem;
}


void rta_mem_malloc_destroy (struct _rta_mem_malloc * mem_malloc)
{
    if (mem_malloc == NULL)
        return;
    if (mem_malloc->left != NULL)
        rta_mem_malloc_destroy(mem_malloc->left);
    if (mem_malloc->right != NULL)
        rta_mem_malloc_destroy(mem_malloc->right);
    free(mem_malloc->data);
    free(mem_malloc);
}

void rta_mem_destroy (struct _rta_mem * mem)
{
    rta_mem_malloc_destroy(mem->mem_malloc);
    free(mem);
}

uint64_t rta_mem_malloc (struct _rta_mem * mem, uint64_t size)
{
	struct _rta_mem_malloc * new_malloc;
	
	new_malloc = (struct _rta_mem_malloc *) malloc(sizeof(new_malloc));
	new_malloc->level = 0;
	new_malloc->data = (unsigned char *) malloc(size);
#if __SIZEOF_PTRDIFF_T__ == 4
	new_malloc->address = (uint64_t) (uint32_t) new_malloc->data;
#else
    new_malloc->address = (uint64_t) new_malloc->data;
#endif
	new_malloc->size = size;
	new_malloc->left  = NULL;
	new_malloc->right = NULL;
	
	mem->mem_malloc = rta_mem_malloc_insert(mem->mem_malloc, new_malloc);
	
	return new_malloc->address;
}


uint64_t rta_mem_load (struct _rta_mem * mem,
                       uint64_t address,
                       int size)
{
	struct _rta_mem_malloc * mem_malloc;
	int offset;
	
	mem_malloc = rta_mem_malloc_find (mem->mem_malloc, address);
	
	offset = (int) (mem_malloc->address - address);
	
	switch (size) {
	case 1 : return *((uint8_t *)  &(mem_malloc->data[offset]));
	case 2 : return *((uint16_t *) &(mem_malloc->data[offset]));
	case 4 : return *((uint32_t *) &(mem_malloc->data[offset]));
	case 8 : return *((uint64_t *) &(mem_malloc->data[offset]));
	}
	return 0;
}


struct _rta_mem_malloc * rta_mem_malloc_skew 
                                   (struct _rta_mem_malloc * mem_malloc)
{
    struct _rta_mem_malloc * L;
    
    if (mem_malloc == NULL)
        return NULL;
    if (mem_malloc->left == NULL)
        return mem_malloc;
    
    if (mem_malloc->left->level == mem_malloc->level) {
        L = mem_malloc->left;
        mem_malloc->left = L->right;
        L->right = mem_malloc;
        return L;
    }
    return mem_malloc;
}


struct _rta_mem_malloc * rta_mem_malloc_split
                                   (struct _rta_mem_malloc * mem_malloc)
{
    struct _rta_mem_malloc * R;
    
    if (mem_malloc == NULL)
        return NULL;
    if (mem_malloc->right == NULL)
        return mem_malloc;
    if (mem_malloc->right->right == NULL)
        return mem_malloc;
    
    if (mem_malloc->level == mem_malloc->right->right->level) {
        R = mem_malloc->right;
        mem_malloc->right = R->left;
        R->left = mem_malloc;
        R->level++;
        return R;
    }
    
    return mem_malloc;
}


struct _rta_mem_malloc * rta_mem_malloc_find (struct _rta_mem_malloc * mem_malloc,
                                              uint64_t address)
{
    if (mem_malloc == NULL)
        return NULL;
    
    if (    (mem_malloc->address <= address)
         && (mem_malloc->address + mem_malloc->size > address)) {
        return mem_malloc;
    }
    else if (address < mem_malloc->address)
        return rta_mem_malloc_find(mem_malloc->left, address);
    else if (address > mem_malloc->address)
        return rta_mem_malloc_find(mem_malloc->right, address);
    return NULL;    
}



struct _rta_mem_malloc * rta_mem_malloc_insert
                                   (struct _rta_mem_malloc * mem_malloc,
                                    struct _rta_mem_malloc * new_malloc)
{
	if (mem_malloc == NULL)
		return new_malloc;
		
    if (new_malloc->address < mem_malloc->address)
    	mem_malloc->left = rta_mem_malloc_insert(mem_malloc, new_malloc);
    else if (new_malloc->address > mem_malloc->address)
    	mem_malloc->right = rta_mem_malloc_insert(mem_malloc, new_malloc);
    else {
        fprintf(stderr, "rta_malloc error: double malloc\n");
        exit(-1);
    }
    
    mem_malloc = rta_mem_malloc_skew(mem_malloc);
    mem_malloc = rta_mem_malloc_split(mem_malloc);
    
    return mem_malloc;
}    
