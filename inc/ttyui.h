#include <ttyfb.h>
#include <stdbool.h>
#include <stdlib.h>

#ifndef __TTYUI_H
#define __TTYUI_H

struct ttyui_mouse {
    int x;
    int y;
    bool left;
    bool middle;
    bool right;
};

int ttyui_init(struct ttyfb *fb);
void ttyui_close(void);

extern struct ttyui_mouse mouse;

#endif /* __TTYUI_H */
