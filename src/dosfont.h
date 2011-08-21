#ifndef DOSFONT_H
#define DOSFONT_H
extern char dosfont[];
extern int default_color_table[];
extern int default_color_table_entries;

#define FNWIDTH 8
#define FNHEIGHT 16
#define FONTSIZE 4096

struct font_info
{
    int color_table[256];
    int color_table_entries;
    char font[FONTSIZE];
};

#endif
