#ifndef OUTPUTPNG_H
#define OUTPUTPNG_H

void *png_output_setup(int height, int width, int *palette, int num_entries);
void png_output_block(void *user, char *pixels, int height, int width );
void png_output_finish(void *user);

#endif
