#include "game_util.h"

void init_screen(screen_def *s, int sw, int sh, int ts) {
	s->screen_w = sw;
	s->screen_h = sh;
	s->tile_w = ts;
	s->tile_h = ts;
	// set up a default camera as if the bottom left corner will be at 0,0
	s->h_units = (float)sw / (float)ts;
	s->v_units = (float)sh / (float)ts;
	s->half_w = s->h_units / 2.0f;
	s->half_h = s->v_units / 2.0f;
	s->near = 0.1f;
	s->far = 100.0f;
	s->cam_pos.x = s->half_w;
	s->cam_pos.y = s->half_h;
	s->cam_pos.z = 10.0f;
	update_proj_mat(s);
}

void update_proj_mat(screen_def *s) {
	vec3_t cat = {s->cam_pos.x, s->cam_pos.y, 0.0f};
	vec3_t up = {0.0f, 1.0f, 0.0f};
	mat4_t p_mat = m4_ortho(-s->half_w, s->half_w, -s->half_h, s->half_h, s->far, s->near);
	mat4_t v_mat = m4_look_at(s->cam_pos, cat, up);
	mat4_t m_mat = m4_identity();
	s->vp_mat = m4_mul(m4_mul(p_mat, v_mat), m_mat);
}


void print_screen_def(screen_def *s) {
	printf("screen_w: %d\n", s->screen_w);
	printf("screen_h: %d\n", s->screen_h);
	printf("tile_w: %d\n", s->tile_w);
	printf("tile_h: %d\n", s->tile_h);
	printf("h_units: %f\n", s->h_units);
	printf("v_units: %f\n", s->v_units);
	printf("near: %f\n", s->near);
	printf("far: %f\n", s->far);
	printf("cam_pos: x=%f, y=%f, z=%f\n", s->cam_pos.x, s->cam_pos.y, s->cam_pos.z);
}

void get_tile_range(screen_def *s, tile_range *tr, tile_range *level_range) {
	tr->l = (int)floorf(s->cam_pos.x - s->half_w);
	tr->r = (int)ceilf(s->cam_pos.x + s->half_w);
	tr->b = (int)floorf(s->cam_pos.y - s->half_h);
	tr->t = (int)ceilf(s->cam_pos.y + s->half_h);
	tr->l--;
	tr->r++;
	tr->b--;
	tr->t++;
	if (tr->l < level_range->l) tr->l = level_range->l;
	if (tr->b < level_range->b) tr->b = level_range->b;
	if (tr->r > level_range->r) tr->r = level_range->r;
	if (tr->t > level_range->t) tr->t = level_range->t;
}
