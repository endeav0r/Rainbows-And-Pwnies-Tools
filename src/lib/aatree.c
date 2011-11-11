#include "aatree.h"

#include <stdlib.h>
#include <string.h>


_aatree * aatree_create (AATREE_CMP_FUNC, int data_size)
{
    _aatree * tree;

    tree = (_aatree *) malloc(sizeof(_aatree));
    tree->size = 0;
    tree->nodes = NULL;
    tree->cmp = cmp;
    tree->data_size = data_size;

    return tree;
}


void aatree_destroy (_aatree * tree)
{
    if (tree == NULL)
        return;

    if (tree->nodes != NULL)
        aatree_node_destroy(tree->nodes);
    free(tree);
}


void aatree_node_destroy (_aatree_node * node)
{
    if (node == NULL)
        return;

    if (node->left != NULL)
        aatree_node_destroy(node->left);
    if (node->right != NULL)
        aatree_node_destroy(node->right);

    free(node->data);
    free(node);
}


int aatree_merge (_aatree * tree_dst, _aatree * tree_src)
{
    aatree_node_merge(tree_dst, tree_src->nodes);
    return 0;
}


int aatree_insert (_aatree * tree, void * data)
{
    int error = 0;
    _aatree_node * new_node;

    new_node = (_aatree_node *) malloc(sizeof(_aatree_node));
    new_node->data = (void *) malloc(tree->data_size);
    memcpy(new_node->data, data, tree->data_size);
    new_node->level = 0;
    new_node->left = NULL;
    new_node->right = NULL;

    tree->nodes = aatree_node_insert(tree, tree->nodes, new_node, &error);
    if (error != AATREE_ERROR_EXISTS)
        tree->size++;
    
    return error;
}


void * aatree_search (_aatree * tree, void * data)
{
    int error;
    _aatree_node * node;

    node = aatree_node_search(tree, tree->nodes, data, &error);
    
    if (node == NULL)
        return NULL;

    return node->data;
}


int aatree_delete (_aatree * tree, void * data)
{
    int error;

    tree->nodes = aatree_node_delete(tree, tree->nodes, data, &error);
    if (error != AATREE_ERROR_NOTFOUND)
        tree->size--;
    return error;
}


int aatree_size (_aatree * tree)
{
    return tree->size;
}


int aatree_node_merge (_aatree * tree, _aatree_node * node)
{
    if (node == NULL)
        return 0;
    
    aatree_node_merge(tree, node->left);
    aatree_node_merge(tree, node->right);
    
    aatree_insert(tree, node->data);
    return 0;
}


_aatree_node * aatree_node_insert (_aatree * tree, _aatree_node * node,
                                   _aatree_node * new_node, int * error)
{
    int error_tmp;

    if (node == NULL) {
        return new_node;
    }

    if (tree->cmp(new_node->data, node->data) == 0) {
        *error = AATREE_ERROR_EXISTS;
        if (aatree_node_delete(tree, node, new_node->data, &error_tmp) == NULL)
            return new_node;
    }

    if (tree->cmp(new_node->data, node->data) < 0)
        node->left = aatree_node_insert(tree, node->left, new_node, error);
    else if (tree->cmp(new_node->data, node->data) > 0)
        node->right = aatree_node_insert(tree, node->right, new_node, error);
    
    node = aatree_node_skew(node);
    node = aatree_node_split(node);

    error = 0;
    return node;
}


_aatree_node * aatree_node_search (_aatree * tree, _aatree_node * node,
                                   void * data, int * error)
{
    if (node == NULL) {
        *error = AATREE_ERROR_NOTFOUND;
        return NULL;
    }

    error = 0;
    if (tree->cmp(data, node->data) == 0) {
        return node;
    }
    else if (tree->cmp(data, node->data) < 0)
        return aatree_node_search(tree, node->left, data, error);
    else 
        return aatree_node_search(tree, node->right, data, error);
}


_aatree_node * aatree_node_delete (_aatree * tree, _aatree_node * node,
                                   void * data, int * error)
{
    int error_tmp;
    _aatree_node * L;
    void * tmp;

    if (node == NULL) {
        *error = AATREE_ERROR_NOTFOUND;
        return NULL;
    }

    if (tree->cmp(data, node->data) < 0)
        node->left = aatree_node_delete(tree, node->left, data, error);
    else if (tree->cmp(data, node->data) > 0)
        node->right = aatree_node_delete(tree, node->right, data, error);
    else {
        *error = 0;
        if ((node->right == NULL) && (node->left == NULL)) {
            free(node->data);
            free(node);
            return NULL;
        }
        else if (node->left == NULL) {
            L = aatree_node_successor(node);
            tmp = L->data;
            L->data = node->data;
            node->data = tmp;
            node->right = aatree_node_delete(tree, node->right, data, &error_tmp);
        }
        else {
            L = aatree_node_predecessor(node);
            tmp = L->data;
            L->data = node->data;
            node->data = tmp;
            node->left = aatree_node_delete(tree, node->left, data, &error_tmp);
        }
    }

    node = aatree_node_decrease_level(node);
    node = aatree_node_skew(node);
    if (node->right != NULL) {
        node->right = aatree_node_skew(node->right);
        if (node->right->right != NULL)
            node->right->right = aatree_node_skew(node->right->right);
    }
    node = aatree_node_split(node);
    if (node->right != NULL)
        node->right = aatree_node_split(node->right);

    return node;
}


_aatree_node * aatree_node_skew (_aatree_node * node)
{
    _aatree_node * L;

    if (node == NULL)
        return NULL;
    if (node->left == NULL)
        return node;
    if (node->left->level == node->level) {
        L = node->left;
        node->left = L->right;
        L->right = node;
        return L;
    }
    return node;
}

_aatree_node * aatree_node_split (_aatree_node * node)
{
    _aatree_node * R;

    if (node == NULL)
        return NULL;
    if (node->right == NULL)
        return node;
    if (node->right->right == NULL)
        return node;
    if (node->right->right->level == node->level) {
        R = node->right;
        node->right = R->left;
        R->left = node;
        R->level++;
        return R;
    }
    return node;
}


_aatree_node * aatree_node_decrease_level (_aatree_node * node)
{
    int should_be;

    if ((node->left == NULL) || (node->right == NULL))
        return node;

    should_be = (node->left->level < node->right->level)
                ? node->left->level : node->right->level;

    if (should_be < node->level) {
        node->level = should_be;
        if (should_be < node->right->level)
            node->right->level = should_be;
    }

    return node;
}


_aatree_node * aatree_node_successor (_aatree_node * node)
{
    _aatree_node * successor;
    
    if (node == NULL)
        return NULL;
    if (node->right == NULL)
        return NULL;

    successor = node->right;
    while (successor->left != NULL)
        successor = successor->left;

    return successor;
}


_aatree_node * aatree_node_predecessor (_aatree_node * node)
{
    _aatree_node * predecessor;

    if (node == NULL)
        return NULL;
    if (node->left == NULL)
        return NULL;

    predecessor = node->left;
    while (predecessor->right != NULL)
        predecessor = predecessor->right;

    return predecessor;
}
