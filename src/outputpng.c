/*
 *  libpng output driver 
 */
#include <stdio.h>

#ifdef HAVE_LIBPNG
#include <png.h>

static png_structp png_ptr;
static png_infop info_ptr;
static FILE *fp;
static png_color png_palette[256];

int pngOutputSetup( int height, int width, int *palette ) {

  int i;

  fp = fdopen( 1, "wb" );
  png_ptr = png_create_write_struct( PNG_LIBPNG_VER_STRING,
                                     NULL, NULL, NULL );
  if (png_ptr)
    info_ptr = png_create_info_struct( png_ptr );

  if (!fp || !png_ptr || !info_ptr || setjmp(png_ptr->jmpbuf)) 
  {
    fprintf( stderr, "PNG Write Error\n" );
    fclose( fp );
    if (png_ptr) png_destroy_write_struct( &png_ptr, (png_infopp) NULL );
    return 0;
  }

  png_init_io( png_ptr, fp );
  png_set_IHDR( png_ptr, info_ptr, width, height, 
                8, PNG_COLOR_TYPE_PALETTE, PNG_INTERLACE_NONE, 
                PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE );

  for ( i=0; i<256; i++ ) {
    png_palette[ i ].red   = (palette[i] & 0xff0000) >> 16;
    png_palette[ i ].green = (palette[i] & 0xff00) >> 8;
    png_palette[ i ].blue  = (palette[i] & 0xff); 
  }
  png_set_PLTE(png_ptr, info_ptr, png_palette, 256);

  png_write_info( png_ptr, info_ptr );

  return 1;
}

int pngOutputBlock( char *pixels, int height, int width ) {
  
  int i;

  for (i=0; i<height; i++) 
     png_write_row( png_ptr, (png_bytep) &pixels[width*i] );  
  return 1;
}

int pngOutputFinish() {
  png_write_end(png_ptr, info_ptr);
  png_destroy_write_struct(&png_ptr, (png_infopp)NULL);

  fclose( fp );
  return 1;
}

#else

int pngOutputSetup( int height, int width, char *palette ) {
  fprintf( stderr, "PNG output option not enabled at compile time\n" );
  return 0;
}

int pngOutputBlock( char *pixels, int height, int width ) {
  return 0;
}

int pngOutputFinish() {
  return 0;
}

#endif

