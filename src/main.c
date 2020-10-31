/*
 *  Main entry point for the ansi executable.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>

#include "ansiconv.h"

static char *input_file_name = NULL;
static char *output_file_name = NULL;
static int have_tokenized = 0;
char x1, x2;

static char option_string[] = "tf:o:g:";

static struct option long_options[] = {
    {"filename",0,0,'x'},
    {"help",0,0,'h'},
    {"input_format",1,0,'f'},
    {"output_file",1,0,'o'},
    {"thumbnail",0,0,'t'},
    {0,0,0,0}
}; 

static char *arg_desc[] = {
    "use FILE for input (optional)", "FILE",
    "display this message", 0,
    "specify the input format. TYPE is (ansi|bin|idf)", "TYPE",
    "use output file name FILE (defaults to stdout)", "FILE",
    "generate thumbnail image", 0
};

static void clean_exit() 
{
    if (input_file_name) 
        free (input_file_name);
    if (output_file_name) 
        free (output_file_name);

    exit(0); 
}

static void long_usage(char *program) 
{
    int i = 0;

    int space_col = 30;
    int extra_space = 0;

    printf("\nUsage: %s <options> <file>\n\n"
            "Options are the following:\n", program);

    while (long_options[i].name != 0) {
        if (arg_desc[2*i+1])
        {
            extra_space = space_col - 
                (strlen(arg_desc[2*i+1]) + strlen(long_options[i].name) + 7);
            if (extra_space < 0) extra_space = 0;

            printf("-%c, --%s=%s%*c%s\n", long_options[i].val, 
                    long_options[i].name, arg_desc[2*i+1], extra_space,
                    ' ', arg_desc[2*i]);
        } else {
            extra_space = space_col - (strlen(long_options[i].name) + 6);
            if (extra_space < 0) extra_space = 0;

            printf("-%c, --%s%*c%s\n", long_options[i].val, 
                    long_options[i].name, extra_space, ' ',
                    arg_desc[2*i]);
        }
        i++;
    }
    printf("\n");
    clean_exit();
}

static void usage_error( char *program,  char *string ) 
{
    if (string) 
        printf ("\nError: %s", string) ;
    
    printf ("\n\
Usage: \n\
%s [-fgot] [--input_format {ansi,bin,idf}] \n\
%*c [--graphic_format {gif,png}] [--output_file {file}] name... \n\
\n\
Use --help for more info.\n\n", program, strlen(program), ' ');

    clean_exit();
}

/* returns the option character from command line */
static int get_next_option( int argc, char *argv[], char *optstring,
                     struct option *long_opts) 
{
    char *name_string;

#ifdef HAVE_GETOPT_H
    return getopt_long( argc, argv, optstring, long_opts, NULL );
#else
    return getopt( argc, argv, optstring );
#endif   
}
        

int main(int argc, char *argv[]) 
{
    char *extension;
    int input_format = -1;
    int output_format = -1;
    int thumbnail = 0;
    FILE *in = stdin, *out;
    char option;

    /* get arguments */
    while (( option =  get_next_option( argc, argv, option_string, 
                    long_options)) != -1 ) {
        switch(option) {
            case 'h':
                long_usage( argv[0] );
                break;
            case 'f':
                if ( !strcasecmp( optarg, "ansi" ))
                    input_format = ANSI;
                else if ( !strcasecmp( optarg, "bin" ))
                    input_format = BIN;
                else if ( !strcasecmp( optarg, "idf" ))
                    input_format = IDF;
                else 
                    usage_error( argv[0], "invalid input format specified" );
                break;
            case 't':
                thumbnail = 1;
                break;
            case 'o':
                output_file_name = strdup(optarg);
                break;
            case 'x':
                input_file_name = strdup(optarg);
                break;
            default:
                break;
        }
    }

    if (!input_file_name && optind < argc) 
          input_file_name = strdup(argv[optind]);

    output_format = PNG;

    if (input_format == -1) {
        input_format = ANSI;
        if (input_file_name)
        {
            extension = strrchr( input_file_name, '.' );
            if (extension) {
                if (!strcasecmp( extension+1, "bin" ))
                    input_format = BIN;
                else if (!strcasecmp( extension+1, "idf" ))
                    input_format = IDF;
            }
        }
    }

    if (input_file_name) {
        in = freopen(input_file_name, "rb", stdin);
        if (!in) {
            fprintf(stderr, "Could not open %s for read\n", input_file_name);
            clean_exit();
        }
    }
    
    if (output_file_name) {
        out = freopen(output_file_name, "wb", stdout);
        if (!out) {
            fprintf(stderr, "Could not open %s for write\n", output_file_name);
            clean_exit();
        }
    }

    if (input_file_name) 
        free(input_file_name);
    if (output_file_name) 
        free(output_file_name);

    if (ansiconv_convert(in, input_format, output_format, thumbnail))
        fprintf(stderr, "Error parsing files\n");
}
