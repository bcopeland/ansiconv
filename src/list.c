/*
 *  Implements a doubly-linked list.  Basically implements a queue that
 *  can grow either way.  The user has to be smart; there is ample 
 *  opportunity to scribble over pointers that get passed back.
 *
 *  Possible future enhancement: parse multiple lines into a single node.
 */
#include <stdio.h>
#include <stdlib.h>

#include "list.h"

list_t *list_create(int width, void(*init)(char *, int)) 
{
    list_t *l = malloc(sizeof(list_t));
    l->head.next = l->head.prev = &l->head;
    l->current = &l->head;
    l->line_width = width;
    l->initialize = init;
    l->num_lines = 0;
}

void list_rewind(list_t *l) 
{
    l->current = &l->head;
}

int list_height(list_t *l) 
{
    return l->num_lines;
}

int list_width(list_t *l) 
{
    return l->line_width;
}

static void get_node(list_t *l, node_t **node)
{
    *node = malloc(sizeof(node_t)); 
    (*node)->line = malloc(l->line_width); 
    if (l->initialize) 
        l->initialize((*node)->line, l->line_width);
    
    l->num_lines++;
}

char *list_forward(list_t *l) 
{
    if (l->current->next != &l->head) 
        l->current = l->current->next;
    return l->current->line;
}

char *list_backward(list_t *l) 
{
    if (l->current->prev != &l->head)
        l->current = l->current->prev;
    return l->current->line;
}

char *list_forward_cat(list_t *l)
{
    if (l->current->next != &l->head) {
        l->current = l->current->next;
    } else {
        get_node(l, &l->current->next);

        l->current->next->prev = l->current;
        l->current = l->current->next;
        l->current->next = &l->head; 
        l->head.prev = l->current; 
    }
    return l->current->line;
}

char *list_backward_cat(list_t *l) 
{
    if (l->current->prev != &l->head) {
        l->current = l->current->prev;
    } else {
        get_node(l, &l->current->prev);
        l->current->prev->next = l->current;
        l->current = l->current->prev;
        l->current->prev = &l->head; 
        l->head.next = l->current;
    }
    return l->current->line;
}

char *list_contents(list_t *l) 
{
    return l->current->line;
}

void *list_get_position(list_t *l) 
{
    return l->current;
}

void list_set_position(list_t *l, void *node) 
{
    l->current = node;
}

static void free_node(node_t *node) 
{
    free(node->line);
    free(node);
}

int end_list(list_t *l) 
{
    return l->current->next == &l->head;
}

void list_destroy(list_t *l)
{
    l->current = l->head.next;
    while (l->current != &l->head) 
    {
        node_t *tmp = l->current;
        l->current = l->current->next;
        free_node(tmp);
    }
}

