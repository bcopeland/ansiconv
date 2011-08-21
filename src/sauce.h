#ifndef INC_SAUCE
#define INC_SAUCE

#include <stdio.h>

typedef struct _sc {
  char ID[5];
  char Version[2];
  char Title[35];
  char Author[20];
  char Group[20];
  char Date[8];
  long FileSize;
  unsigned char DataType;
  unsigned char FileType;
  unsigned short TInfo1;
  unsigned short TInfo2;
  unsigned short TInfo3;
  unsigned short TInfo4;
  unsigned char Comments;
  unsigned char Flags;
  char Filler[22];
} Sauce;

Sauce *getSauce( FILE *fp );

#endif
