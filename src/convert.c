#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ansiconv.h"
#include "output.h"
#include "outputgif.h"
#include "outputpng.h"
#include "dosfont.h"

list_t *ansi_parse(FILE *fp, struct font_info *fi);
list_t *bin_read(FILE *fp, struct font_info *fi);
list_t *idf_read(FILE *fp, struct font_info *fi);

/*
 *  Output goes to stdout or whatever has been freopened beforehand.
 */
int ansiconv_convert(FILE *in, input_format_t in_format, 
             output_format_t out_format, int thumbnail)
{
    list_t *result;
    output_info driver;
    struct font_info *font;

    font = malloc(sizeof(struct font_info));
    if (!font)
        return -1;

    memcpy(font->color_table, default_color_table, sizeof(font->color_table));
    memcpy(font->font, dosfont, sizeof(font->font));
    font->color_table_entries = default_color_table_entries;

    switch (out_format) 
    { 
        case GIF:
            driver.output_setup = gif_output_setup; 
            driver.output_block = gif_output_block; 
            driver.output_finish = gif_output_finish; 
            break;
        case PNG:
            driver.output_setup = png_output_setup; 
            driver.output_block = png_output_block; 
            driver.output_finish = png_output_finish; 
            break;
    }

    switch (in_format) 
    {
        case ANSI:
            result = ansi_parse(in, font);
            break;
        case BIN:
            result = bin_read(in, font);
            break;
        case IDF:
            result = idf_read(in, font);
            break;
        default:
            result = NULL;
            break;
    }

    if (!result)
    {
        free(font);
        return -1;
    }

    if (thumbnail)
        output_thumb_image(&driver, result, font);
    else
        output_image(&driver, result, font);

    list_destroy(result);
    return 0;
}
