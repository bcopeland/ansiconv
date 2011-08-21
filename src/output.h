#ifndef OUTPUT_H
#define OUTPUT_H

#include "list.h"
#include "dosfont.h"

typedef struct _output_info {
  void* (*output_setup)(int height, int width, int *palette, int num_entries);
  void (*output_block)(void *user, char *pixels, int height, int width);
  void (*output_finish)(void *user);
} output_info;

void output_image(output_info *driver, list_t *list, struct font_info *fi);
void output_thumb_image(output_info *driver, list_t *list, struct font_info *fi);

#endif /* OUTPUT_H */
