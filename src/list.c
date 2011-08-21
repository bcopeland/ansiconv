/*
 *  Implements a doubly-linked list.  Basically implements a queue that
 *  can grow either way.  The user has to be smart; there is ample 
 *  opportunity to scribble over pointers that get passed back.
 *
 *  Possible future enhancement: parse multiple lines into a 
 *  single node.
 */
#include <stdio.h>

typedef struct _node {
  char *line;
  struct _node *prev;
  struct _node *next;  
} Node;

Node List;
Node *current;
int line_width;
int num_lines;
void (*initialize)(char *, int) = NULL;

void listInit( int width, void (*init)(char *, int) ) {
  List.next = List.prev = &List;
  current = &List;
  line_width = width;
  initialize = init;
  num_lines = 0;
}

void listRewind() {
  current = &List;
}

int listHeight() {
  return( num_lines );
}

int listWidth() {
  return line_width;
}

void getNode( Node **new ) {
  *new = (Node *) malloc( sizeof(Node) ); 
  (*new)->line = (char *) malloc( line_width ); 
  if (initialize) {
    initialize( (*new)->line, line_width );
  }
  num_lines ++;
}

char *listForward() {
  if( current->next != &List ) 
    current = current->next;
  return current->line;
}

char *listBackward() {
  if( current->prev != &List )
    current = current->prev;
  return current->line;
}

char *listForwardCat() {
  if ( current->next != &List ) {
    current = current->next;
  } else {
    getNode( &current->next );
    current->next->prev = current;
    current = current->next;
    current->next = &List; 
    List.prev = current; 
  }
  return current->line;
}

char *listBackwardCat() {
  if ( current->prev != &List ) {
    current = current->prev;
  } else {
    getNode( &current->prev );
    current->prev->next = current;
    current = current->prev;
    current->prev = &List; 
    List.next = current;
  }
  return current->line;
}

char *listContents() {
  return( current->line );
}

void *listGetPosition() {
  return( current );
}

void listSetPosition( void *node ) {
  current = (Node *) node;
}

void freeNode( Node *new ) {
  free( new->line );
  free( new );
}

int endList() {
  return( current->next == &List );
}

void listDestroy() {
  current = List.next;
  while( current != &List ) {
    Node *temp = current;
    current = current->next;
    freeNode( temp );
  }
}

