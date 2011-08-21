/*
 *  Main entry point for the IDF cgi and the general executable.
 */
#include <stdio.h>
#include <stdlib.h>
#include "list.h"

void main(int argc, char *argv[]) {
  char *file;
  FILE *fp;

  // first check QUERY_STRING to see if CGI
  file = getenv("QUERY_STRING");
  if ( !file ) {

    // Assume command line 
    if ( argc < 2 ) {
      printf( "USAGE: %s <in_file> [out_file]\n", argv[0] );
      exit(1);
    } 

    if ( argc > 2 ) {
      fp = freopen( argv[2], "wb", stdout );
      if (!fp) {
        fprintf( stderr, "Could not open %s for write\n", argv[2] );
        exit(1);
      }

    }
    if ( idfRead( argv[1] ) == -1 ) {
      fprintf( stderr, "Error in IDF reader (file not found?)\n" );
      exit(1);
    }
    outputGif();
    if ( argc > 2 )  
      fclose( fp );
  } else {
    if ( idfRead( file ) == -1 ) {
      printf ("Content-type: text/html\n\n<H2>Error in file spec</H2>\n");
    } else {
      printf ("Content-type: image/gif\n\n");
      fflush( stdout );
      outputGif();
    }
  }
} 
