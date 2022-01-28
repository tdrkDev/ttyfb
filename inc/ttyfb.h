#include <fcntl.h>
#include <linux/fb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#ifndef __TTYFB_H
#define __TTYFB_H

typedef struct ttyfb {
  struct fb_var_screeninfo vinfo;
  struct fb_fix_screeninfo finfo;
  char *fbp;
  size_t screensize;
  int fd;
} ttyfb_t;

typedef struct ttyfb_color {
  int8_t b;
  int8_t g;
  int8_t r;
} ttyfb_color_t;

typedef enum ttyfb_figure {
  FIG_RECT,
  FIG_ELLIPSE,
} ttyfb_figure_t;

int ttyfb_init(ttyfb_t *fb);
void ttyfb_close(ttyfb_t *fb);

void put_pixel(ttyfb_t *fb, ttyfb_color_t clr, int x, int y);
void put_square(ttyfb_t *fb, ttyfb_color_t clr, int x, int y, int size);
void put_rect(ttyfb_t *fb, ttyfb_color_t clr, int x, int y, int x2, int y2);
void put_ellipse(ttyfb_t *fb, ttyfb_color_t clr, int x, int y, int h, int w);
void put_img(ttyfb_t *fb, int ix, int iy, char *filename);

#endif