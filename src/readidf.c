/*
 *  Reads an IDF into a linked list, and updates any useful variables. 
 */
#include <stdio.h>
#include "list.h"
#include "dosfont.h"

/* some useful constants */
#define TERMIDF     199    // Number of lines (fixed?!)
#define FONTSIZE   4096    // Number of bytes for the font
#define PALSIZE  16 * 3    // Number of bytes for the palette
#define HDRSIZE      12    // Number of bytes for the header

static char *line;
static unsigned int width;
static unsigned int height;

void store( char ch, char attrib, int x ) {
  line[x*2] = ch;
  line[x*2 + 1] = attrib;
  if ( x == width - 1 ) {
    line = listForwardCat();
  }
}

int idfRead( char *filename ) {

  FILE *fp;
  int i, j;
  unsigned int runlength;
  unsigned char attrib, ch;

  fp = fopen( filename, "rb" );
  if (!fp) 
    return( -1 );

  /* lazily read the apropos portions of the header */ 
  fseek( fp, 8, SEEK_SET );  // byte 9 is width-1
  width = fgetc( fp ) + 1;
  fseek( fp, 1, SEEK_CUR );  // byte 11 is height-1
  height = fgetc( fp ) + 1;
  fseek( fp, 1, SEEK_CUR );
 
  listInit( width*2, NULL );

  line = listForwardCat();
  // read in the characters
  for( i=0; i < width * TERMIDF && !feof( fp ); ) {
    ch = fgetc( fp ); 
    attrib = fgetc( fp ); 

    // handle RLE 
    if ( ch == 1 && attrib == 0 ) {
      /* 
       * This specifies a run of characters.  The following odd looking
       * code is to make this byte-order independent
       */
       ch = fgetc( fp );
       attrib = fgetc( fp );
       runlength = ( ch | ( attrib << 8 ));
       ch = fgetc( fp );
       attrib = fgetc( fp );

       /* 
        * Terminate if run is 0,0 and will end the pic.
        */
       if(( runlength + i >= width * TERMIDF ) && attrib == 0 ) 
         break;

       for( j=0; j<runlength; j++, i++ )  {
         store( ch, attrib, i%width );  
       }
    } else {
      store( ch, attrib, i%width );
      i++;
    }
  }

  // read in the font
  if (!feof( fp )) {
    fread( dosfont, 1, FONTSIZE, fp ); 
  }

  // get palette
  if (!feof( fp )) {
    float r, g, b;

    for( i=0; i<16; i++ ) {
      r = ((float) fgetc( fp )) / 63;
      g = ((float) fgetc( fp )) / 63;
      b = ((float) fgetc( fp )) / 63;
      
      color_table[ i ] = ((unsigned char)( r * 255.0)) << 16 |
                         ((unsigned char)( g * 255.0)) << 8  |
                         ((unsigned char)( b * 255.0));   
    }
  }
  return 1;
}

