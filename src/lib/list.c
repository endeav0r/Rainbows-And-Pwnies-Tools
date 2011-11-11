#include "list.h"

#include <stdlib.h>
#include <string.h>


_list * list_create (int data_size)
{
    _list * list;

    list = (_list *) malloc(sizeof(_list));
    list->data_size = data_size;
    list->size = 0;
    list->first = NULL;
    list->last = NULL;

    return list;
}


void list_destroy (_list * list)
{
    _list_node * next;

    if (list == NULL);
        return;

    while (list->first != NULL) {
        next = list->first->next;
        free(list->first->data);
        free(list->first);
        list->first = next;
    }

    free(list);
}


void list_copy_aatree_node (_list * list, _aatree_node * node)
{
    if (node == NULL)
        return;
    list_copy_aatree_node(list, node->left);
    list_insert(list, node->data);
    list_copy_aatree_node(list, node->right);
}


_list * list_copy_aatree (_aatree * tree)
{
    _list * list;

    list = list_create(tree->data_size);
    list_copy_aatree_node(list, tree->nodes);

    return list;
}


int list_insert (_list * list, void * data)
{
    _list_node * list_node;

    list_node = (_list_node *) malloc(sizeof(_list_node));
    list_node->data = (void *) malloc(list->data_size);
    memcpy(list_node->data, data, list->data_size);
    list_node->next = NULL;
    list_node->prev = NULL;

    if (list->first == NULL) {
        list->first = list_node;
        list->last = list_node;
    }
    else {
        list->last->next = list_node;
        list_node->prev = list->last;
        list->last = list_node;
    }

    list->size++;

    return 0;
}


int list_delete (_list * list, LIST_CMP_FUNC, void * data)
{
    _list_node * list_node;

    list_node = list->first;
    while (list_node != NULL) {
        if (cmp(list_node, data) == 0) {
            if (list_node->prev == NULL)
                list->first = list_node->next;
            else
                list_node->prev->next = list_node->next;
            if (list_node->next == NULL)
                list->last = list_node->prev;
            else
                list_node->next->prev = list_node->prev;
            free(list_node->data);
            free(list_node);
            list->size--;
            return 0;
        }
        list_node = list_node->next;
    }

    return LIST_ERROR_NOTFOUND;
}


int list_sort (_list * list, LIST_CMP_FUNC)
{
    _list_node * list_node;

    if (list->first == NULL)
        return LIST_ERROR_EMPTY;
    
    list->first = list_node_sort(list->first, cmp);

    list_node = list->first;
    while (list_node->next != NULL)
        list_node = list_node->next;
    list->last = list_node;

    return 0;
}


int list_iterator (_list * list, _list_iterator * iterator)
{
    iterator->node = list->first;
    return 0;
}


void * list_next (_list_iterator * iterator)
{
    void * data;
    if (iterator->node == NULL)
        return NULL;

    data = iterator->node->data;
    iterator->node = iterator->node->next;

    return data;
}


_list_node * list_node_sort (_list_node * list_node, LIST_CMP_FUNC)
{
    _list_node * a;
    _list_node * b;
    _list_node * result;

    if (list_node == NULL)
        return NULL;
    if (list_node->next == NULL)
        return list_node;
    
    a = list_node;
    b = list_node->next;

    while (b->next != NULL) {
        b = b->next;
        if (b->next == NULL)
            break;
        a = a->next;
        b = b->next;
    }

    a->next = NULL;

    a = list_node_sort(a, cmp);
    b = list_node_sort(b, cmp);

    result = NULL;
    while ((a != NULL) && (b != NULL)) {
        if (a == NULL) {
            b->prev = result;
            if (result == NULL)
                result = b;
            else {
                result->next = b;
                result = result->next;
            }
            b = b->next;
        }
        else if (b == NULL) {
            a->prev = result;
            if (result == NULL)
                result = a;
            else {
                result->next = a;
                result = result->next;
            }
            a = a->next;
        }
        else if (cmp(a, b) < 0) {
            a->prev = result;
            if (result == NULL)
                result = a;
            else {
                result->next = a;
                result = result->next;
            }
            a = a->next;
        }
        else {
            b->prev = result;
            if (result == NULL)
                result = b;
            else {
                result->next = b;
                result = result->next;
            }
            b = b->next;
        }
    }

    // redundant, but safe
    result->next = NULL;

    return result;
}
