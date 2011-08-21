/*
 *  Reads a BIN into a linked list.  This is simple.
 */
#include <stdio.h>
#include "list.h"
#include "sauce.h"
#include "dosfont.h"

#define BINWIDTH 160
#define CTRLZ 0x1a

list_t *bin_read(FILE *fp, struct font_info *fi)
{
    static char *line;
    unsigned int width;
    unsigned int sizeread;
    char ch;
    int i;
    sauce_t sauce;
    int result;
    list_t *list;

    // try and get width from sauce
    result = get_sauce(fp, &sauce);
    if (result == 0 && sauce.file_type > 0)
        width = sauce.file_type * 2;
    else
        width = BINWIDTH;

    sizeread = width * 2;
    list = list_create(width*2, NULL);

    // read in the characters
    while (!feof(fp) && sizeread == width*2) {
        if ((ch = fgetc(fp)) == CTRLZ) 
            break;
        ungetc(ch, fp);
        line = list_forward_cat(list);
        sizeread = fread(line, 1, width*2, fp);
    }
    // that was easy!
    fclose(fp);
    return list;
}
