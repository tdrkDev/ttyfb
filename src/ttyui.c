#include <ttyui.h>
#include <ttyfb.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>

/* More high-level library than ttyfb-img. */
struct ttyui_mouse mouse;
static int input_fd;
static int fb_res[2];

void *ttyui_mouse_thread(void *x) {
    int ret;
    unsigned char data[3];
    int tx, ty;

    while (true) {
        ret = read(input_fd, data, 3);
        if (ret) {
            mouse.left = data[0] & 0x1;
            mouse.right = data[0] & 0x2;
            mouse.middle = data[0] & 0x4;

            tx = mouse.x + (char)data[1];
            ty = mouse.y - (char)data[2];

            if (tx > 0 && tx <= fb_res[0])
                mouse.x = tx;

            if (ty > 0 && ty <= fb_res[1])
                mouse.y = ty;
        }
    }
}

int ttyui_init(struct ttyfb *fb) {
    pthread_t thread;

    mouse.x = 0;
    mouse.y = 0;

    input_fd = open("/dev/input/mice", O_RDWR);
    if (input_fd < 0) {
        printf("Failed to open /dev/input/mice\n");
        return input_fd;
    }

    fb_res[0] = fb->vinfo.xres;
    fb_res[1] = fb->vinfo.yres;

    pthread_create(&thread, NULL, ttyui_mouse_thread, NULL);
}

void ttyui_close(void) {
    close(input_fd);
}
