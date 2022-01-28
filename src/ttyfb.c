#include <fcntl.h>
#include <linux/fb.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <memory.h>
#include <unistd.h>
#include <errno.h>
#include <ttyfb.h>
#include <curses.h>

void put_pixel(struct ttyfb *fb, struct ttyfb_color clr, int x, int y) {
  long int location = (x + fb->vinfo.xoffset) * 4 +
             (y + fb->vinfo.yoffset) * fb->finfo.line_length;

  *(fb->fbp + location) = clr.b;
  *(fb->fbp + location + 1) = clr.g;
  *(fb->fbp + location + 2) = clr.r;
  *(fb->fbp + location + 3) = 0;
}

int ttyfb_init(struct ttyfb *fb) {
  fb->fd = open("/dev/fb0", O_RDWR);
  if (fb->fd == -1) {
    perror("Error opening /dev/fb0");
    return -1;
  }

  // Get fixed screen information
  if (ioctl(fb->fd, FBIOGET_FSCREENINFO, &fb->finfo)) {
    printf("Error reading fixed information.\n");
    return -EINVAL;
  }

  // Get variable screen information
  if (ioctl(fb->fd, FBIOGET_VSCREENINFO, &fb->vinfo)) {
    printf("Error reading variable information.\n");
    return -EINVAL;
  }

  printf("%dx%d, %dbpp\n", fb->vinfo.xres, fb->vinfo.yres, fb->vinfo.bits_per_pixel);

  if (fb->vinfo.bits_per_pixel < 32) {
    printf("Your display isn't supported.\n");
    return -EINVAL;
  }

  // Figure out the size of the screen in bytes
  fb->screensize = fb->vinfo.xres * fb->vinfo.yres * fb->vinfo.bits_per_pixel / 8;

  // Map the device to memory
  fb->fbp =
      (char *)mmap(0, fb->screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fb->fd, 0);
  if ((size_t)fb->fbp == -1) {
    printf("Failed to map framebuffer device to memory.\n");
    return -ENOMEM;
  }

  // Hide cursor
  fputs("\e[?25h", stdout);

  return 0;
}

void ttyfb_close(struct ttyfb *fb) {
  munmap(fb->fbp, fb->screensize);
  close(fb->fd);

  // Show cursor
  fputs("\e[?25l", stdout);
}
