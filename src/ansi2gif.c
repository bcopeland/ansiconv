/*
 *  Main entry point for the ansi cgi and the general executable.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

void main(int argc, char *argv[]) {
  char *file;
  char *extension;
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
    if ( ansiParse( argv[1] ) == -1 ) {
      fprintf( stderr, "Error in parser (file not found?)\n" );
      exit(1);
    }
    outputGif();
    if ( argc > 2 )  
      fclose( fp );
  } else {
    extension = strrchr( file, '.' );
    if (!extension || (strcasecmp( extension+1, "ice" ) != 0 && 
                       strcasecmp( extension+1, "ans" ) != 0 &&
                       strcasecmp( extension+1, "ansi" ) != 0)) {
      printf ("Content-type: text/html\n\n<H2>Permission denied</H2>\n");
      exit(0); 
    }
    if ( ansiParse( file ) == -1 ) {
      printf ("Content-type: text/html\n\n<H2>Error in file spec</H2>\n");
    } else {
      printf ("Content-type: image/gif\n\n");
      fflush( stdout );
      outputGif();
    }
  }
} 
