#ifndef OUTPUT_H
#define OUTPUT_H

typedef struct _output_info {
  int (*output_setup)(int height, int width, int *palette);
  int (*output_block)(char *pixels, int height, int width);
  int (*output_finish)();
} output_info;

void outputImage( output_info *driver );
void outputThumbImage( output_info *driver );

#endif /* OUTPUT_H */
