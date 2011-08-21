/*
 *    Reads an IDF into a linked list, and updates any useful variables. 
 */
#include <stdio.h>
#include "list.h"
#include "dosfont.h"

/* some useful constants */
#define TERMIDF       199        // Number of lines (fixed?!)
#define PALSIZE    16 * 3        // Number of bytes for the palette
#define HDRSIZE        12        // Number of bytes for the header

struct idf_parse_state
{
    char *line;
    list_t *list;
    unsigned int width;
    unsigned int height;
};

static void store_idf(struct idf_parse_state *p, char ch, char attrib, int x) 
{
    p->line[x*2] = ch;
    p->line[x*2 + 1] = attrib;
    if (x == p->width - 1) {
        p->line = list_forward_cat(p->list);
    }
}

list_t *idf_read(FILE *fp, struct font_info *fi)
{
    int i, j;
    unsigned int runlength;
    unsigned char attrib, ch;
    struct idf_parse_state p;

    /* lazily read the apropos portions of the header */ 
    fseek(fp, 8, SEEK_SET);    // byte 9 is width-1
    p.width = fgetc(fp) + 1;
    fseek(fp, 1, SEEK_CUR);    // byte 11 is height-1
    p.height = fgetc(fp) + 1;
    fseek(fp, 1, SEEK_CUR);
 
    p.list = list_create(p.width*2, NULL);
    p.line = list_forward_cat(p.list);

    // read in the characters
    for(i=0; i < p.width * TERMIDF && !feof(fp);) {
        ch = fgetc(fp); 
        attrib = fgetc(fp); 

        // handle RLE 
        if (ch == 1 && attrib == 0) {
            /* 
             * This specifies a run of characters.    The following odd looking
             * code is to make this byte-order independent
             */
            ch = fgetc(fp);
            attrib = fgetc(fp);
            runlength = (ch | (attrib << 8));
            ch = fgetc(fp);
            attrib = fgetc(fp);

             /* 
              * Terminate if run is 0,0 and will end the pic.
              */
            if ((runlength + i >= p.width * TERMIDF) && attrib == 0) 
                break;

            for(j=0; j<runlength; j++, i++) 
                store_idf(&p, ch, attrib, i % p.width);

        } else {
            store_idf(&p, ch, attrib, i % p.width);
            i++;
        }
    }

    // read in the font
    if (!feof(fp)) {
        fread(fi->font, 1, FONTSIZE, fp); 
    }

    // get palette
    if (!feof(fp)) {
        float r, g, b;

        for(i=0; i<16; i++) {
            r = ((float) fgetc(fp)) / 63;
            g = ((float) fgetc(fp)) / 63;
            b = ((float) fgetc(fp)) / 63;
            
            fi->color_table[i] = ((unsigned char)(r * 255.0)) << 16 |
                                 ((unsigned char)(g * 255.0)) << 8  |
                                 ((unsigned char)(b * 255.0));
        }
    }
    return p.list;
}

