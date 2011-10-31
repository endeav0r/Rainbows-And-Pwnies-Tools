#ifndef aatree_HEADER
#define aatree_HEADER

#define AATREE_ERROR_NOTFOUND 1
#define AATREE_ERROR_EXISTS   2

#define AATREE_CMP_FUNC int (* cmp) (void * a, void * b)

typedef struct _aatree_node_s {
    int level;
    void * data;
    struct _aatree_node_s * left;
    struct _aatree_node_s * right;
} _aatree_node;


typedef struct _aatree_s {
    int size;
    int data_size;
    _aatree_node * nodes;
    AATREE_CMP_FUNC;
} _aatree;


_aatree * aatree_create  (AATREE_CMP_FUNC, int data_size);
void      aatree_destroy (_aatree * tree);

void      aatree_node_destroy (_aatree_node * node);

int    aatree_merge  (_aatree * tree_dst, _aatree * tree_src);
int    aatree_insert (_aatree * tree, void * data);
void * aatree_search (_aatree * tree, void * data);
int    aatree_delete (_aatree * tree, void * data);
int    aatree_size   (_aatree * tree);

int            aatree_node_merge  (_aatree * tree, _aatree_node * node);
_aatree_node * aatree_node_insert (_aatree * tree, _aatree_node * node,
                                   _aatree_node * new_node, int * error);
_aatree_node * aatree_node_search (_aatree * tree, _aatree_node * node,
                                   void * data, int * error);
_aatree_node * aatree_node_delete (_aatree * tree, _aatree_node * node,
                                   void * data, int * error);
_aatree_node * aatree_node_skew   (_aatree_node * node);
_aatree_node * aatree_node_split  (_aatree_node * node);
_aatree_node * aatree_node_decrease_level (_aatree_node * node);
_aatree_node * aatree_node_successor      (_aatree_node * node);
_aatree_node * aatree_node_predecessor    (_aatree_node * node);

#endif
