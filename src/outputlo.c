/*
 *  Reduces a BIN to an 80-wide pixmap by an almost clever color 
 *  reduction algorithm, known loosely as first-come-first-serve 
 *  (FCFS) along with a RGB color cube search for the really 
 *  psychadelic ansis with > 256 colors.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "dosfont.h" 
#include "output.h"

#ifndef MAX_DBL
#define MAX_DBL 1e30
#endif 

static void rasterize_thumb_character(unsigned char ch, int x, int y, 
                                      char *framebuf, int width, 
                                      char fgindex, char bgindex, 
                                      struct font_info *fi);

/* stores a .75/.25 and .5/.5 mapping table for fg & bg mixtures */
static unsigned char shade_table[ 16 * 16 * 2 ];

static int mix_color(unsigned int c1, unsigned int c2, double ptc1) 
{
    int i; 
    unsigned char rgb;
    float cm1, cm2;
    int retval = 0xff000000;

    for (i = 0; i < 3; i++) {
        cm1 = (double) ((c1 >> (i << 3)) & 0xff);
        cm2 = (double) ((c2 >> (i << 3)) & 0xff);
        rgb = (unsigned char) (cm1 * ptc1 + cm2 * (1.0-ptc1));
        retval |= (rgb & 0xff) << (i << 3);
    }
    return(retval);
}

/* 
 * Adds the color to the palette, returning a close match if no
 * more space in the table.
 */ 
static unsigned char add_color(unsigned int rgb, struct font_info *fi) 
{
    int i,j;
    double d_best = MAX_DBL, d, x; 
    unsigned char best_col = 16;
    double rgb_conv[3];

    if (fi->color_table_entries < 256) {
        fi->color_table[ fi->color_table_entries ] = rgb;
        fi->color_table_entries++; 
        return(fi->color_table_entries-1);
    } else {
        /* seperate rgb into components */
        for(i=0; i<3; i++) 
            rgb_conv[ i ] = (double) ((rgb >> (i << 3)) & 0xff);

        /* linear search using manhattan distance in RGB space */
        for (i=0; i<256; i++) {
            d = 0;
            for (j=0; j<3; j++) {
                x = rgb_conv[ j ] - (double) ((fi->color_table[i] >> (j << 3)) & 0xff);
                d += x * x;
            } 
            if (d < d_best) {
                d_best = d;
                best_col = i;
            }
        }
        return(best_col);
    } 
}

/*
 *  Returns a byte representing the palette entry for a color with
 *  the foreground color fg, background color bg, and shade 0-4 from
 *  most foreground to least (technically, four times the fraction of
 *  background color).  Reduces all duplicate entries, I hope.  
 */
static unsigned char get_color(int fg, int bg, int shade, struct font_info *fi) 
{
    unsigned char col;
    int rgb;

    if (fg == bg) {   /* No shade if both same */
        col = fg;
    } else {
        switch (shade) {
            case 0:          /* 1 fg, 0 bg */
                col = fg;
                break;
            case 1:          /* 3/4 fg, 1/4 bg */
                if((col = shade_table[ bg + fg * 16 ]) == 0) {
                    rgb = mix_color(fi->color_table[fg], 
                          fi->color_table[bg], .75);
                    col = add_color(rgb, fi); 
                    shade_table[ bg + fg * 16 ] = col;
                } 
                break;
            case 2:           /* 1/2 fg, 1/2 bg */
                if((col = shade_table[ 256 + bg + fg * 16 ]) == 0) {
                    rgb = mix_color(fi->color_table[fg], 
                          fi->color_table[bg], .5);
                    col = add_color(rgb, fi); 
                    shade_table[ 256 + bg + fg * 16 ] = col;
                    shade_table[ 256 + fg + bg * 16 ] = col;
                }
                break;
            case 3:           /* 1/4 fg, 3/4 bg */
                if((col = shade_table[ fg + bg * 16 ]) == 0) {
                    rgb = mix_color(fi->color_table[fg], 
                          fi->color_table[bg], .25);
                    col = add_color(rgb, fi); 
                    shade_table[ fg + bg * 16 ] = col;
                }
                break;
            case 4:           /* 0 fg, 1 bg */
                col = bg;
                break;
            default: 
                col = -1;
                break;
        }
    }
    return(col); 
}

static int is_top(unsigned char ch) 
{
    // false only if a-z, punctuation, and bottom blk (0xdc).
    if (ch == 0xdc || 
            ch == '_'  ||
            ch == '.'  ||
            ch == ','  ||
            ch == '-'  ||
            (ch >= 'a' && ch <= 'z')) {
        return 0;
    }
    return 1;
}

static int is_bottom(unsigned char ch) 
{
    // false only if top blk (0xdf) or quotes 
    if (ch == 0xdf || 
            ch == '\'' || 
            ch == '\"') {
        return 0;
    }
    return 1;
}

/*
 * Outputs a thumbnail to stdout.
 */
void output_thumb_image(output_info *driver, list_t *l, struct font_info *fi) 
{
    int x, y;
    int i;

    char *ansibuf; 
    char *line;
    unsigned int ansibufsize;
    int width, height;
    void *ctx;

    width = list_width(l) / 2;
    height = list_height(l) * 2;
    list_rewind(l);
    ansibufsize = width * height * 2;
    ansibuf = malloc (ansibufsize);

    memset(shade_table, 0, 16 * 16 * 2); 

    for (y=0; y<height; y++) {
        line = list_forward(l);
        for (x=0; x<width; x++) {
            unsigned char attrib = line[x * 2 + 1];
            rasterize_thumb_character(line[x * 2], x, y, ansibuf, width, 
                    attrib & 0x0f, attrib >> 4, fi);  
        }
    }

    ctx = driver->output_setup(height, width, fi->color_table, 
            fi->color_table_entries);

    if (!ctx) {
        free(ansibuf);
        return;
    }

    driver->output_block(ctx, ansibuf, height, width); 
    driver->output_finish(ctx);
    free (ansibuf);
}

/* 
 * Easy version of the other one that is used for the full  
 * conversion, this routine just determines what character 
 * we're dealing with, calls the appropriate routines to get
 * the color index, then boom! it's done.
 */
void rasterize_thumb_character(unsigned char ch, int x, int y, 
                              char *framebuf, int width, 
                              char fgindex, char bgindex,
                              struct font_info *fi) 
{

    unsigned char color = 0;
    switch (ch) {
        case 0xb0:         // 1/4 blk 
            color = get_color(fgindex, bgindex, 3, fi);
            break;
        case 0xb1:         // 1/2 blk
            color = get_color(fgindex, bgindex, 2, fi);
            break;
        case 0xb2:         // 3/4 blk
            color = get_color(fgindex, bgindex, 1, fi);
            break;
        case 0:
        case 255:
        case ' ':
            color = get_color(fgindex, bgindex, 4, fi);
            break;
        default:
            color = get_color(fgindex, bgindex, 0, fi);
            break;
    }

    if (is_top(ch)) 
        framebuf[ (y * 2) * width + x ] = color;
    else
        framebuf[ (y * 2) * width + x ] = bgindex;
    if (is_bottom(ch)) 
        framebuf[ (y * 2 + 1) * width + x ] = color;
    else
        framebuf[ (y * 2 + 1) * width + x ] = bgindex;
}

