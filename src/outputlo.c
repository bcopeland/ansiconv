/*
 *  Converts a BIN to a gif, using giflib.  Reduces it to an 80-wide 
 *  pixmap by an almost clever color reduction algorithm, known loosely
 *  as first-come-first-serve (FCFS) along with a RGB color cube search
 *  for the really psychadelic ansis with > 256 colors.
 */
#include <stdio.h>
#include <gif_lib.h>
#include "list.h"
#include "dosfont.h" 

#ifndef MAX_DBL
#define MAX_DBL 1e30
#endif 

void rasterizeCharacter( unsigned char ch, int x, int y, 
                         char *framebuf, int width, 
                         char fgindex, char bgindex );

/* stores a .75/.25 and .5/.5 mapping table for fg & bg mixtures */
static unsigned char shade_table[ 16 * 16 * 2 ];

int mixColor( unsigned int c1, unsigned int c2, double ptc1 ) {
  int i; 
  unsigned char rgb;
  float cm1, cm2;
  int retval = 0xff000000;

  for ( i = 0; i < 3; i++ ) {
    cm1 = (double) ((c1 >> (i << 3)) & 0xff);
    cm2 = (double) ((c2 >> (i << 3)) & 0xff);
    rgb = (unsigned char) ( cm1 * ptc1 + cm2 * ( 1.0-ptc1 ));
    retval |= ( rgb & 0xff ) << ( i << 3 );
  }
  return( retval );
}

/* 
 * Adds the color to the palette, returning a close match if no
 * more space in the table.
 */ 
unsigned char addColor( unsigned int rgb ) {
  int i,j;
  double d_best = MAX_DBL, d, x; 
  unsigned char best_col = 16;
  double rgb_conv[3];

  if ( color_table_entries < 256 ) {
    color_table[ color_table_entries ] = rgb;
    color_table_entries++; 
    return( color_table_entries-1 );
  } else {
    /* seperate rgb into components */
    for( i=0; i<3; i++ ) 
      rgb_conv[ i ] = (double) ((rgb >> ( i << 3 )) & 0xff);

    /* linear search using manhattan distance in RGB space */
    for ( i=0; i<256; i++ ) {
      d = 0;
      for ( j=0; j<3; j++ ) {
        x = rgb_conv[ j ] - (double) ((color_table[i] >> (j << 3)) & 0xff);
        d += x * x;
      } 
      if ( d < d_best ) {
        d_best = d;
        best_col = i;
      }
    }
    return( best_col );
  } 
}

/*
 *  Returns a byte representing the palette entry for a color with
 *  the foreground color fg, background color bg, and shade 0-4 from
 *  most foreground to least (technically, four times the fraction of
 *  background color).  Reduces all duplicate entries, I hope.  
 */
unsigned char getColor( int fg, int bg, int shade ) {
  unsigned char col;
  int rgb;

  if ( fg == bg ) {   /* No shade if both same */
    col = fg;
  } else {
    switch (shade) {
      case 0:          /* 1 fg, 0 bg */
        col = fg;
        break;
      case 1:          /* 3/4 fg, 1/4 bg */
        if(( col = shade_table[ bg + fg * 16 ]) == 0 ) {
          rgb = mixColor( color_table[ fg ], color_table[ bg ], .75 );
          col = addColor( rgb ); 
          shade_table[ bg + fg * 16 ] = col;
        } 
        break;
      case 2:           /* 1/2 fg, 1/2 bg */
        if(( col = shade_table[ 256 + bg + fg * 16 ]) == 0 ) {
          rgb = mixColor( color_table[ fg ], color_table[ bg ], .5 );
          col = addColor( rgb ); 
          shade_table[ 256 + bg + fg * 16 ] = col;
          shade_table[ 256 + fg + bg * 16 ] = col;
        }
        break;
      case 3:           /* 1/4 fg, 3/4 bg */
        if(( col = shade_table[ fg + bg * 16 ]) == 0 ) {
          rgb = mixColor( color_table[ fg ], color_table[ bg ], .25 );
          col = addColor( rgb ); 
          shade_table[ fg + bg * 16 ] = col;
        }
        break;
      case 4:           /* 0 fg, 1 bg */
        col = bg;
        break;
      default: 
        col = -1;
        break;
    }
  }
  return( col ); 
}

int isTop( unsigned char ch ) {
  // false only if a-z, punctuation, and bottom blk (0xdc).
  if ( ch == 0xdc || 
       ch == '_'  ||
       ch == '.'  ||
       ch == ','  ||
       ch == '-'  ||
       ( ch >= 'a' && ch <= 'z' )) {
    return 0;
  }
  return 1;
}

int isBottom( unsigned char ch ) {
  // false only if top blk (0xdf) or quotes 
  if ( ch == 0xdf || 
       ch == '\'' || 
       ch == '\"' ) {
    return 0;
  }
  return 1;
}

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
  height = listHeight() * 2;
  listRewind();
  ansibufsize = width * height * 2;
  ansibuf = (char *) malloc (ansibufsize);

  memset( shade_table, 0, 16 * 16 * 2 ); 

  gif = EGifOpenFileHandle( 1 );
  if (!gif) {
    free (ansibuf);
    return;
  }

  for (y=0; y<height; y++) {
    line = listForward();
    for (x=0; x<width; x++) {
      unsigned char attrib = line[x * 2 + 1];
      rasterizeCharacter( line[x * 2], x, y, ansibuf, width, 
                          attrib & 0x0f, attrib >> 4 );  
    }
  }

  gifcmap = MakeMapObject( 256, NULL );
   
  for ( i=0; i<256; i++ ) {
    gifcmap->Colors[ i ].Red   = (color_table[i] & 0xff0000) >> 16;
    gifcmap->Colors[ i ].Green = (color_table[i] & 0xff00) >> 8;
    gifcmap->Colors[ i ].Blue  = (color_table[i] & 0xff); 
  }
  EGifPutScreenDesc( gif, width, height, 
                     gifcmap->BitsPerPixel, 0, gifcmap );
  EGifPutImageDesc( gif, 0, 0, width, height, FALSE, NULL ); 


  EGifPutLine( gif, ansibuf, width*height );  

  EGifCloseFile( gif );
  free (ansibuf);
}

/* 
 * Easy version of the other one that is used for the full  
 * conversion, this routine just determines what character 
 * we're dealing with, calls the appropriate routines to get
 * the color index, then boom! it's done.
 */
void rasterizeCharacter( unsigned char ch, int x, int y, 
                         char *framebuf, int width, 
                         char fgindex, char bgindex ) {

  unsigned char color = 0;
  switch ( ch ) {
    case 0xb0:         // 1/4 blk 
      color = getColor( fgindex, bgindex, 3 );
      break;
    case 0xb1:         // 1/2 blk
      color = getColor( fgindex, bgindex, 2 );
      break;
    case 0xb2:         // 3/4 blk
      color = getColor( fgindex, bgindex, 1 );
      break;
    case 0:
    case 255:
    case ' ':
      color = getColor( fgindex, bgindex, 4 );
      break;
    default:
      color = getColor( fgindex, bgindex, 0 );
      break;
  }
 
  if ( isTop( ch )) 
    framebuf[ ( y * 2 ) * width + x ] = color;
  else
    framebuf[ ( y * 2 ) * width + x ] = bgindex;
  if ( isBottom( ch )) 
    framebuf[ ( y * 2 + 1 ) * width + x ] = color;
  else
    framebuf[ ( y * 2 + 1 ) * width + x ] = bgindex;
}

