#ifndef LIST_H
#define LIST_H
typedef struct _node 
{
    char *line;
    struct _node *prev;
    struct _node *next;
} node_t;

typedef struct _list_t 
{
    node_t head;
    node_t *current;
    int line_width;
    int num_lines;
    void (*initialize)(char *, int);
} list_t;

list_t *list_create(int width, void(*init)(char *, int));
void list_rewind(list_t *l); 
int list_height(list_t *l);
int list_width(list_t *l);
char *list_forward(list_t *l);
char *list_backward(list_t *l);
char *list_forward_cat(list_t *l);
char *list_backward_cat(list_t *l);
char *list_contents(list_t *l);
void *list_get_position(list_t *l);
void list_set_position(list_t *l, void *node);
int end_list(list_t *l);
void list_destroy(list_t *l);

#endif /* LIST_H */
