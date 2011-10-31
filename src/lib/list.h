#ifndef list_HEADER
#define list_HEADER

#include "aatree.h"

#define LIST_ERROR_NOTFOUND 1
#define LIST_ERROR_EMPTY    2

#define LIST_CMP_FUNC int (* cmp) (void * a, void * b)

typedef struct _list_node_s {
    void * data;
    struct _list_node_s * next;
    struct _list_node_s * prev;
} _list_node;

typedef struct _list_s {
    int data_size;
    int size;
    _list_node * first;
    _list_node * last;
} _list;

typedef struct _list_iterator_s {
    _list_node * node;
} _list_iterator;

_list * list_create      (int data_size);
void    list_destroy     (_list * list);
_list * list_copy_aatree (_aatree * tree);

int     list_insert (_list * list, void * data);
int     list_delete (_list * list, LIST_CMP_FUNC, void * data);
int     list_sort   (_list * list, LIST_CMP_FUNC);

int    list_iterator (_list * list, _list_iterator * iterator);
void * list_next     (_list_iterator * iterator);

_list_node * list_node_sort (_list_node * list_node, LIST_CMP_FUNC);

#endif
