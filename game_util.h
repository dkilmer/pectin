#ifndef GAME_UTIL_H
#define GAME_UTIL_H

#include <stdbool.h>
#include "math_3d.h"

#define ONE_DEG_IN_RAD  (2.0f * M_PI) / 360.0f // 0.017444444

#if defined __cplusplus
extern "C" {
#endif

typedef struct { float x, y, z, w; } vec4_t;

// struct to hold a rect of tiles
typedef struct tile_range {
	int l;
	int r;
	int b;
	int t;
} tile_range;

// simple rectangle
typedef struct {
	float x1, y1, x2, y2;
} srect;

// light
typedef struct {
	vec4_t position;
	vec3_t intensities;
	float attenuation;
	float ambient_coefficient;
	float cone_angle;
	vec3_t cone_direction;
} light;

// struct to hold the definition of a screen view
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
	float fov;
	vec3_t cam_pos;
	mat4_t vp_mat;
} screen_def;

// struct to hold a dynamic object that is influenced by physics and collisions
typedef struct dobj {
	float x;
	float y;
	float vx;
	float vy;
	float rx;
	float ry;
	int row;
	int col;
	bool in_air;
	bool jumping;
	bool cut_jump;
	bool can_jump;
	bool moving_left;
	bool moving_right;
} dobj;

// struct to hold a "particle" object that is unaffected by physics
typedef struct pobj {
	float x;
	float y;
	float z_norm;
	float scale_norm_x;
	float scale_norm_y;
	float rot_norm;
	float r;
	float g;
	float b;
} pobj;


void init_screen(screen_def *s, int sw, int sh, int ts, bool ortho);
void update_proj_mat(screen_def *s, bool ortho);
mat4_t get_light_mat(screen_def *s, vec3_t light_pos);
mat4_t get_light_bias_mat(screen_def *s, vec3_t light_pos);
void mouse_to_world(screen_def *s, int mx, int my, float *wx, float *wy);
void print_screen_def(screen_def *s);
void get_tile_range(screen_def *s, tile_range *tr, tile_range *level_range);


#ifdef __cplusplus
}
#endif

#endif //GAME_UTIL_H
