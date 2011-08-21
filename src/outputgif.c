/*
 *  Giflib output driver 
 */
#include <stdio.h>

#ifdef HAVE_LIBGIF
#include <gif_lib.h>

static GifFileType *gif;
static ColorMapObject *gifcmap;
   
int gifOutputSetup( int height, int width, int *palette ) {
  int i;

  gif = EGifOpenFileHandle( 1 );

  /* This can occur if giflib is installed with RPM */
  if (!gif) {
    fprintf( stderr, "Cannot open stdout (missing encode routines?)\n" );
    return 0;
  }

  gifcmap = MakeMapObject( 256, NULL );

  for ( i=0; i<256; i++ ) {
    gifcmap->Colors[ i ].Red   = (palette[i] & 0xff0000) >> 16;
    gifcmap->Colors[ i ].Green = (palette[i] & 0xff00) >> 8;
    gifcmap->Colors[ i ].Blue  = (palette[i] & 0xff); 
  }

  EGifPutScreenDesc( gif, width, height, gifcmap->BitsPerPixel, 0, gifcmap );
  EGifPutImageDesc( gif, 0, 0, width, height, FALSE, NULL ); 

  return 1;
}

int gifOutputBlock( char *pixels, int height, int width ) {

  EGifPutLine( gif, pixels, height * width );
  return 1;
}

int gifOutputFinish() {
  EGifCloseFile( gif );
}

#else

int gifOutputSetup( int height, int width, char *palette ) {
  fprintf( stderr, "GIF output option not enabled at compile time\n" );
  return 0;
}

int gifOutputBlock( char *pixels, int height, int width ) {
  return 0;
}

int gifOutputFinish() {
  return 0;
}

#endif
