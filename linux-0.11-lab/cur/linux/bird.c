#define __LIBRARY__

#include<stdio.h>

#include<unistd.h>

#include<errno.h>

#include<time.h>

#include<stdlib.h>

#define __NR_init_graphics 87

#define __NR_get_message 88

#define __NR_timer_create 89

#define __NR_show 90

#define SCREEN_COLOR 3

#define MSG_MOUSE_CLICK 1

#define MSG_TIME 2

#define BIRD_X 30

#define BIRD_Y 100

#define BIRD_WIDTH 10

#define BIRD_HEIGHT 12

#define BIRD_COLOR 6

#define BIRD_UP_SPEED 20

#define BIRD_FALL_SPEED 5

#define B_NUM 4

#define B_WIDTH 20

#define B_HEIGHT 300

#define B_COLOR 4

#define B_x1 50

#define B_x2 150 

#define B_x3 0 

#define B_x4 150 

#define B_SPEED 2

typedef struct messages {

    unsigned char mid;

    int pid;

    struct messages* next;

}message;

typedef struct timer {

    long jiffies;

    int type;

    long init_jiffies;

    int pid;

    struct usertimer* next;

}timers;
typedef struct barriers {

    unsigned char posx;
    unsigned char posy;
    unsigned char width;
    unsigned char height;
    unsigned char color;

}bar;



_syscall1(int, init_graphics, int, color)

_syscall1(int, get_message, unsigned char*, msg)

_syscall2(int, timer_create, long, milliseconds, int, type)

_syscall1(int, show, unsigned char*, ob)

int Rand(int x, int y) {

    int t;

    t = time(NULL);

    return x + (((t << 5) % y) * 314) % y;

}

void showit(bar m) {

    unsigned char* t = (unsigned char*)malloc(sizeof(bar));

    *t = m.posx;

    *(t + 1) = m.posy;

    *(t + 2) = m.width;

    *(t + 3) = m.height;

    *(t + 4) = m.color;

    show(c);

    free(c);

}

int main() {

    bar bars[B_NUM];

    unsigned char* c;

    int i;

    bars[0].posx = BIRD_X;

    bars[0].posy = BIRD_Y;

    bars[0].width = BIRD_WIDTH;

    bars[0].height = BIRD_HEIGHT;

    bars[0].color = BIRD_COLOR;



    for (i = 1; i < B_NUM; i++) {

        bars[i].height = B_HEIGHT + (i % 2) * 3000;

        bars[i].width = B_WIDTH;

        bars[i].posx = B_x1 + 300 * i;

        bars[i].posy = (i % 2) ? ((bars[i].posx * i) % B_x2) : B_x3;

        bars[i].color = B_COLOR;

    }

    init_graphics(SCREEN_COLOR);

    for (i = 0; i < B_NUM; i++) {

        showit(bars[i]);

    }

    timer_create(100, 0);

    while (1) {

        unsigned char* msg = (unsigned char*)malloc(sizeof(unsigned char));

        get_message(msg);

        if (*msg == MSG_MOUSE_CLICK) {

            bars[0].color = SCREEN_COLOR;

            showit(bars[0]);

            bars[0].posy -= BIRD_UP_SPEED;

            bars[0].color = BIRD_COLOR;

            showit(bars[0]);

        }

        else if (*msg == MSG_TIME) {

            bars[0].color = SCREEN_COLOR;

            showit(bars[0]);

            bars[0].posy += BIRD_FALL_SPEED;

            bars[0].color = BIRD_COLOR;

            showit(bars[0]);

            for (i = 1; i < B_NUM; i++) {

                bars[i].color = SCREEN_COLOR;

                showit(bars[i]);

                bars[i].posx -= B_SPEED;

                bars[i].color = B_COLOR;

                showit(bars[i]);

                if ((bars[i].posx + bars[i].width) < 0) {

                    bars[i].posx = 150;

                    bars[i].posy = Rand(B_x3, B_x4);

                }

            }

        }

        free(msg);

    }

    return 0;

}





