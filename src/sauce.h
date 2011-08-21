#ifndef SAUCE_H
#define SAUCE_H

#include <stdio.h>
#include <inttypes.h>

typedef struct _sauce {
    char id[5];
    char version[2];
    char title[35];
    char author[20];
    char group[20];
    char date[8];
    uint32_t file_size;
    unsigned char data_type;
    unsigned char file_type;
    uint16_t t_info1;
    uint16_t t_info2;
    uint16_t t_info3;
    uint16_t t_info4;
    unsigned char comments;
    unsigned char flags;
    char filler[22];
} sauce_t;

int get_sauce(FILE *fp, sauce_t *out);

#endif
