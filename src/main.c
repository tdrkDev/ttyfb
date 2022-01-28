#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <ttyfb.h>
#include <ttyui.h>
#include <unistd.h>
#include <pthread.h>

static unsigned char *minidraw_buf;
static int minidraw_scr_size[2];
static struct ttyfb_color white = {255, 255, 255};
static struct ttyfb_color black = {0, 0, 0};

static inline void minidraw_put(int x, int y, bool state) {
    int pos = y * minidraw_scr_size[0] + x;

    minidraw_buf[pos] = state;
}

static inline void minidraw_flush(void) {
    for (int i = 0; i < minidraw_scr_size[1] * minidraw_scr_size[0]; i++)
        minidraw_buf[i] = false;
}

static inline void minidraw_draw_scr(struct ttyfb *fb) {
    for (int y = 0; y < minidraw_scr_size[1]; y++) {
        for (int x = 0; x < minidraw_scr_size[0]; x++) {
            int pos = y * minidraw_scr_size[0] + x;
            put_pixel(fb, minidraw_buf[pos] ? white : black, x, y);
        }
    }
}

static inline void minidraw_alloc(struct ttyfb *fb) {
    minidraw_scr_size[0] = fb->vinfo.xres;
    minidraw_scr_size[1] = fb->vinfo.yres;
    minidraw_buf = malloc(minidraw_scr_size[0] * minidraw_scr_size[1] * sizeof(unsigned char));
}

static void *minidraw_mouse_thread(void *a) {
    while (true) {
        // Perform MiniDraw FB operations
        if (mouse.left)
            minidraw_put(mouse.x, mouse.y, true);
        else if (mouse.right)
            for (int x = mouse.x - 15; x <= mouse.x + 15; x++) {
                for (int y = mouse.y - 15; y <= mouse.y + 15; y++) {
                    if (x > 0 && y > 0 && x < minidraw_scr_size[0] && y < minidraw_scr_size[1])
                      minidraw_put(x, y, false);
                }
            }
        else if (mouse.middle)
            minidraw_flush();
    }
}

int main(int argc, char *argv[]) {
    int ret = 0;
    struct ttyfb fb;
    struct ttyfb_color darkgrey = {20, 20, 20};
    struct ttyfb_color blue = {0, 90, 255};
    struct ttyfb_color red = {255, 0, 20};
    struct ttyfb_color green = {0, 255, 20};
    pthread_t thread;
    bool state = false;
    struct timespec spec, spec_prev;

    printf("ttyFB test\n");
    printf("%d\n", sizeof(struct ttyfb));

    ret = ttyfb_init(&fb);
    if (ret) {
        perror("Failed to init ttyFB\n");
        return ret;
    }

    ret = ttyui_init(&fb);
    if (ret) {
        perror("Failed to init ttyUI\n");
        goto exit;
    }

    minidraw_alloc(&fb);

    // Move drawing stuff to another thread
    pthread_create(&thread, NULL, minidraw_mouse_thread, NULL);

    while (true) {
        clock_gettime(CLOCK_REALTIME, &spec_prev);

        // Draw MiniDraw's FB to main FB
        minidraw_draw_scr(&fb);

        // Draw mouse cursor
        if (mouse.right) {
            put_rect(&fb, white, mouse.x - 15, mouse.y - 15, 30, 30);
        } else {
            put_rect(&fb, white, mouse.x - 1, mouse.y - 1, 3, 3);
        }

        clock_gettime(CLOCK_REALTIME, &spec);
        
        // Get ~60FPS
        int sleep_time = 16000 - ((spec.tv_nsec - spec_prev.tv_nsec) / 1000);
        if (sleep_time > 0 && spec.tv_nsec > spec_prev.tv_nsec)
            usleep(sleep_time);
    }

    ttyui_close();
exit:
    ttyfb_close(&fb);
    return 0;
}