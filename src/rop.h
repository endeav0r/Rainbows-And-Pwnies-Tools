#ifndef rop_HEADER
#define rop_HEADER

#include <stdlib.h>
#include <string.h>
#include <udis86.h>

struct _rop_ins {
    int offset;
    char * description;
    struct _rop_ins * next;
};

struct _rop_list {
    int offset;
    struct _rop_ins * ins;
    struct _rop_list * next;
};

/** finds all 'ret' rops in data
* @param data a pointer to data containing instructions
* @param data_size the size, in bytes, of data
* @return a struct _rop_list which contains all the rop gadgets found
*/
struct _rop_list * rop_ret_rops (unsigned char * data, int data_size, int depth);

/** gets the struct _rop_ins from a struct _rop_list
* @param rop_list a valid struct _rop_list
* @return the corresponding struct _rop_ins
*/
struct _rop_ins  * rop_list_ins     (struct _rop_list * rop_list);

/** destroys all data in a struct _rop_list, including the struct _rop_ins
* @param rop_list the struct _rop_list to destroy
*/
void rop_list_destroy (struct _rop_list * rop_list);



// you are not *intended* to interact with these functions, but do as you wish

// destroys the entire struct _rop_ins list
void rop_ins_destroy (struct _rop_ins * ins);

// returns depth of rop if data points to valid instructions, 0 otherwise
int rop_depth (unsigned char * data, int data_size);
// creates a list of struct _rop_ins from data
struct _rop_ins * rop_ins_create (unsigned char * data, int data_size, int offset);


#endif
