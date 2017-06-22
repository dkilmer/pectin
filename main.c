#include <time.h>
#include <stdbool.h>
#include <glad/glad.h>
#include "window.h"
#include "render_util.h"
#include "game_util.h"
#include "misc_util.h"
#include "physics.h"
#include "easing.h"
#include "colors.h"
#include "boids.h"
#include <chipmunk/chipmunk.h>

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

int font_space[] = {
	6,2,4,6,6,7,7,2,4,4,6,6,3,6,2,6,6,4,6,6,6,6,6,6,
	6,6,2,3,6,6,6,6,7,6,6,6,6,6,6,6,6,6,6,6,6,8,7,6,
	6,6,6,6,6,6,8,8,6,6,6,4,6,4,6,7,5,6,6,6,6,6,6,6,
	6,2,5,6,2,8,6,6,6,6,6,6,6,6,6,8,6,6,6,4,2,4,6,8
};

int font_back[] = {
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,-3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

int font_kern = 1;

int level_trifle[] = {
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,3,1,1,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,3,3,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,2,4,4,2,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,3,0,0,0,0,1,3,0,0,0,0,0,0,0,52,51,57,49,51,49,57,49,0,0,0,0,0,0,0,0,0,2,3,0,0,0,0,0,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,1,1,0,0,0,0,0,0,3,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,2,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,0,0,0,0,0,0,1,4,3,4,3,2,2,4,2,2,3,3,4,2,2,3,4,1,1,1,2,2,4,4,1,4,1,4,1,3,4,2,3,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,4,2,4,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,3,2,3,3,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,1,2,2,2,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,3,3,4,4,2,1,1,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,3,3,3,2,2,1,3,1,1,0,0,0,0,0,56,55,53,53,51,54,57,57,0,0,0,0,0,0,0,0,0,2,1,4,0,0,0,0,0,4,4,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,0,0,0,0,0,0,3,4,3,0,0,0,0,0,0,3,4,1,0,0,0,0,0,0,0,0,0,0,0,0,0,4,0,0,0,0,0,0,1,4,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,4,3,1,1,2,2,1,3,4,1,2,1,3,3,4,4,4,2,1,1,2,4,3,1,4,3,3,4,3,1,2,3,1,0,0,3,2,2,4,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,3,4,1,2,0,0,0,0,0,3,3,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,1,2,0,0,0,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,1,4,3,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,3,4,2,2,2,3,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,4,3,1,2,2,2,3,1,0,0,0,0,0,0,0,0,0,0,51,0,0,0,0,0,0,0,0,0,0,0,0,2,1,2,1,2,1,1,3,1,1,0,0,0,0,0,0,0,0,50,51,49,0,0,0,0,0,0,0,0,0,0,0,2,1,1,4,3,1,1,2,2,3,2,0,0,0,0,0,0,51,56,55,57,51,0,0,0,0,0,0,0,0,0,0,4,2,4,4,3,3,3,4,3,1,1,4,0,0,0,58,55,51,54,58,57,56,55,0,0,0,0,0,0,0,0,0,1,4,2,1,2,4,4,1,2,3,1,4,1,0,0,0,0,0,0,0,0,0,0,0,0,2,3,2,1,3,3,1,4,4,1,2,4,2,2,4,3,3,3,4,3,4,0,0,0,0,0,0,0,0,0,0,0,4,4,1,1,2,1,1,1,3,3,4,4,3,1,3,2,3,4,4,2,1,3,0,0,0,0,0,0,0,0,0,0,1,2,3,4,4,3,2,4,3,1,4,4,1,3,3,1,3,1,4,1,2,1,1,2,4,3,2,1,3,4,2,1,2,1,4,1,1,4,4,3,1,2,3,3,2,4,1,4,3,0,0,2,3,1,2,1,2,1,3,3,3,1,0,3,1,2,3,3,1,4,2,3,0,3,3,1,2,4,4,1,0,0,0,2,2,2,1,3,4,2,1,1,4,2,0,0,1,4,4,2,2,3,1,0,0,0,3,3,1,1,4,0,0,0,0,0,4,2,1,3,4,3,4,3,4,2,0,0,0,1,2,2,1,3,0,0,0,0,0,4,3,1,0,0,0,0,0,0,4,2,1,3,1,2,4,1,3,0,0,0,0,0,4,3,3,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,0,0,0,0,
	19,17,18,17,19,17,17,17,18,17,18,20,18,18,18,20,18,17,17,20,18,20,18,19,17,19,19,20,20,19,19,18,19,20,17,19,17,19,19,20,19,18,18,17,19,17,19,17,18,20,19,20,18,17,18,19,17,19,18,19,18,20,17,18,20,17,17,18,18,18,17,20,20,18,18,18,19,19,17,19,17,20,18,17,19,18,19,19,19,17,19,19,18,17,19,19,20,17,20,18,20,17,20,17,18,18,17,20,20,20,17,20,17,18,20,19,19,20,18,20,20,19,20,18,18,18,19,17,18,17,18,18,18,17,19,19,17,19,19,17,18,19,18,19,20,19,19,19,17,20,19,18,18,20,19,17,19,19,17,18,20,18,19,19,20,19,17,20,19,17,18,20,19,20,17,20,19,18,20,19,18,17,20,17,17,19,20,20,17,20,17,19,18,17,20,18,20,17,19,19,20,18,17,20,17,17,17,19,17,19,18,18,18,17,20,17,19,17,20,17,20,19,19,17,18,19,20,20,18,20,18,17,17,18,20,20,17,17,19,20,19,19,17,18,20,20,18,18,18,20,18,18,17,19,18,18,17,18,18,18,19,17,18,17,19,17,17,18,17,18,17,18,17,19,18,18,17,17,17,17,17,18,17,20,18,18,17,18,20,18,17,19,17,20,19,17,17,18,20,20,17,18,18,18,18,17,20,19,19,20,19,17,19,19,18,20,18,20,18,20,20,20,17,19,20,19,19,19,20,18,17,18,20,18,20,20,18,17,17,18,19,19,17,18,20,19,17,19,19,20,20,18,20,18,18,17,18,20,19,19,18,20,17,20,20,20,18,20,17,20,19,20,17,20,19,18,18,20,19,20,18,18,17,18,17,17,17,20,17,17,17,20,17,20,20,17,18,18,18,18,17,18,18,17,20,17,18,18,17,20,17,17,18,20,17,20,18,18,18,20,18,20,20,20,20,18,19,20,20,20,17,19,17,18,17,18,17,17,17,17,20,17,19,18,19,20,17,17,19,18,19,19,19,20,19,17,19,17,20,19,18,18,19,19,20,17,18,18,19,17,17,20,18,18,17,20,17,18,18,18,18,19,17,17,20,17,19,19,19,17,18,19,17,19,18,18,20,17,20,20,19,20,19,19,18,18,18,19,18,17,17,20,18,20,17,18,19,17,19,17,20,20,17,18,18,18,20,20,20,17,18,18,20,20,19,17,17,19,17,19,19,19,20,19,19,19,17,19,20,17,20,18,20,19,17,18,19,19,17,19,17,18,18,20,18,20,20,20,18,17,19,19,17,19,17,18,17,17,17,19,19,19,19,17,17,18,19,19,18,17,20,17,17,17,20,19,19,18,18,19,17,19,17,19,19,18,20,19,18,17,20,18,19,18,20,19,19,19,20,17,19,20,18,19,20,20,20,18,20,17,17,20,20,17,17,19,19,18,19,20,
	17,18,19,18,17,19,19,19,17,19,19,19,18,19,20,17,18,19,19,19,17,19,18,17,17,19,19,20,20,20,19,20,19,19,19,18,20,19,19,17,17,19,18,17,17,17,18,19,19,19,20,20,18,19,20,18,17,19,17,20,18,19,18,18,20,17,19,17,20,19,20,19,17,20,19,19,18,19,18,19,19,18,18,18,19,18,19,18,19,18,18,18,19,18,17,19,17,20,19,18,20,19,18,19,18,17,20,17,20,20,18,18,18,18,20,18,18,18,20,17,17,19,20,19,19,20,17,20,20,18,20,17,19,18,18,19,18,17,17,18,18,20,20,18,20,17,17,17,17,18,18,17,17,17,18,20,18,19,19,18,19,18,18,19,20,18,18,17,19,20,19,17,19,20,20,18,19,20,20,19,19,20,18,17,20,17,17,19,20,19,20,17,20,20,19,20,17,19,20,17,20,18,18,19,17,18,20,19,20,17,19,17,18,20,20,18,19,19,20,17,18,18,18,19,20,20,18,20,18,17,17,19,20,20,20,18,17,20,19,18,20,19,20,19,19,18,18,19,18,17,19,20,17,18,19,17,20,17,19,20,17,17,20,18,17,20,17,17,18,19,17,17,19,19,19,19,20,19,17,19,17,17,19,19,18,20,17,18,18,18,19,18,18,18,18,20,18,20,18,20,19,17,18,19,18,17,20,17,18,18,18,20,18,19,18,18,18,19,20,19,18,20,18,19,18,17,17,19,20,20,20,20,20,18,17,17,17,17,20,18,18,20,20,17,18,18,19,18,20,18,19,19,17,18,18,20,19,18,20,18,17,20,17,19,17,20,19,20,18,20,19,18,20,19,20,18,17,20,20,18,17,20,20,19,19,18,18,18,20,19,20,19,17,20,18,17,20,20,17,17,17,18,19,18,19,18,18,18,19,18,20,17,19,20,19,18,17,19,20,18,20,19,17,17,17,19,19,18,17,18,20,20,20,18,19,20,19,17,18,19,17,17,17,20,20,17,18,17,17,20,19,17,19,19,17,19,18,18,18,19,20,17,19,20,17,18,18,17,19,17,20,18,19,18,18,17,17,20,18,17,20,17,19,19,20,17,20,20,19,20,20,19,19,20,19,17,17,20,19,19,18,17,18,20,19,18,20,19,18,18,17,18,17,17,20,20,17,18,17,17,19,20,17,18,18,17,17,17,19,20,18,20,18,17,18,20,17,20,19,17,19,20,17,18,18,18,20,18,17,17,19,18,19,17,20,19,20,17,19,20,20,19,19,19,18,19,19,18,17,19,18,17,18,17,20,20,18,18,20,17,17,17,18,18,18,18,17,18,20,19,20,20,19,19,18,20,20,18,17,20,18,20,18,20,20,17,17,18,19,18,19,17,18,20,18,19,17,20,18,19,19,18,19,20,20,19,19,19,18,17,17,17,18,19,17,19,18,20,19,18
};

int level_throw[] = {
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
};

int static_cnt = 7;
srect statics[] = {
	{1.0,13.0,6.0,14.0},
	{1.0,9.0,7.0,10.0},
	{1.0,4.0,11.0,5.0},
	//{14.0,4.0,20.0,5.0},
	{0.0,0.0,1.0,20.0},
	{31.0,0.0,32.0,20.0},
	{1.0,19.0,31.0,20.0},
	{1.0,0.0,31.0,1.0}
};

//int moss[] = {
//	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,449,337,337,401,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,449,369,401,0,353,401,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,449,305,449,369,401,449,369,273,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,449,434,0,353,465,433,353,401,449,401,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,321,497,369,337,401,0,417,0,481,369,497,273,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,353,401,449,305,449,369,401,353,465,305,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,353,369,449,305,385,481,337,305,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,353,465,497,305,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,417,417,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,481,433,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,417,417,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,481,433,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,417,417,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
//};

int level_w = 32;
int level_h = 20;
int num_boids = 50;
int level_layers = 5;
int collision_layer = 1;

void aabb_for(float x, float y, aabb *b) {
	b->x = x;
	b->y = y;
	b->rx = 0.5f;
	b->ry = 0.5f;
}

void sprite_for(float x, float y, int layer, int ridx, int cidx, sprite *s, render_def *rd) {
	s->x = (x + 0.5f + rd->xoff) + (rd->layer_xoff * (float)layer);
	s->y = (y + 0.5f + rd->yoff) + (rd->layer_yoff * (float)layer);
	s->z = ((((level_h-y)*level_w)+x) * -0.0001f) + ((float)layer*-1.0f);
	s->r = 0.556863f;
	s->g = 0.443137f;
	s->b = 0.329412f;
	s->scale_x = rd->xscale * 0.5f;
	s->scale_y = rd->yscale * 0.5f;
	s->rot = 0.0f;
	s->spr_row = (ridx * rd->cols) + cidx;
	s->spr_col = 0.0f;
	s->spr_extra = rd->cols;
}

void sprite_for_pobj(pobj *p, sprite *s, int layer, render_def *rd) {
	s->x = p->x;
	s->y = p->y;
	s->z = ((((level_h-p->y)*level_w)+p->x) * -0.0001f) + ((float)layer*-1.0f);
	//s->z -= ((0.1f * oscillate(p->scale_norm_x, 0.0f, 1.0f)) - 0.05f);
	//s->z += (0.004f * oscillate(p->z_norm, 0.0f, 1.0f));
	s->r = p->r;
	s->g = p->g;
	s->b = p->b;
	s->scale_x = rd->xscale * oscillate(p->scale_norm_x, 0.8f, 1.0f);
	s->scale_y = rd->yscale * oscillate(p->scale_norm_y, 0.8f, 1.0f);
	//s->scale_x = (CubicEaseInOut(p->scale_norm_x) * 0.3f) + 0.08f;
	//s->scale_y = (CubicEaseInOut(p->scale_norm_y) * 0.3f) + 0.08f;
	s->rot = p->rot_norm;
	s->spr_row = 126.0f;
	s->spr_col = 0.0f;
	s->spr_extra = rd->cols;
}

void sprite_for_boid(boid *b, sprite *s, int layer, render_def *rd) {
	s->x = b->x;
	s->y = b->y;
	s->z = ((((level_h-b->y)*level_w)+b->x) * -0.00001f) + ((float)layer*-1.0f);
	s->r = 0.0f;
	s->g = 0.0f;
	s->b = 0.0f;
	//s->scale_x = rd->xscale * fabsf(b->vx) * 4.0f;
	//s->scale_y = rd->yscale * fabsf(b->vy) * 4.0f;
	s->scale_x = rd->xscale + ((float)b->frame * 0.02f);
	s->scale_y = rd->yscale + ((float)b->frame * 0.02f);
	s->rot = b->ang;
	s->spr_row = 147 + b->frame;
	s->spr_col = 0.0f;
	s->spr_extra = rd->cols;
}

void randomize_particle(pobj *p) {
	p->x = p->x + ((rand_float() * 0.02f) - 0.01f);
	p->y = p->y + ((rand_float() * 0.02f) - 0.01f);
	p->z_norm = rand_float();
	p->scale_norm_x = rand_float();
	p->scale_norm_y = rand_float();
	unsigned char rr = 62;
	unsigned char gg = 78;
	unsigned char bb = 164;
	rgb rclr = {(float)rr / 255.0f, (float)gg / 255.0f, (float)bb / 255.0f};
	hsv hclr = rgb2hsv(rclr);
	hclr.v += ((rand_float() * 0.1f) - 0.05f);
	if (hclr.v > 1.0f) hclr.v = 1.0f;
	if (hclr.v < 0.0f) hclr.v = 0.0f;
	rgb aclr = hsv2rgb(hclr);
	p->r = aclr.r;
	p->g = aclr.g;
	p->b = aclr.b;
}

void update_particle(pobj *p) {
	p->z_norm = p->z_norm + 0.01f;
	if (p->z_norm > 1.0f) p->z_norm = 0.0f;
	p->scale_norm_x = p->scale_norm_x + 0.02f;
	if (p->scale_norm_x > 1.0f) p->scale_norm_x = 0.0f;
	p->scale_norm_y = p->scale_norm_y + 0.02f;
	if (p->scale_norm_y > 1.0f) p->scale_norm_y = 0.0f;
}

timespec time_diff(timespec start, timespec end)
{
	timespec temp;
	if ((end.tv_nsec-start.tv_nsec)<0) {
		temp.tv_sec = end.tv_sec-start.tv_sec-1;
		temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
	} else {
		temp.tv_sec = end.tv_sec-start.tv_sec;
		temp.tv_nsec = end.tv_nsec-start.tv_nsec;
	}
	return temp;
}

double time_diff_d(timespec start, timespec end)
{
	timespec temp;
	if ((end.tv_nsec-start.tv_nsec)<0) {
		temp.tv_sec = end.tv_sec-start.tv_sec-1;
		temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
	} else {
		temp.tv_sec = end.tv_sec-start.tv_sec;
		temp.tv_nsec = end.tv_nsec-start.tv_nsec;
	}
	return (double)temp.tv_sec + ((double)temp.tv_nsec / 1000000000.0);
}

int thing_at(int x, int y, int layer) {
	int off = (layer * level_w * level_h)+(((level_h-1)-y)*level_w)+x;
	if (off < 0 || off >= (level_w * level_h * level_layers)) return 0;
	return level[off];
}

bool block_at(int x, int y, int layer) {
	int off = (layer * level_w * level_h)+(((level_h-1)-y)*level_w)+x;
	if (off < 0 || off >= (level_w * level_h * level_layers)) return true;
	return (level[off] != 0);
}

int blat(int x, int y, int layer) {
	if (x < 0 || x >= level_w) return 0;
	if (y < 0 || y >= level_h) return 0;
	if (layer < 0 || layer >= level_layers) return 0;
	int off = (layer * level_w * level_h)+(((level_h-1)-y)*level_w)+x;
	if (off < 0 || off >= (level_w * level_h * level_layers)) return 0;
	return (level[off] != 0) ? 1 : 0;
}

int line_idx_for(int x, int y, int l) {
	return (blat(x-1, y, l) * LEFT) +
		(blat(x, y+1, l) * ABOVE) +
		(blat(x+1, y, l) * RIGHT) +
		(blat(x, y-1, l) * BELOW) +
		(blat(x, y, l-1) * FRONT) +
		(blat(x, y, l+1) * BACK) +
		192;
}

bool collides_with(aabb *b, int x, int y, int layer) {
	if (!block_at(x, y, layer)) return false;
	aabb sb;
	aabb_for(x, y, &sb);
	return collides(b, &sb);
}

int handle_horz_collision(dobj *k, float *fixx) {
	int coll = 0;
	// these are used to determine what blocks to check in the grid
	int kx = (int)floorf(k->x+0.5f);
	int ky = (int)floorf(k->y+0.5f);
	aabb kb = {k->x, k->y, k->rx, k->ry};
	bool u = block_at(kx, ky+1, collision_layer);
	bool d = block_at(kx, ky-1, collision_layer);

	bool ul = collides_with(&kb, kx-1, ky+1, collision_layer);
	bool ur = collides_with(&kb, kx+1, ky+1, collision_layer);
	bool dl = collides_with(&kb, kx-1, ky-1, collision_layer);
	bool dr = collides_with(&kb, kx+1, ky-1, collision_layer);
	bool l = collides_with(&kb, kx-1, ky, collision_layer);
	bool r = collides_with(&kb, kx+1, ky, collision_layer);

	if (l || (!u && ul) || (!d && dl)) {
		coll = (COLLIDE_LEFT);
		*fixx = (float)kx;//(kx - 1)+(0.5f + k->rx);
		//printf("COLLIDE_LEFT\n");
	}
	if (r || (!u && ur) || (!d && dr)) {
		coll = (COLLIDE_RIGHT);
		*fixx = (float)kx;//(kx + 1)-(0.5f + k->rx);
		//printf("COLLIDE_RIGHT\n");
	}
	//if (coll == 0) printf("COLLIDE_NONE\n");
	return coll;
}

int handle_vert_collision(dobj *k, float *fixy) {
	int coll = 0;
	// these are used to determine what blocks to check in the grid
	int kx = (int)floorf(k->x+0.5f);
	int ky = (int)floorf(k->y+0.5f);
	aabb kb = {k->x, k->y, k->rx, k->ry};
	bool ul = collides_with(&kb, kx-1, ky+1, collision_layer);
	bool u = collides_with(&kb, kx, ky+1, collision_layer);
	bool ur = collides_with(&kb, kx+1, ky+1, collision_layer);
	bool dl = collides_with(&kb, kx-1, ky-1, collision_layer);
	bool d = collides_with(&kb, kx, ky-1, collision_layer);
	bool dr = collides_with(&kb, kx+1, ky-1, collision_layer);

	if (k->vy > 0.0f && (u || ul || ur)) {
		coll = (coll | COLLIDE_ABOVE);
		*fixy = (float)(ky + 1)-(0.5f + k->ry);
		//printf("COLLIDE_ABOVE\n");
	}
	if (k->vy < 0.0f && (d || dl || dr)) {
		coll = (coll | COLLIDE_BELOW);
		*fixy = (float)ky;//(ky - 1)+(0.5f + k->ry);
		//printf("COLLIDE_BELOW %f\n", *fixy);
	}
	//if (coll == 0) printf("COLLIDE_NONE\n");
	return coll;
}

void run_trifle() {
	level_layers = 5;
	collision_layer = 1;
	level = level_trifle;
	// setup physics - use the defaults
	phys_def phys;
	init_default_phys(&phys);
	// check physics - last second the player can terminate the jump
	float tt = phys.jump_time - ((2 * (phys.max_jump_h - phys.min_jump_h)) / (phys.v_jump + phys.v_term_jump) );
	printf("g is %f\n", phys.g);
	printf("v_jump is %f\n", phys.v_jump);
	printf("v_term_jump is %f\n", phys.v_term_jump);
	printf("abort is %f\n", tt);

	// variables for timing used in physics
	timespec t1, t2;
	bool loop = true;

	// keyboard event variables
	bool kdown[NUM_KEYS];
	bool kpress[NUM_KEYS];
	for (int i=0; i<NUM_KEYS; i++) {
		kdown[i] = false;
		kpress[i] = false;
	}
	int key_map[NUM_KEYS];
	set_default_key_map(key_map);
	mouse_input mouse;

	// initialize the screen view
	screen_def sd;
	init_screen(&sd, SCREEN_W, SCREEN_H, 32);
	//glViewport(-32, -32, 512, 320);
	print_screen_def(&sd);

	const char *config_filename = "/Users/dmk/code/pectin/config/render_defs.cfg";
	// setup rendering for particles (water and boids)
	render_def *part_def = load_render_def(config_filename, "particles", (GLfloat *)&sd.vp_mat);
	// set up rendering for the player character
	render_def *trif_def = load_render_def(config_filename, "player", (GLfloat *)&sd.vp_mat);
	// set up rendering for blocks
	render_def *block_def = load_render_def(config_filename, "blocks", (GLfloat *)&sd.vp_mat);

	// create some particles using the pobj structure (used for non-interactive things)
	pobj parts[360];
	float px = 25.0f;
	float py = 6.2f;
	for (int i=0; i<360; i++) {
		parts[i].x = px;
		parts[i].y = py;
		randomize_particle(&parts[i]);
		px += 0.2f;
		py += 0.2f;
		if (px > 31.0f) px = 25.0f;
		if (py > 8.6f) py = 6.2f;
	}

	// create some boids
	boid_list *bl = create_boids(
		num_boids,
		150.0f, // coziness
		150.0f, // conformity
		100.0f, // attraction
		0.15f, // speed
		0.25f, // repel distance
		0.0f, // min x
		0.0f, // min y
		32.0f, // max x
		16.0f // max y
	);

	// create the player, using the dobj structure (for dynamic objects that have physics and collide)
	dobj trif = {5.4f, 8, 0, 0, 0.499f, 0.499f, 0, 0, false, false, false, true, false, false};

	sprite *s = (sprite *)malloc(sizeof(sprite));
	tile_range lr = {0, 32, 0, 20};
	tile_range tr;
	get_tile_range(&sd, &tr, &lr);
	bool cam_moved = false;
	init_render_environment();
	printf("l=%d, r=%d, t=%d, b=%d\n", tr.l, tr.r, tr.t, tr.b);
	for (int l=0; l<level_layers; l++) {
		for (int y=tr.b; y<tr.t; y++) {
			for (int x=tr.l; x<tr.r; x++) {
				int tidx = thing_at(x, y, l);
				if (tidx == 0) continue;
				int ridx = (tidx - 1) / block_def->cols;
				int cidx = (tidx - 1) % block_def->cols;
				sprite_for((float)x, (float)y, l, ridx, cidx, s, block_def);
				render_sprite(block_def->rbuf, s);
			}
		}
	}
	clock_gettime(CLOCK_REALTIME, &t1);
	int mult = 2;
	int frame = 0;
	while (loop) {
		get_input(kdown, kpress, key_map, &mouse);
		if (kpress[KEY_QUIT]) {
			loop = false;
		} else if (kdown[KEY_DOWN]) {
			sd.cam_pos.y -= 0.25f;
			if (sd.cam_pos.y < 5.0f) sd.cam_pos.y = 5.0f;
			cam_moved = true;
		}
		/*
		if (cam_moved) {
			update_proj_mat(&sd);
			update_view_mat(rb, (GLfloat *)&sd.vp_mat);
			update_view_mat(trb, (GLfloat *)&sd.vp_mat);
			int cnt = 0;
			render_advance(rb);
			get_tile_range(&sd, &tr, &lr);
			//printf("l=%d, r=%d, t=%d, b=%d\n", tr.l, tr.r, tr.t, tr.b);
			for (int x=tr.l; x<tr.r; x++) {
				for (int y=tr.b; y<tr.t; y++) {
					int off = (((level_h-1)-y)*level_w)+x;
					int ridx = level[off];
					sprite_for((float)x, (float)y, (float)cnt*-0.02f, ridx, 0, s, &block_def);
					render_sprite(rb, s);
					cnt++;
				}
			}
			cam_moved = false;
		}
		*/
		clock_gettime(CLOCK_REALTIME, &t2);
		double dt = time_diff_d(t1, t2);
		update_dobj(&phys, &trif, (float) dt, kdown[KEY_LEFT], kdown[KEY_RIGHT], (kdown[KEY_FIRE] || kdown[KEY_UP]),
		            &handle_horz_collision, &handle_vert_collision);
		if (kdown[KEY_LEFT]) {
			trif.row = 4;
			mult = 3;
		} else if (kdown[KEY_RIGHT]) {
			trif.row = 5;
			mult = 3;
		} else {
			trif.row = 2;
			mult = 4;
		}
		trif.col = ((trif.col + 1) % (mult * 10));

		clock_gettime(CLOCK_REALTIME, &t1);

		render_advance(trif_def->rbuf);
		sprite_for(trif.x, trif.y, 1, trif.row, (trif.col / mult), s, trif_def);
		render_sprite(trif_def->rbuf, s);
		if ((frame % 2) == 0) update_boids(bl);
		bl->ax = s->x;
		bl->ay = s->y;

		render_advance(part_def->rbuf);
		for (int i=0; i<360; i++) {
			update_particle(&parts[i]);
			sprite_for_pobj(&parts[i], s, 1, part_def);
			render_sprite(part_def->rbuf, s);
		}
		for (int i=0; i<bl->num_boids; i++) {
			sprite_for_boid(bl->boids[i], s, 0, part_def);
			render_sprite(part_def->rbuf, s);
		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		render_buffer(block_def->rbuf);
		render_buffer(trif_def->rbuf);
		render_buffer(part_def->rbuf);
		swap_window();
		frame += 1;
		if (frame == 60) frame = 0;
	}
	free(s);
	free_shader_program(block_def->shader);
	free_shader_program(trif_def->shader);
	free_shader_program(part_def->shader);
	free_render_buf(block_def->rbuf);
	free_render_buf(trif_def->rbuf);
	free_render_buf(part_def->rbuf);
}

int project_trajectory(phys_def *phys, line *proj, float x, float y, float vx, float vy) {
	dobj o = {x, y, vx, vy, 0.499f, 0.499f, 0, 0, false, false, false, true, false, false};
	int ppcnt = 0;
	float lastx = x;
	float lasty = y;
	for (int i=0; i<20; i++) {
		update_np_dobj(phys, &o, (float) 0.05f, &handle_horz_collision, &handle_vert_collision);
		proj[i].p1.x = lastx;
		proj[i].p1.y = lasty;
		proj[i].p1.z = 0.5f;
		proj[i].p1.r = 1.0f;
		proj[i].p1.g = 1.0f;
		proj[i].p1.b = 1.0f;

		proj[i].p2.x = o.x;
		proj[i].p2.y = o.y;
		proj[i].p2.z = 0.5f;
		proj[i].p2.r = 1.0f;
		proj[i].p2.g = 1.0f;
		proj[i].p2.b = 1.0f;

		if (!o.in_air) break;
		lastx = o.x;
		lasty = o.y;
		ppcnt++;
	}
	return ppcnt;
}

static cpBody *add_kbar(cpSpace *space, cpVect a, cpVect b, int group)
{
	cpVect center = cpvmult(cpvadd(a, b), 1.0f/2.0f);
	cpFloat length = cpvlength(cpvsub(b, a));

	cpBody *body = cpSpaceAddBody(space, cpBodyNewKinematic());
	cpBodySetPosition(body, center);
	cpBodySetAngle(body, 0);

	//cpShape *shape = cpSpaceAddShape(space, cpSegmentShapeNew(body, a, b, 0.1f));
	cpVect av = cpvsub(a, center);
	cpVect bv = cpvsub(b, center);
	cpShape *shape = cpSpaceAddShape(space, cpSegmentShapeNew(body, av, bv, 0.0f));
	cpShapeSetFilter(shape, cpShapeFilterNew(group, CP_ALL_CATEGORIES, CP_ALL_CATEGORIES));

	return body;
}

static cpBody *add_bar(cpSpace *space, cpVect a, cpVect b, int group)
{
	cpVect center = cpvmult(cpvadd(a, b), 1.0f/2.0f);
	cpFloat length = cpvlength(cpvsub(b, a));
	cpFloat mass = length;

	cpBody *body = cpSpaceAddBody(space, cpBodyNew(mass, mass*length*length/1.2f));
	cpBodySetPosition(body, center);
	cpBodySetAngle(body, 0);

	cpVect av = cpvsub(a, center);
	cpVect bv = cpvsub(b, center);
	cpShape *shape = cpSpaceAddShape(space, cpSegmentShapeNew(body, av, bv, 0.0f));
	cpShapeSetFilter(shape, cpShapeFilterNew(group, CP_ALL_CATEGORIES, CP_ALL_CATEGORIES));

	return body;
}

void line_for(cpVect pos, cpFloat ang, cpFloat r, line *l) {
	// position as if about the origin
	cpFloat x = (r * sin(ang));
	cpFloat y = (r * cos(ang));
	l->p1.x = (float)(x + pos.x);
	l->p1.y = (float)(y + pos.y);
	l->p2.x = (float)((x * -1.0) + pos.x);
	l->p2.y = (float)((y * -1.0) + pos.y);
}


void run_throw() {
	cpSpace *space = cpSpaceNew();
	cpSpaceSetIterations(space, 10);
	cpSpaceSetGravity(space, cpv(0, -30));
	cpSpaceSetSleepTimeThreshold(space, 0.5f);

	cpBody *staticBody = cpSpaceGetStaticBody(space);

	for (int i=0; i<static_cnt; i++) {
		cpVect verts[] = {
			{statics[i].x1,statics[i].y1},
			{statics[i].x1,statics[i].y2},
			{statics[i].x2,statics[i].y2},
			{statics[i].x2,statics[i].y1}
		};
		cpSpaceAddShape(space, cpPolyShapeNewRaw(staticBody, 4, verts, 0.0f));
	}

	int num_bodies = 6;
	cpBody *last_body = staticBody;
	cpVect start_pos = cpv(20, 1);
	cpBody **plant = (cpBody **)malloc(sizeof(cpBody *) * num_bodies);
	for (int i=0; i<num_bodies; i++) {
		cpFloat cur_y = start_pos.y + ((cpFloat)i);
		plant[i] = add_bar(space, cpv(start_pos.x, cur_y), cpv(start_pos.x, cur_y+1), 1);
		cpSpaceAddConstraint(space, cpPivotJointNew(plant[i],  last_body, cpv(start_pos.x, cur_y)));
		cpSpaceAddConstraint(space, cpDampedRotarySpringNew(plant[i], last_body, 0, 2000.0f, 10.0f));
		last_body = plant[i];
	}

	/*
	cpBody *kbody = add_bar(space, cpv(28, 1), cpv(28, 2), 1);
	cpBody *body1  = add_bar(space, cpv(28, 2), cpv(28, 3), 1);
	cpBody *body2  = add_bar(space, cpv(28, 3), cpv(28, 4), 1);
	cpSpaceAddConstraint(space, cpPivotJointNew( kbody,  staticBody, cpv(28, 1)));
	cpSpaceAddConstraint(space, cpPivotJointNew( body1,  kbody, cpv(28, 2)));
	cpSpaceAddConstraint(space, cpPivotJointNew( body2,  body1, cpv(28, 3)));
	cpSpaceAddConstraint(space, cpDampedRotarySpringNew(kbody, staticBody, 0, 1000.0f, 20.0f));
	cpSpaceAddConstraint(space, cpDampedRotarySpringNew(body1, kbody , 0, 1000.0f, 20.0f));
	cpSpaceAddConstraint(space, cpDampedRotarySpringNew(body2, body1 , 0, 1000.0f, 20.0f));
	*/


	level_layers = 1;
	collision_layer = 0;
	level = level_throw;
	// setup physics - use the defaults
	phys_def phys;
	init_default_phys(&phys);
	// check physics - last second the player can terminate the jump
	float tt = phys.jump_time - ((2 * (phys.max_jump_h - phys.min_jump_h)) / (phys.v_jump + phys.v_term_jump) );
	printf("g is %f\n", phys.g);
	printf("v_jump is %f\n", phys.v_jump);
	printf("v_term_jump is %f\n", phys.v_term_jump);
	printf("abort is %f\n", tt);

	// variables for timing used in physics
	timespec t1, t2;
	bool loop = true;

	// keyboard event variables
	bool kdown[NUM_KEYS];
	bool kpress[NUM_KEYS];
	for (int i=0; i<NUM_KEYS; i++) {
		kdown[i] = false;
		kpress[i] = false;
	}
	int key_map[NUM_KEYS];
	set_default_key_map(key_map);
	mouse_input mouse;
	float wmx = 0.0f;
	float wmy = 0.0f;

	// initialize the screen view
	screen_def sd;
	init_screen(&sd, SCREEN_W, SCREEN_H, 32);
	//glViewport(-32, -32, 512, 320);
	print_screen_def(&sd);

	const char *config_filename = "/Users/dmk/code/pectin/config/throw_render_defs.cfg";
	// set up rendering for the player character
	render_def *ball_def = load_render_def(config_filename, "ball", (GLfloat *)&sd.vp_mat);
	// set up rendering for blocks
	render_def *block_def = load_render_def(config_filename, "blocks", (GLfloat *)&sd.vp_mat);
	// set up rendering for lines
	render_def *line_def = load_render_def(config_filename, "lines", (GLfloat *)&sd.vp_mat);
	// set up rendering for font
	render_def *font_def = load_render_def(config_filename, "font", (GLfloat *)&sd.vp_mat);

	// create the player, using the dobj structure (for dynamic objects that have physics and collide)
	dobj ball = {16, 8, 0, 0, 0.499f, 0.499f, 2, 0, false, false, false, true, false, false};

	cpFloat mass = 1.0f;
	cpFloat r = 0.5f;
	cpBody *body = cpSpaceAddBody(space, cpBodyNew(mass, cpMomentForCircle(mass, 0.0f, r, cpvzero)));
	cpBodySetPosition(body, cpv(16.5f, 8.5f));
	cpSpaceAddShape(space, cpCircleShapeNew(body, r, cpvzero));

	line ln = {{16, 8, 0, 0.0f, 0.5f, 0.0f}, {8, 12, 0, 0.0f, 0.5f, 0.0f}};

	//const char *txt = "This is an [example] of some {text}. Who knows if it has mojo?";
	const char *txt = "This is some text that will be on the screen";
	float txt_x = 1.0f;
	float txt_y = 18.0f;
	int txt_len = (int)strlen(txt);

	printf("sizeof(line) is %d\n", (int)sizeof(line));
	line *project = (line *)malloc(20 * sizeof(line));

	sprite *s = (sprite *)malloc(sizeof(sprite));
	tile_range lr = {0, 32, 0, 20};
	tile_range tr;
	get_tile_range(&sd, &tr, &lr);
	bool cam_moved = false;
	init_render_environment();
	for (int y=tr.b; y<tr.t; y++) {
		for (int x=tr.l; x<tr.r; x++) {
			int tidx = thing_at(x, y, 0);
			if (tidx == 0) continue;
			int ridx = (tidx - 1) / block_def->cols;
			int cidx = (tidx - 1) % block_def->cols;
			sprite_for((float)x, (float)y, 0, ridx, cidx, s, block_def);
			render_sprite(block_def->rbuf, s);
		}
	}

	int advance = 0;
	float tx = txt_x;
	for (int i=0; i<txt_len; i++) {
		int ch = txt[i] - 32;
		int trow = ch / 24;
		int tcol = ch % 24;
		int back = font_back[ch];
		float kern = (((float)font_kern / 8.0f) * font_def->xscale);
		tx += (((float)advance / 8.0f) * font_def->xscale);
		tx += kern;
		if (back < 0) tx += (((float)back / 8.0f) * font_def->xscale);
		sprite_for(tx, txt_y, 0, trow, tcol, s, font_def);
		render_sprite(font_def->rbuf, s);
		advance = font_space[ch];
	}
	clock_gettime(CLOCK_REALTIME, &t1);
	int mult = 2;
	int frame = 0;
	int proj_cnt = 0;
	while (loop) {
		get_input(kdown, kpress, key_map, &mouse);
		if (kpress[KEY_QUIT]) {
			loop = false;
		}
		cpVect cur_vel = cpBodyGetVelocity(body);
		if (kdown[KEY_LEFT]) {
			cpBodySetVelocity(body, cpv(-5.0, cur_vel.y));
		} else if (kdown[KEY_RIGHT]) {
			cpBodySetVelocity(body, cpv(5.0, cur_vel.y));
		} else if (kdown[KEY_FIRE]) {
			cpBodySetVelocity(body, cpv(cur_vel.x, 10.0));
		}
		clock_gettime(CLOCK_REALTIME, &t2);
		double dt = time_diff_d(t1, t2);
		cpSpaceStep(space, dt);
		//update_dobj(&phys, &ball, (float) dt, kdown[KEY_LEFT], kdown[KEY_RIGHT], (kdown[KEY_FIRE] || kdown[KEY_UP]),
		//            &handle_horz_collision, &handle_vert_collision);
		clock_gettime(CLOCK_REALTIME, &t1);

		render_advance(ball_def->rbuf);
		cpVect ball_pos = cpBodyGetPosition(body);
		ball.x = (float)(ball_pos.x - 0.5);
		ball.y = (float)(ball_pos.y - 0.5);
		sprite_for(ball.x, ball.y, 1, ball.row, (ball.col / mult), s, ball_def);
		render_sprite(ball_def->rbuf, s);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		render_advance(line_def->rbuf);
		ln.p2.x = (float)start_pos.x;
		ln.p2.y = (float)start_pos.y;
		for (int i=0; i<num_bodies; i++) {
			cpVect lpos = cpBodyGetPosition(plant[i]);
			ln.p1.x = ln.p2.x;
			ln.p1.y = ln.p2.y;
			ln.p2.x = (float)lpos.x;
			ln.p2.y = (float)lpos.y;
			render_line(line_def->rbuf, &ln);
		}
		/*
		//cpFloat lang = cpBodyGetAngle(kbody);
		cpVect lpos = cpBodyGetPosition(kbody);
		ln.p1.x = startx;
		ln.p1.y = starty;
		ln.p2.x = (float)lpos.x;
		ln.p2.y = (float)lpos.y;
		//line_for(lpos, lang, 0.5, &ln);
		render_line(line_def->rbuf, &ln);
		//lang = cpBodyGetAngle(body1);
		lpos = cpBodyGetPosition(body1);
		ln.p1.x = ln.p2.x;
		ln.p1.y = ln.p2.y;
		ln.p2.x = (float)lpos.x;
		ln.p2.y = (float)lpos.y;
		//line_for(lpos, lang, 0.5, &ln);
		render_line(line_def->rbuf, &ln);
		//lang = cpBodyGetAngle(body2);
		lpos = cpBodyGetPosition(body2);
		ln.p1.x = ln.p2.x;
		ln.p1.y = ln.p2.y;
		ln.p2.x = (float)lpos.x;
		ln.p2.y = (float)lpos.y;
		//line_for(lpos, lang, 0.5, &ln);
		render_line(line_def->rbuf, &ln);
		*/

		/*
		ln.p2.x = ball.x + 0.5f;
		ln.p2.y = ball.y + 0.5f;
		if (mouse.ldown) {
			mouse_to_world(&sd, mouse.x, mouse.y, &wmx, &wmy);
			//printf("mouse: (%f, %f)\n", wmx, wmy);
			ln.p1.x = wmx;
			ln.p1.y = wmy;

			float pvx = (ln.p2.x - ln.p1.x) * 5.0f;
			float pvy = (ln.p2.y - ln.p1.y) * 5.0f;
			proj_cnt = project_trajectory(&phys, project, ln.p2.x, ln.p2.y, pvx, pvy);
		} else {
			proj_cnt = 0;
			ln.p1.x = ball.x + 0.5f;
			ln.p1.y = ball.y + 0.5f;
			ln.p2.x = ball.x + 0.5f;
			ln.p2.y = ball.y + 0.5f;
		}
		render_line(line_def->rbuf, &ln);
		for (int i=0; i<proj_cnt; i++) {
			render_line(line_def->rbuf, &project[i]);
		}
		*/
		render_buffer(line_def->rbuf);

		render_buffer(block_def->rbuf);
		render_buffer(ball_def->rbuf);
		render_buffer(font_def->rbuf);
		swap_window();
		frame += 1;
		if (frame == 60) frame = 0;
	}
	free(s);
	free_shader_program(block_def->shader);
	free_shader_program(ball_def->shader);
	free_shader_program(line_def->shader);
	free_render_buf(block_def->rbuf);
	free_render_buf(ball_def->rbuf);
	free_render_buf(line_def->rbuf);
}

int main(int argc, char *argv[]) {
	if (!init_window("test sdl_ogl", SCREEN_W, SCREEN_H)) {
		return -1;
	}
	print_sdl_gl_attributes();

	run_throw();

	cleanup_window();

	return 0;
}


