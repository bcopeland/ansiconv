/*
 *  Routine to grab Sauce from a file.  
 */
#include <stdio.h>
#include "sauce.h"

static void get_u32(uint32_t *out, FILE *fp)
{
    unsigned char c[4];

    fread(c, 1, 4, fp);
    *out = (c[3] << 24) | (c[2] << 16) | (c[1] << 8) | c[0];
}

static void get_u16(uint16_t *out, FILE *fp)
{
    unsigned char c[2];

    fread(c, 1, 2, fp);
    *out = (c[1] << 8) | c[0];
}

int get_sauce(FILE *fp, sauce_t *out) 
{
    long cur_pos;

    cur_pos = ftell(fp);  
    if (fseek(fp, -128, SEEK_END) == -1)
        return -1;

    /* 
     * can't count on Sauce structure to be char aligned, 
     * so this bit reads it piecemeal.  Hate to add all these 
     * numbers without #defines, but they're in sauce.h if it 
     * really is confusing.
     */ 
    fread(out->id, 1, 5, fp);
    fread(out->version, 1, 2, fp);
    fread(out->title, 1, 35, fp);
    fread(out->author, 1, 20, fp);
    fread(out->group, 1, 20, fp);
    fread(out->date, 1, 8, fp);

    get_u32(&out->file_size, fp);

    fread(&out->data_type, 1, 1, fp);
    fread(&out->file_type, 1, 1, fp);

    get_u16(&out->t_info1, fp);
    get_u16(&out->t_info2, fp);
    get_u16(&out->t_info3, fp);
    get_u16(&out->t_info4, fp);

    fread(&out->comments, 1, 1, fp);
    fread(&out->flags, 1, 1, fp);

    fseek(fp, cur_pos, SEEK_SET);

    if (strncmp(out->id, "SAUCE", 5) == 0) 
        return 0;

    return -1;
}
