#ifndef PECTIN_EXAMPLE_COMMON_H
#define PECTIN_EXAMPLE_COMMON_H

#include <time.h>
#include "render_util.h"

#define SCREEN_W 1024
#define SCREEN_H 640

#define LEFT    1
#define ABOVE   2
#define RIGHT   4
#define BELOW   8
#define FRONT   16
#define BACK    32

typedef struct timespec timespec;

int *level;
int level_w;
int level_h;
int level_layers;
int collision_layer;
int font_kern;
extern int font_back[];
extern int font_space[];

double time_diff_d(timespec start, timespec end);
int thing_at(int x, int y, int layer);
void sprite_for(float x, float y, int layer, int ridx, int cidx, sprite *s, render_def *rd);

#endif //PECTIN_EXAMPLE_COMMON_H
