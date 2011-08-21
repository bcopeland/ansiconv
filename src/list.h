void listInit( int width, void (*init)(char *buf, int size));
void listRewind();
int listHeight();
int listWidth();
char *listForward();
char *listBackward();
char *listForwardCat();
char *listBackwardCat();
char *listContents();
int endList();
void listDestroy();
void *listGetPosition();
void listSetPosition( void *node );


