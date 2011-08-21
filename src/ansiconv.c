/*
 *  Main entry point for the ansi cgi and the general executable.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_GETOPT_H
#include <getopt.h>
#else 

extern char *optarg;
extern int optind, opterr, optopt;

struct option {
  const char *name;
  int has_arg;
  int *flag;
  int val;
};
#endif

#include "output.h"
#include "outputgif.h"
#include "outputpng.h"
#include "list.h"

enum graphics_drivers { GIF, PNG };
enum input_formats { ANSI, BIN, IDF };

static char *cgi_parameter_string; 
static char *cgi_value_string;
static char *input_file_name = NULL;
static char *output_file_name = NULL;
static int have_tokenized = 0;
char x1, x2;

void clean_exit() {

  if (input_file_name) 
    free (input_file_name);
  if (output_file_name) 
    free (output_file_name);

  exit(0); 
}


/* Converts those "%YY" hex strings to the appropriate characters */
void http_canonicalize( char *string ) {
  while ( *string ) {
    if ( *string == '+')
      *string = ' ';
    if ( *string == '%' && isxdigit( x1 = *(string+1) ) &&
         isxdigit( x2 = *(string+2) )) { 
      *string = 16 * ((x1 > '9') ? 10 + tolower( x1 ) - 'a' : x1 - '0') 
                   + ((x2 > '9') ? 10 + tolower( x2 ) - 'a' : x2 - '0');
      strcpy(string+1, string+3) ;
    }
    string++;
  }
}

void usage_error( char *string ) {

  if (string) {
    printf ("\nError: %s", string) ;
  }
  printf ("\n\
Usage: \n\
ansiconv [-fgot] [--input_format {ansi,bin,idf}] \n\
         [--graphics_format {gif,png}] [--output_file {file}] \n\
         [--help] name... \n\
          \n");

  clean_exit();
}

/* returns the option character from command line or CGI */
int get_next_option( int argc, char *argv[], char *optstring,
                     struct option *long_opts, int is_cgi )  {

  char *name_string;

  if ( is_cgi ) {
    if ( !have_tokenized ) {
      name_string = strtok( cgi_parameter_string, "=" );
      have_tokenized = 1;
    } else {
      name_string = strtok( NULL, "=" );
    }
  
    if ( !name_string ) 
      return -1;

    http_canonicalize( name_string );

    cgi_value_string = strtok( NULL, "&" );
    if ( !cgi_value_string )
      return -1;
 
    http_canonicalize( cgi_value_string );
    while (long_opts->name) {
      if (!strcasecmp(long_opts->name, name_string)) {
        optarg = cgi_value_string ;
        return long_opts->val ;  
      }
      long_opts++;
    }
    return -1;
    
  } else {

#ifdef HAVE_GETOPT_H
    return getopt_long( argc, argv, optstring, long_opts, NULL );
#else
    return getopt( argc, argv, optstring );
#endif   

  }
}
        

int main(int argc, char *argv[]) {

  char *extension;
  int input_format = -1;
  int output_format = -1;
  int thumbnail = 0;
  FILE *fp;
  output_info driver;
  int is_cgi = 0;
  char option;
  int result;

  char option_string[] = "tf:o:g:";

  static struct option long_options[] = {
    {"graphic_format",1,0,'g'},
    {"input_format",1,0,'f'},
    {"output_file",1,0,'o'},
    {"thumbnail",0,0,'t'},
    {"help",0,0,'?'},
    {"filename",0,0,'x'},
    {0,0,0,0}
  };  

  /* get arguments */
  is_cgi = ( argc < 2 ) && 
           ( getenv( "REQUEST_METHOD" ) && 
           ( cgi_parameter_string = getenv( "QUERY_STRING" )));

  while (( option =  get_next_option( argc, argv, option_string, 
                                      long_options, is_cgi )) != -1 ) {
    switch(option) {
      case '?':
        usage_error( NULL );
        break;
      case 'g':
        if ( !strcasecmp( optarg, "gif" ))
          output_format = GIF;
        else if ( !strcasecmp( optarg, "png" ))
          output_format = PNG;
        else 
          usage_error( "invalid output format specified" );
        break;
      case 'f':
        if ( !strcasecmp( optarg, "ansi" ))
          input_format = ANSI;
        else if ( !strcasecmp( optarg, "bin" ))
          input_format = BIN;
        else if ( !strcasecmp( optarg, "idf" ))
          input_format = IDF;
        else 
          usage_error( "invalid input format specified" );
        break;
      case 't':
        thumbnail = 1;
        break;
      case 'o':
        output_file_name = (char *) malloc( (strlen(optarg) + 1) * 
                                            sizeof (char) );  
        strcpy( output_file_name, optarg );
        break;
      case 'x':
        input_file_name = (char *) malloc( (strlen(optarg) + 1) * 
                                            sizeof (char) );  
        strcpy( input_file_name, optarg );
        break;
      default:
        break;
    }
  }

  if (!input_file_name && !is_cgi) {

    if (optind >= argc) 
      usage_error( "No input file specified\n" );

    input_file_name = (char *) malloc( (strlen(argv[optind]) + 1) * 
                                        sizeof (char) );  
    strcpy( input_file_name, argv[optind] );
  } 

  if (!input_file_name && is_cgi) {
    printf ("Content-type: text/html\n\n<H2>No file specified</H2>\n");
    clean_exit();
  }

  if (output_format == -1) {
    output_format = GIF;
    if (output_file_name) {
      extension = strrchr( output_file_name, '.' );
      if (extension) {
        if (!strcasecmp( extension+1, "gif" ))
          output_format = GIF;
        else if (!strcasecmp( extension+1, "png" ))
          output_format = PNG;
      }
    }
  }

  extension = strrchr( input_file_name, '.' );
  if (input_format == -1) {
    input_format = ANSI;
    if (extension) {
      if (!strcasecmp( extension+1, "ice" ) ||
          !strcasecmp( extension+1, "ans"  ) ||
          !strcasecmp( extension+1, "ansi" ))
        input_format = ANSI;
      else if (!strcasecmp( extension+1, "bin" ))
        input_format = BIN;
      else if (!strcasecmp( extension+1, "idf" ))
        input_format = IDF;
    }
  }

  /* security check */
  if (is_cgi) {
    if ((strcasecmp( extension+1, "ice" ) != 0) && 
        (strcasecmp( extension+1, "ans" ) != 0) &&
        (strcasecmp( extension+1, "ansi" )!= 0) &&
        (strcasecmp( extension+1, "bin" ) != 0) &&
        (strcasecmp( extension+1, "idf" ) != 0)) {

      printf ("Content-type: text/html\n\n<H2>Permission denied</H2>\n");
      clean_exit();
    }
  }

  switch (output_format) 
    { 
      case GIF:
        driver.output_setup = gifOutputSetup; 
        driver.output_block = gifOutputBlock; 
        driver.output_finish = gifOutputFinish; 
        break;
      case PNG:
        driver.output_setup = pngOutputSetup; 
        driver.output_block = pngOutputBlock; 
        driver.output_finish = pngOutputFinish; 
        break;
    }

  
  if ( !is_cgi && output_file_name) {
    fp = freopen( output_file_name, "wb", stdout );
    if (!fp) {
      fprintf( stderr, "Could not open %s for write\n", output_file_name );
      clean_exit();
    }
  }

  switch( input_format ) {
    case ANSI:
      result = ansiParse( input_file_name );
      break;
    case BIN:
      result = binRead( input_file_name );
      break;
    case IDF:
      result = idfRead( input_file_name );
      break;
    default:
      result = -1;
      break;
  }
  if ( result == -1 ) {
    if ( !is_cgi ) {
      fprintf( stderr, "Error reading file.\n" );
    } else {
      printf ("Content-type: text/html\n\n<H2>Error reading file.</H2>\n");
    }
    clean_exit();
  }


  if (input_file_name) free (input_file_name);
  if (output_file_name) free (output_file_name);
  input_file_name = output_file_name = NULL;

  if ( is_cgi ) {
    printf ("Content-type: image/gif\n\n");
    fflush( stdout );
  }

  if ( thumbnail )
    outputThumbImage( &driver );
  else
    outputImage( &driver );

  listDestroy();

  exit(0);
} 
