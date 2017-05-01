#ifndef SCREEN_UTIL_H
#define SCREEN_UTIL_H

#define MATH_3D_IMPLEMENTATION
#include "math_3d.h"

#if defined __cplusplus
extern "C" {
#endif

typedef struct tile_range {
	int l;
	int r;
	int b;
	int t;
} tile_range;

typedef struct screen_def {
	int screen_w; // screen pixel width
	int screen_h; // screen pixel height
	int tile_w; // tile pixel width
	int tile_h; // tile pixel height
	float h_units;
	float v_units;
	float half_w;
	float half_h;
	float near;
	float far;
	vec3_t cam_pos;
	mat4_t vp_mat;
} screen_def;

typedef struct layer {
	int layer_z;
	int level_w;
	int level_h;
	int *things;
} layer;

void init_screen(screen_def *s, int sw, int sh, int ts);
void print_screen_def(screen_def *s);
void get_tile_range(screen_def *s, tile_range *tr);


#ifdef __cplusplus
}
#endif

#endif //SCREEN_UTIL_H
