#include <ttyfb.h>
#include <lodepng.h>
#include <stdio.h>
#include <unistd.h>

void put_square(ttyfb_t *fb, ttyfb_color_t clr, int x, int y, int size) {
    for (;x < x + size; x++) {
        for (;y < y + size; y++) {
            if (x > fb->vinfo.xres || y > fb->vinfo.yres || x < 0 || y < 0)
                continue;
            
            put_pixel(fb, clr, x, y);
        }
    }
}

void put_rect(ttyfb_t *fb, ttyfb_color_t clr, int x, int y, int x2, int y2) {
    int tx, ty;

    for (ty = y; ty < (y + y2); ty++) {
        for (tx = x; tx < (x + x2); tx++) {
            if (tx > fb->vinfo.xres || ty > fb->vinfo.yres || tx < 0 || ty < 0)
                continue;

            put_pixel(fb, clr, tx, ty);
        }
    }
}

void put_ellipse(ttyfb_t *fb, ttyfb_color_t clr, int x, int y, int h, int w) {
    int tx, ty;

    for (int ty=-h; ty<=h; ty++) {
        for (int tx=-w; tx<=w; tx++) {
            double dx = (double)tx / (double)w;
            double dy = (double)ty / (double)h;
            if ((dx * dx + dy * dy <= 1)
                  && ((tx + x) < fb->vinfo.xres || (ty + y) < fb->vinfo.yres || (tx + x) > 0 || (ty + y) > 0))
                put_pixel(fb, clr, x + tx, y + ty);
        }
    }
}


void put_img(ttyfb_t *fb, int ix, int iy, char *filename) {
    int ret;
    unsigned char* image = 0;
    unsigned w, h;
    
    ret = lodepng_decode32_file(&image, &w, &h, filename);
    if (ret) {
        printf("Failed to decode image\n");
        return;
    }

    for (int y = iy; y < iy + h; y += 1)
        for (int x = ix; x < ix + w; x += 1) {
            if (x > fb->vinfo.xres || y > fb->vinfo.yres || x < 0 || y < 0)
                continue;

            unsigned int r, g, b;

            /*get RGB components*/
            r = image[4 * (y - iy) * w + 4 * (x - ix) + 0]; /*red*/
            g = image[4 * (y - iy) * w + 4 * (x - ix) + 1]; /*green*/
            b = image[4 * (y - iy) * w + 4 * (x - ix) + 2]; /*blue*/

            ttyfb_color_t clr = {r,g,b};
            put_pixel(fb, clr, x, y);
        }

    free(image);
}
