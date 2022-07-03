#define __LIBRARY__
#include <unistd.h>
#include <stdlib.h>
#define MSG_MOUSE_LEFT_DOWN 1
#define MSG_MOUSE_RIGHT_DOWN 2
#define MSG_MOUSE_CENTER_DOWN 3
#define MSG_USER_TIMER 4
#define TYPE_USER_TIMER_INFTY 5
#define TYPE_USER_TIMER_ONCE 6
#define BIRD_X 120
#define BIRD_Y 100
#define BIRD_WIDTH 10
#define BIRD_HEIGHT 8
#define MAX_BARRIER 20
#define CLOCK_TRIGGER 400
#define DROP_PER_TRIGGER 1
#define UP_PER_CLICK 10
#define LEFT_PER_TRIGGER 1
#define BIRD_COLOR 12
#define BARRIER_COLOR 12
#define BACKGROUND_COLOR 3
#define GAME_OVER_COLOR 12
#define VAG_WIDTH 320
#define VGA_HEIGHT 200
#define BARRIER_WIDTH 10
#define BARRIER_INTERVAL 20
#define BARRIER_HEIGHT (rand()%(VGA_HEIGHT*3/4))
struct message {
    long mid;
    long pid;
} message;
struct rect {
    long color;
    long x;
    long y;
    long dx;
    long dy;
};
struct user_timer {
	long init_jiffies;
	long jiffies;
	int type;
	int pid;
	struct user_timer * next;
};
struct obj {
    int x;
    int y;
    int dx; /
    int dy;
};
struct obj bird;
int fron, rear;
struct obj barrier[MAX_BARRIER];
int i;
struct message msg;
struct rect rect;
struct obj obj
_syscall0(int,init_graphics)
_syscall1(void,get_message,struct message *,msg)
_syscall1(int,paintrect,struct rect *,rect)
_syscall2(int,timercreate,long,ms,int,type)
int init_bird(void) {
    bird.x = BIRD_X;
    bird.y = BIRD_Y;
    bird.dx = BIRD_WIDTH;
    bird.dy = BIRD_HEIGHT;
    return 0;
}
int init_barrier(void) {
    fron = rear = 0;
    return 0;
}

int init_all(void) {
    if (init_bird() < 0)
        return -1;
    if (init_barrier() < 0)
        return -1;
    return 0;
}
int push_obj(struct obj * obj) {
    if (rear != (fron + MAX_BARRIER - 1) % MAX_BARRIER) {
        barrier[rear].x = obj->x;
        barrier[rear].y = obj->y;
        barrier[rear].dx = obj->dx;
        barrier[rear].dy = obj->dy;
        rear = (rear + 1) % MAX_BARRIER;
        return 0;
    }
    return -1;
}
int pop_obj(struct obj * obj) {
    if (rear == fron)
        return -1;
    if (obj != NULL) {
        obj->x = barrier[fron].x;
        obj->y = barrier[fron].y;
        obj->dx = barrier[fron].dx;
        obj->dy = barrier[fron].dy;
    }
    fron = (fron + 1) % MAX_BARRIER;
    return 0;
}
int paint_background(int color) {
    struct rect rect;
    rect.color = color;
    rect.x = 0;
    rect.y = 0;
    rect.dx = VAG_WIDTH;
    rect.dy = VGA_HEIGHT;
    return paintrect(&rect);
}
int paint_bird(void) {
    struct rect rect;
    rect.color = BIRD_COLOR;
    rect.x = bird.x;
    rect.y = bird.y;
    rect.dx = bird.dx;
    rect.dy = bird.dy;
    return paintrect(&rect);
}
int paint_barrier(void) {
    int i;
    struct rect rect;
    for (i = fron; i != rear; i = (i+1)%MAX_BARRIER) {
        rect.color = BARRIER_COLOR;
        rect.x = barrier[i].x;
        rect.y = barrier[i].y;
        rect.dx = barrier[i].dx;
        rect.dy = barrier[i].dy;
        if (paintrect(&rect) < 0)
            return -1;
    }
    return 0;
}
int paint_all(void) {
    if (paint_background(BACKGROUND_COLOR) < 0)
        return -1;
    if (paint_bird() < 0)
        return -1;
    if (paint_barrier() < 0)
        return -1;
    return 0;
}
int main()
{
    timercreate(CLOCK_TRIGGER, TYPE_USER_TIMER_INFTY);
    initgraphics();
    if (init_all() < 0)
        return -1;
    while(1) {
        get_message(&msg);
        if (msg.mid < 0)
            continue;
        switch(msg.mid) {
        case MSG_MOUSE_LEFT_DOWN:
            bird.y -= UP_PER_CLICK;
            break;
        case MSG_USER_TIMER:
            if (paint_all() < 0)
                return -1;
            for (i = fron; i != rear; i = (i+1)%MAX_BARRIER)
                if (barrier[i].x < bird.x+bird.dx && bird.x < barrier[i].x+barrier[i].dx)
                    if (barrier[i].y < bird.y+bird.dy && bird.y < barrier[i].y+barrier[i].dy)
                        {  paint_background(GAME_OVER_COLOR);return 0;
                        }
            bird.y += DROP_PER_TRIGGER;
            for (i = fron; i != rear; i = (i+1)%MAX_BARRIER)
                barrier[i].x -= LEFT_PER_TRIGGER;
            if (fron == rear) {
                obj.dx = BARRIER_WIDTH;
                obj.dy = BARRIER_HEIGHT;
                obj.x = VAG_WIDTH;
                obj.y = 0;
                push_obj(&obj);
            }
            else { if (barrier[(rear+MAX_BARRIER-1)%MAX_BARRIER].x+barrier[(rear+MAX_BARRIER-1)%MAX_BARRIER].dx+BARRIER_INTERVAL <= VAG_WIDTH) {
                    obj.dx = BARRIER_WIDTH;
                    obj.dy = BARRIER_HEIGHT;
                    obj.x = VAG_WIDTH;
                    if (barrier[(rear+MAX_BARRIER-1)%MAX_BARRIER].y)
                        obj.y = 0;
                    else
                        obj.y = VGA_HEIGHT - obj.dy;
                    push_obj(&obj);
                }
                if (barrier[fron].x+barrier[fron].dx <= 0)
                    pop_obj(NULL);
            }
        }
    }
}
