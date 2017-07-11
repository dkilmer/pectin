#include "game_util.h"
#define MATH_3D_IMPLEMENTATION
#include "math_3d.h"

void init_screen(screen_def *s, int sw, int sh, int ts, bool ortho) {
	s->screen_w = sw;
	s->screen_h = sh;
	s->tile_w = ts;
	s->tile_h = ts;
	// set up a default camera as if the bottom left corner will be at 0,0
	s->h_units = (float)sw / (float)ts;
	s->v_units = (float)sh / (float)ts;
	s->half_w = s->h_units / 2.0f;
	s->half_h = s->v_units / 2.0f;
	s->far = 200.0f;
	s->fov = 50.0f;
	s->cam_pos.x = s->half_w;
	s->cam_pos.y = s->half_h;
	float rads = (s->fov / 2.0f) * (float)ONE_DEG_IN_RAD;
	float tang = tanf(rads);
	s->cam_pos.z = (ortho) ? 10.0f : (s->half_h / tang);
	s->near = s->cam_pos.z - 0.01f;

	printf("half_w is %f z is %f rads is %f tang is %f\n", s->half_w, s->cam_pos.z, rads, tang);
	update_proj_mat(s, ortho);
}

void update_proj_mat(screen_def *s, bool ortho) {
	float aspect = (float)s->screen_w / (float)s->screen_h; // aspect ratio
	vec3_t cat = {s->cam_pos.x, s->cam_pos.y, 0.0f};
	vec3_t up = {0.0f, 1.0f, 0.0f};
	mat4_t p_mat = (ortho) ? m4_ortho(-s->half_w, s->half_w, -s->half_h, s->half_h, s->far, s->near)
			: m4_perspective(s->fov, aspect, s->near, s->far);
	mat4_t v_mat = m4_look_at(s->cam_pos, cat, up);
	mat4_t m_mat = m4_identity();
	s->vp_mat = m4_mul(m4_mul(p_mat, v_mat), m_mat);
}

mat4_t get_light_mat(screen_def *s, vec3_t light_pos) {
	//vec3_t cat = {light_pos.x, light_pos.y, light_pos.z};
	vec3_t cat = {0.0f, 0.0f, 0.0f};
	vec3_t up = {0.0f, 1.0f, 0.0f};
	mat4_t p_mat = m4_ortho(-s->half_w * 3.0f, s->half_w * 3.0f, -s->half_h * 3.0f, s->half_h * 3.0f, 1000.0f, 0.1f);
	mat4_t v_mat = m4_look_at(light_pos, cat, up);
	mat4_t m_mat = m4_identity();
	return m4_mul(m4_mul(p_mat, v_mat), m_mat);
}

mat4_t get_light_bias_mat(screen_def *s, vec3_t light_pos) {
	mat4_t depth_mvp_mat = get_light_mat(s, light_pos);
	mat4_t bias_mat = {
		0.5, 0.0, 0.0, 0.0,
		0.0, 0.5, 0.0, 0.0,
		0.0, 0.0, 0.5, 0.0,
		0.5, 0.5, 0.5, 1.0
	};
	return m4_mul(bias_mat, depth_mvp_mat);
}

void mouse_to_world(screen_def *s, int mx, int my, float *wx, float *wy) {
	*wx = ((float)mx / (float)s->tile_w) + (s->cam_pos.x - s->half_w);
	*wy = ((float)(s->screen_h-my) / (float)s->tile_h) + (s->cam_pos.y - s->half_h);
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
