#ifndef ANSICONV_H
#define ANSICONV_H

typedef enum {
    PNG,
    GIF 
} output_format_t;

typedef enum {
    ANSI, BIN, IDF 
} input_format_t;

int ansiconv_convert(FILE *in, input_format_t in_format, 
                     output_format_t out_format, int thumbnail);

#endif
