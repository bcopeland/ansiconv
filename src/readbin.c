/*
 *  Reads a BIN into a linked list.   This is simple.
 */
#include <stdio.h>
#include "list.h"
#include "sauce.h"

#define BINWIDTH 160     // assumption.  I *could* read sauce but... 
#define CTRLZ 0x1a

int binRead( char *filename ) {

  FILE *fp;
  static char *line;
  unsigned int width;
  unsigned int sizeread;
  char ch;
  int i;
  Sauce *s;

  fp = fopen( filename, "rb" );
  if (!fp) 
    return( -1 );

  // try and get width from sauce
  s = getSauce( fp );
  if ( s && s->FileType > 0 ) {
    width = s->FileType * 2;
  } else {
    width = BINWIDTH;
  }
  sizeread = width * 2;
  listInit( width*2, NULL );

  // read in the characters
  while ( !feof( fp ) && sizeread == width*2 ) {
    if (( ch = fgetc( fp )) == CTRLZ ) break;
  
    ungetc( ch, fp );
    line = listForwardCat();
    sizeread = fread( line, 1, width*2, fp );
  }
  // that was easy!
  fclose( fp );
  return 1;
}
