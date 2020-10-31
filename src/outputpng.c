/*
 *    libpng output driver 
 */
#include <stdio.h>
#include <stdlib.h>

#include "outputpng.h"

#include <png.h>

typedef struct {
    png_structp png_ptr;
    png_infop info_ptr;
    png_color png_palette[256];
    int bpp;
    FILE *fp;
} png_context_t;

void *png_output_setup(int height, int width, int *palette, int num_entries) 
{
    int i;

    png_context_t *ctx = malloc(sizeof(png_context_t));
    if (!ctx)
        return 0;

    ctx->fp = fdopen(1, "wb");
    ctx->png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
                                           NULL, NULL, NULL);
    if (ctx->png_ptr)
        ctx->info_ptr = png_create_info_struct(ctx->png_ptr);

    if (!ctx->fp || !ctx->png_ptr || !ctx->info_ptr)
    {
        fclose(ctx->fp);
        if (ctx->png_ptr)
            png_destroy_write_struct(&ctx->png_ptr, (png_infopp) NULL);
        free(ctx);
        return 0;
    }
    ctx->bpp = (num_entries == 16) ? 4 : 8;

    png_init_io(ctx->png_ptr, ctx->fp);
    png_set_IHDR(ctx->png_ptr, ctx->info_ptr, width, height, 
        ctx->bpp, PNG_COLOR_TYPE_PALETTE, PNG_INTERLACE_NONE, 
        PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    for (i=0; i<num_entries; i++) {
        ctx->png_palette[i].red     = (palette[i] & 0xff0000) >> 16;
        ctx->png_palette[i].green = (palette[i] & 0xff00) >> 8;
        ctx->png_palette[i].blue    = (palette[i] & 0xff); 
    }
    png_set_PLTE(ctx->png_ptr, ctx->info_ptr, ctx->png_palette, num_entries);
    png_write_info(ctx->png_ptr, ctx->info_ptr);

    return ctx;
}

// inplace conversion of 8 bpp data to 4 bpp
void to_four_bit(char *pixels, int count)
{
    int i, j;
    for (i=0, j=0; i < count; i++, j+=2)
        pixels[i] = (pixels[j] & 0xf) << 4 | (pixels[j+1] & 0xf);
}

void png_output_block(void *user, char *pixels, int height, int width) 
{
    int i;

    png_context_t *ctx = (png_context_t *) user;

    for (i=0; i<height; i++) 
    {
        if (ctx->bpp == 4)
            to_four_bit(&pixels[width * i], width);

        png_write_row(ctx->png_ptr, (png_bytep) &pixels[width*i]);    
    }
}

void png_output_finish(void *user)
{
    png_context_t *ctx = (png_context_t *) user;

    png_write_end(ctx->png_ptr, ctx->info_ptr);
    png_destroy_write_struct(&ctx->png_ptr, (png_infopp)NULL);

    fclose(ctx->fp);
    free(ctx);
}
