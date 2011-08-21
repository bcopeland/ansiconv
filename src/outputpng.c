/*
 *  Converts a BIN to a PNG image file.
 */
#include <stdio.h>
#include <png.h>
#include "list.h"
#include "dosfont.h" 

void rasterizeCharacter( unsigned char ch, int x, int y, 
                         char *framebuf, int width, 
                         char fgindex, char bgindex );

/*
 *  Outputs a PNG to stdout.  Assumes that the list has been 
 *  filled in appropriately.
 */
void outputImage() {

  int x, y;
  int i;

  char *ansibuf; 
  png_bytep row_pointers[FNHEIGHT] ;
  char *line;
  unsigned int ansibufsize;
  FILE *fp;
  int width, height;
  png_structp png_ptr;
  png_infop info_ptr;

  png_color png_palette[256] ;

  width = listWidth() / 2;
  height = listHeight();
  listRewind();
  ansibufsize = FNWIDTH * FNHEIGHT * width;
  ansibuf = (char *) malloc (ansibufsize);

  fp = fdopen( 1, "wb" );
  png_ptr = png_create_write_struct( PNG_LIBPNG_VER_STRING,
                                     NULL, NULL, NULL );
  if (png_ptr)
    info_ptr = png_create_info_struct( png_ptr );

  if (!fp || !png_ptr || !info_ptr || setjmp(png_ptr->jmpbuf)) 
  {
    fclose( fp );
    free( ansibuf );
    if (png_ptr) png_destroy_write_struct( &png_ptr, (png_infopp) NULL );
    return;
  }

  png_init_io( png_ptr, fp );
  png_set_IHDR( png_ptr, info_ptr, width*FNWIDTH, height*FNHEIGHT, 
                8, PNG_COLOR_TYPE_PALETTE, PNG_INTERLACE_NONE, 
                PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE );

  for ( i=0; i<256; i++ ) {
    png_palette[ i ].red   = (color_table[i] & 0xff0000) >> 16;
    png_palette[ i ].green = (color_table[i] & 0xff00) >> 8;
    png_palette[ i ].blue  = (color_table[i] & 0xff); 
  }
  png_set_PLTE(png_ptr, info_ptr, png_palette, 256);

  png_write_info( png_ptr, info_ptr );

  /* setup row pointers */
  for ( i=0; i<FNHEIGHT; i++ ) {
    row_pointers[ i ] = (png_bytep) &ansibuf[ i * FNWIDTH * width ];  
  }

  for (y=0; y<height; y++) {
    line = listForward();
    for (x=0; x<width; x++) {
      unsigned char attrib = line[x * 2 + 1];
      rasterizeCharacter( line[x * 2], x, 0, ansibuf, width, 
                          attrib & 0x0f, attrib >> 4 );  
    }
    png_write_rows( png_ptr, row_pointers, FNHEIGHT );  
  }
  png_write_end(png_ptr, info_ptr);
  png_destroy_write_struct(&png_ptr, (png_infopp)NULL);

  fclose( fp );
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

