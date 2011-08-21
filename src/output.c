/*
 *  This code generates a pixmap of an Ansi/Bin/IDF using the
 *  caller's output driver of choice. 
 */
#include <stdio.h>
#include "list.h"
#include "dosfont.h" 
#include "output.h"

void rasterizeCharacter( unsigned char ch, int x, int y, 
                         char *framebuf, int width, 
                         char fgindex, char bgindex );

/*
 *  Outputs an image to stdout.  Assumes that the list has been 
 *  filled in appropriately.
 */
void outputImage( output_info *driver ) {

  int x, y;
  int i;

  char *ansibuf; 
  char *line;
  unsigned int ansibufsize;
  FILE *fp;
  int width, height;

  width = listWidth() / 2;
  height = listHeight();
  listRewind();
  ansibufsize = FNWIDTH * FNHEIGHT * width;
  ansibuf = (char *) malloc (ansibufsize);

  if (!driver->output_setup( height * FNHEIGHT, width * FNWIDTH,
                             color_table )) {
    free (ansibuf);
    return;
  }   

  for (y=0; y < height; y++) {
    line = listForward();
    for (x=0; x<width; x++) {
      unsigned char attrib = line[x * 2 + 1];
      rasterizeCharacter( line[x * 2], x, 0, ansibuf, width, 
                          attrib & 0x0f, attrib >> 4 );  
    }
    driver->output_block( ansibuf, FNHEIGHT, width * FNWIDTH );
  }

  driver->output_finish();
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

