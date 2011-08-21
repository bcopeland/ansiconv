#ifndef OUTPUT_GIF_H
#define OUTPUT_GIF_H

void *gif_output_setup(int height, int width, int *palette, int num_entries);
void gif_output_block(void *user, char *pixels, int height, int width);
void gif_output_finish(void *user);

#endif
