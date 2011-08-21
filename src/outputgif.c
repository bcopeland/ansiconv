/*
 *  Converts a BIN to a gif, using giflib.
 */
#include <stdio.h>
#include <gif_lib.h>
#include "list.h"
#include "dosfont.h" 


void rasterizeCharacter( unsigned char ch, int x, int y, 
                         char *framebuf, int width, 
                         char fgindex, char bgindex );

// Number of lines to display at a time 
#define LINES 5

/*
 *  Outputs a GIF to stdout.  Assumes that the list has been 
 *  filled in appropriately.
 */
void outputGif() {

  int x, y;
  int i;

  char *ansibuf; 
  char *line;
  unsigned int ansibufsize;
  FILE *fp;
  int width, height;
  GifFileType *gif;
  ColorMapObject *gifcmap;

  width = listWidth() / 2;
  height = listHeight();
  listRewind();
  ansibufsize = FNWIDTH * FNHEIGHT * width * LINES;
  ansibuf = (char *) malloc (ansibufsize);

  gif = EGifOpenFileHandle( 1 );
  if (!gif) {
    free (ansibuf);
    return;
  }
  gifcmap = MakeMapObject( 256, NULL );
   
  for ( i=0; i<256; i++ ) {
    gifcmap->Colors[ i ].Red   = (color_table[i] & 0xff0000) >> 16;
    gifcmap->Colors[ i ].Green = (color_table[i] & 0xff00) >> 8;
    gifcmap->Colors[ i ].Blue  = (color_table[i] & 0xff); 
  }
  EGifPutScreenDesc( gif, width*FNWIDTH, height*FNHEIGHT, 
                     gifcmap->BitsPerPixel, 0, gifcmap );
  EGifPutImageDesc( gif, 0, 0, width*FNWIDTH, height*FNHEIGHT, FALSE, NULL ); 


  for (y=0; y<height; y++) {
    line = listForward();
    for (x=0; x<width; x++) {
      unsigned char attrib = line[x * 2 + 1];
      rasterizeCharacter( line[x * 2], x, y % LINES, ansibuf, width, 
                          attrib & 0x0f, attrib >> 4 );  
    }
    for ( i=0; i<FNHEIGHT; i++ ) {
      EGifPutLine( gif, &ansibuf[ ((y % LINES) * FNHEIGHT + i ) * 
                   width * FNWIDTH ], width * FNWIDTH );  
    }
  }

  EGifCloseFile( gif );
  free (ansibuf);
}

void rasterizeCharacter( unsigned char ch, int x, int y, 
                         char *framebuf, int width, 
                         char fgindex, char bgindex ) {

  int i, j;

  char *font_index;
  char font_bitmap;
  char bit;

  font_index = &dosfont[ (unsigned int) (ch * FNHEIGHT) ];
  // 8x16 font is stored as 16 bytes

  for (i=0; i<FNHEIGHT; i++) {
    font_bitmap = *(font_index + i);
    for (j=0; j<FNWIDTH; j++) {
      bit = 1 << (FNWIDTH-j-1);
      if ((font_bitmap & bit) != 0) {
        framebuf[ width * FNWIDTH * (FNHEIGHT * y + i) +
                  x * FNWIDTH + j ] = fgindex;  
      } else {
        framebuf[ width * FNWIDTH * (FNHEIGHT * y + i) +
                  x * FNWIDTH + j ] = bgindex;  
      }
    }
  }
}

