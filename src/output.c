/*
 *  This code generates a pixmap of an Ansi/Bin/IDF using the
 *  caller's output driver of choice. 
 */
#include <stdio.h>
#include <stdlib.h>
#include "list.h"
#include "dosfont.h" 
#include "output.h"

static void rasterize_character(unsigned char ch, int x, int y, 
                 char *framebuf, int width, char fgindex, char bgindex,
                 struct font_info *fi);

/*
 *  Outputs an image to stdout.  Assumes that the list has been 
 *  filled in appropriately.
 */
void output_image(output_info *driver, list_t *l, struct font_info *fi) 
{
    int x, y;
    int i;

    char *ansibuf; 
    char *line;
    unsigned int ansibufsize;
    FILE *fp;
    int width, height;

    width = list_width(l) / 2;
    height = list_height(l);
    list_rewind(l);
    ansibufsize = FNWIDTH * FNHEIGHT * width;
    ansibuf = malloc (ansibufsize);

    void *ctx = driver->output_setup(height * FNHEIGHT, width * FNWIDTH,
            fi->color_table, fi->color_table_entries);

    if (!ctx) 
        goto out;

    for (y=0; y < height; y++) {
        line = list_forward(l);
        for (x=0; x<width; x++) {
            unsigned char attrib = line[x * 2 + 1];
            rasterize_character(line[x * 2], x, 0, ansibuf, width, 
                    attrib & 0x0f, attrib >> 4, fi);  
        }
        driver->output_block(ctx, ansibuf, FNHEIGHT, width * FNWIDTH);
    }

    driver->output_finish(ctx);
out:
    free (ansibuf);
}

void rasterize_character(unsigned char ch, int x, int y, 
                         char *framebuf, int width, 
                         char fgindex, char bgindex, 
                         struct font_info *fi) 
{
    int i, j;

    char *font_index;
    char font_bitmap;
    char bit;

    font_index = &fi->font[ (unsigned int) (ch * FNHEIGHT) ];
    // 8x16 font is stored as 16 bytes

    for (i=0; i<FNHEIGHT; i++) {
        font_bitmap = *(font_index + i);
        for (j=0; j<FNWIDTH; j++) {
            bit = 1 << (FNWIDTH-j-1);
            if ((font_bitmap & bit) != 0) {
                framebuf[ width * FNWIDTH * (FNHEIGHT * y + i) +
                    x * FNWIDTH + j ] = fgindex;  
            } else {
                framebuf[ width * FNWIDTH * (FNHEIGHT * y + i) +
                    x * FNWIDTH + j ] = bgindex;  
            }
        }
    }
}

