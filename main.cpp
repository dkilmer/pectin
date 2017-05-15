#include <time.h>
#include <glad/glad.h>
#include "window.h"
#include "render_util.h"
#include "game_util.h"
#include "misc_util.h"
#include "physics.h"

int level[] = {
	8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
	8, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 8,
	8, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 8,
	8, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 8,
	8, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 8,
	8, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 8,
	8, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 8,
	8, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 8,
	8, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 8,
	8, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 8,
	8, 8, 8, 8, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 8,
	8, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 8,
	8, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 8,
	8, 2, 2, 2, 2, 8, 8, 8, 8, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 8,
	8, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 8,
	8, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 8,
	8, 2, 8, 2, 2, 2, 2, 2, 2, 8, 8, 8, 8, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 8,
	8, 2, 8, 2, 2, 2, 2, 2, 2, 2, 2, 2, 8, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 8,
	8, 2, 8, 2, 2, 2, 2, 2, 2, 2, 2, 2, 8, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 8,
	8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8
};
int level_w = 32;
int level_h = 20;

void random_sprite(sprite *s) {
	s->x = rand_float() * 32.0f;
	s->y = rand_float() * 23.0f;
	s->z = 0.0f;
	s->r = 0.0f;
	s->g = 0.0f;
	s->b = 0.0f;
	s->scale_x = 1.0f;
	s->scale_y = 1.0f;
	s->rot = 0.0f;
	s->spr_row = rand_int(4);
	s->spr_col = rand_int(4);
	s->spr_extra = 0.0f;
}

void aabb_for(float x, float y, aabb *b) {
	b->x = x;
	b->y = y;
	b->rx = 0.5f;
	b->ry = 0.5f;
}

void sprite_for(float x, float y, int ssw, int idx, sprite *s) {
	s->x = x + 0.5f;
	s->y = y + 0.5f;
	s->z = 0.0f;
	s->r = 0.0f;
	s->g = 0.0f;
	s->b = 0.0f;
	s->scale_x = 0.5f;
	s->scale_y = 0.5f;
	s->rot = 0.0f;
	s->spr_row = idx / ssw;
	s->spr_col = idx % ssw;
	s->spr_extra = 0.0f;
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

int thing_at(int x, int y) {
	int off = (((level_h-1)-y)*level_w)+x;
	if (off < 0 || off >= (level_w * level_h)) return 8;
	return level[off];
}

bool block_at(int x, int y) {
	int off = (((level_h-1)-y)*level_w)+x;
	if (off < 0 || off >= (level_w * level_h)) return true;
	return (level[off] == 8);
}

bool collides_with(aabb *b, int x, int y) {
	if (!block_at(x, y)) return false;
	aabb sb;
	aabb_for(x, y, &sb);
	return collides(b, &sb);
}

int handle_horz_collision(kobj *k, float *fixx) {
	int coll = 0;
	// these are used to determine what blocks to check in the grid
	int kx = (int)floorf(k->x+0.5f);
	int ky = (int)floorf(k->y+0.5f);
	aabb kb = {k->x, k->y, k->rx, k->ry};
	bool u = block_at(kx, ky+1);
	bool d = block_at(kx, ky-1);

	bool ul = collides_with(&kb, kx-1, ky+1);
	bool ur = collides_with(&kb, kx+1, ky+1);
	bool dl = collides_with(&kb, kx-1, ky-1);
	bool dr = collides_with(&kb, kx+1, ky-1);
	bool l = collides_with(&kb, kx-1, ky);
	bool r = collides_with(&kb, kx+1, ky);

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

int handle_vert_collision(kobj *k, float *fixy) {
	int coll = 0;
	// these are used to determine what blocks to check in the grid
	int kx = (int)floorf(k->x+0.5f);
	int ky = (int)floorf(k->y+0.5f);
	aabb kb = {k->x, k->y, k->rx, k->ry};
	bool ul = collides_with(&kb, kx-1, ky+1);
	bool u = collides_with(&kb, kx, ky+1);
	bool ur = collides_with(&kb, kx+1, ky+1);
	bool dl = collides_with(&kb, kx-1, ky-1);
	bool d = collides_with(&kb, kx, ky-1);
	bool dr = collides_with(&kb, kx+1, ky-1);

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

void run() {
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

	screen_def sd;
	init_screen(&sd, 1024, 640, 32);
	//glViewport(-32, -32, 512, 320);
	print_screen_def(&sd);

	GLuint ttex;
	GLuint tshader = create_geom_shader_program(
		"/Users/dmk/code/pectin/shaders/sprite_vert.glsl",
		"/Users/dmk/code/pectin/shaders/sprite_geom.glsl",
		"/Users/dmk/code/pectin/shaders/sprite_frag_simple.glsl",
		"/Users/dmk/code/pectin/images/trifle_diff.png",
		10, 6,
		(GLfloat *)&sd.vp_mat,
	  &ttex
	);
	render_buf *trb = create_render_buf(300, tshader, ttex);
	kobj trif = {5.4f, 1, 0, 0, 0.499f, 0.499f, false, false, false, true, false, false};

	GLuint tex;
	GLuint shader = create_geom_shader_program(
		"/Users/dmk/code/pectin/shaders/sprite_vert.glsl",
		"/Users/dmk/code/pectin/shaders/sprite_geom.glsl",
		"/Users/dmk/code/pectin/shaders/sprite_frag_simple.glsl",
		"/Users/dmk/code/pectin/images/sprites_diff.png",
	  8, 8,
		(GLfloat *)&sd.vp_mat,
	  &tex
	);
	render_buf *rb = create_render_buf(640, shader, tex);

	sprite *s = (sprite *)malloc(sizeof(sprite));
	tile_range lr = {0, 32, 0, 20};
	tile_range tr;
	get_tile_range(&sd, &tr, &lr);
	bool cam_moved = false;
	init_render_environment();
	printf("l=%d, r=%d, t=%d, b=%d\n", tr.l, tr.r, tr.t, tr.b);
	for (int x=tr.l; x<tr.r; x++) {
		for (int y=tr.b; y<tr.t; y++) {
			int idx = thing_at(x, y);
			if (idx == 2) idx = rand_int(4);
			sprite_for((float)x, (float)y, 8, idx, s);
			render_sprite(rb, s);
		}
	}
	clock_gettime(CLOCK_REALTIME, &t1);
	while (loop) {
		get_input(kdown, kpress, key_map);
		if (kpress[KEY_QUIT]) {
			loop = false;
		} else if (kdown[KEY_DOWN]) {
			sd.cam_pos.y -= 0.25f;
			if (sd.cam_pos.y < 5.0f) sd.cam_pos.y = 5.0f;
			cam_moved = true;
		}
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
					int idx = level[off];
					sprite_for((float)x, (float)y, 8, idx, s);
					render_sprite(rb, s);
					cnt++;
				}
			}
			cam_moved = false;
		}
		clock_gettime(CLOCK_REALTIME, &t2);
		double dt = time_diff_d(t1, t2);
		update_kobj(&phys, &trif, (float)dt, kdown[KEY_LEFT], kdown[KEY_RIGHT], (kdown[KEY_FIRE] || kdown[KEY_UP]), &handle_horz_collision, &handle_vert_collision);

		clock_gettime(CLOCK_REALTIME, &t1);

		render_advance(trb);
		sprite_for(trif.x, trif.y, 10, 0, s);
		render_sprite(trb, s);
		glClear(GL_COLOR_BUFFER_BIT);
		render_sprites(rb);
		render_sprites(trb);
		swap_window();
	}
	free(s);
	free_geom_shader_program(tshader);
	free_geom_shader_program(shader);
	free_render_buf(rb);
	free_render_buf(trb);
}

int main(int argc, char *argv[]) {
	if (!init_window("test sdl_ogl", 1024, 640)) {
		return -1;
	}
	print_sdl_gl_attributes();

	run();

	cleanup_window();

	return 0;
}


