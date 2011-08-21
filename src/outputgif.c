/*
 *  Giflib output driver 
 */
#include <stdio.h>
#include <stdlib.h>

#include "outputgif.h"

#ifdef HAVE_LIBGIF
#include <gif_lib.h>

typedef struct {
    GifFileType *gif;
    ColorMapObject *gifcmap;
} giflib_context_t;
   
void *gif_output_setup(int height, int width, int *palette, int num_entries) 
{
    int i;

    giflib_context_t *ctx = malloc(sizeof(giflib_context_t));
    if (!ctx)
        return 0;

    ctx->gif = EGifOpenFileHandle(1);
    /* This can occur if giflib is installed with RPM */
    if (!ctx->gif) {
        fprintf( stderr, "Cannot open stdout (missing encode routines?)\n" );
        free(ctx);
        return 0;
    }

    ctx->gifcmap = MakeMapObject(num_entries, NULL);

    for ( i=0; i<num_entries; i++ ) {
        ctx->gifcmap->Colors[ i ].Red   = (palette[i] & 0xff0000) >> 16;
        ctx->gifcmap->Colors[ i ].Green = (palette[i] & 0xff00) >> 8;
        ctx->gifcmap->Colors[ i ].Blue  = (palette[i] & 0xff); 
    }

    EGifPutScreenDesc(ctx->gif, width, height, ctx->gifcmap->BitsPerPixel, 0, 
            ctx->gifcmap);
    EGifPutImageDesc(ctx->gif, 0, 0, width, height, FALSE, NULL); 

    return ctx;
}

void gif_output_block(void *user, char *pixels, int height, int width) 
{
    giflib_context_t *ctx = (giflib_context_t *) user;
    EGifPutLine(ctx->gif, pixels, height * width );
}

void gif_output_finish(void *user) 
{
    giflib_context_t *ctx = (giflib_context_t *) user;
    EGifCloseFile(ctx->gif);
    free(ctx);
}

#else

void *gif_output_setup(int height, int width, int *palette, int num_entries) 
{
    fprintf(stderr, "GIF output option not enabled at compile time\n");
    return 0;
}

void gif_output_block(void *user, char *pixels, int height, int width) 
{
}

void gif_output_finish(void *user) 
{
}

#endif
