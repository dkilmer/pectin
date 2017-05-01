#include <glad/glad.h>
#include "window.h"
#include "render_util.h"
#include "game_util.h"
#include "misc_util.h"

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

void sprite_for(int x, int y, int idx, sprite *s) {
	s->x = (float)x + 0.5f;
	s->y = (float)y + 0.5f;
	s->z = 0.0f;
	s->r = 0.0f;
	s->g = 0.0f;
	s->b = 0.0f;
	s->scale_x = 0.5f;
	s->scale_y = 0.5f;
	s->rot = 0.0f;
	s->spr_row = idx / 8;
	s->spr_col = idx % 8;
	s->spr_extra = 0.0f;
}

void run() {
	int level[] = {
		8, 8, 8, 8, 8, 8, 8, 0, 0, 0, 0, 0, 0, 0, 8, 8, 8, 8, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		8, 8, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0,
		8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0,
		8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0,
		8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0,
		8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 8, 8, 8, 8, 8, 8, 8,
		0, 0, 0, 0, 0, 8, 8, 8, 0, 8, 8, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 8, 0, 8, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 8, 8, 8, 0, 8, 8, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 8, 0, 8, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 8, 8, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0,
		0, 0, 0, 0, 0, 8, 8, 8, 0, 8, 8, 8, 0, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 8, 8, 8, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 8, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		8, 8, 8, 8, 8, 8, 8, 8, 0, 0, 0, 0, 8, 8, 8, 8, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 8, 0, 0, 0
	};

	bool loop = true;
	bool kdown[NUM_KEYS];
	bool kpress[NUM_KEYS];
	for (int i=0; i<NUM_KEYS; i++) {
		kdown[i] = false;
		kpress[i] = false;
	}
	int key_map[NUM_KEYS];
	set_default_key_map(key_map);

	screen_def sd;
	init_screen(&sd, 512, 320, 32);
	print_screen_def(&sd);

	GLuint shader = create_geom_shader_program(
		"/Users/dmk/code/pectin/shaders/sprite_vert.glsl",
		"/Users/dmk/code/pectin/shaders/sprite_geom.glsl",
		"/Users/dmk/code/pectin/shaders/sprite_frag_simple.glsl",
		"/Users/dmk/code/gl-sandbox/images/sprites_diff.png",
	  8, 8,
		(GLfloat *)&sd.vp_mat
	);
	render_buf *rb = create_render_buf(216, shader);
	sprite *s = (sprite *)malloc(sizeof(sprite));
	tile_range tr;
	get_tile_range(&sd, &tr);
	bool cam_moved = false;
	init_render_environment();
	printf("l=%d, r=%d, t=%d, b=%d\n", tr.l, tr.r, tr.t, tr.b);
	for (int x=tr.l; x<tr.r; x++) {
		for (int y=tr.b; y<tr.t; y++) {
			int off = ((19-y)*32)+x;
			int idx = level[off];
			//printf("cnt=%d, x=%d, y=%d, off=%d, idx=%d\n", cnt, x, y, off, idx);
			sprite_for(x, y, idx, s);
			render_sprite(rb, s);
		}
	}
	while (loop) {
		get_input(kdown, kpress, key_map);
		if (kpress[KEY_QUIT]) {
			loop = false;
		} else if (kdown[KEY_LEFT]) {
			if (sd.cam_pos.x > 8.0f) {
				sd.cam_pos.x -= 0.1f;
				cam_moved = true;
			}
		} else if (kdown[KEY_RIGHT]) {
			if (sd.cam_pos.x < 24.0f) {
				sd.cam_pos.x += 1.0f;
				cam_moved = true;
			}
		} else if (kdown[KEY_UP]) {
			if (sd.cam_pos.y < 15.0f) {
				sd.cam_pos.y += 0.1f;
				cam_moved = true;
			}
		} else if (kdown[KEY_DOWN]) {
			if (sd.cam_pos.y > 5.0f) {
				sd.cam_pos.y -= 0.1f;
				cam_moved = true;
			}
		}
		if (cam_moved) {
			int cnt = 0;
			render_advance(rb);
			get_tile_range(&sd, &tr);
			printf("l=%d, r=%d, t=%d, b=%d\n", tr.l, tr.r, tr.t, tr.b);
			for (int x=tr.l; x<tr.r; x++) {
				for (int y=tr.b; y<tr.t; y++) {
					int off = ((19-y)*32)+x;
					int idx = level[off];
					sprite_for(x, y, idx, s);
					printf("cnt=%d, x=%d, y=%d, off=%d, idx=%d\n", cnt, x, y, off, idx);
					render_sprite(rb, s);
					//printf("%d\n", cnt);
					cnt++;
				}
			}
			cam_moved = false;
		}
		glClear(GL_COLOR_BUFFER_BIT);
		render_sprites(rb);
		swap_window();
	}
	free(s);
}

int main(int argc, char *argv[]) {
	if (!init_window("test sdl_ogl", 512, 320)) {
		return -1;
	}
	print_sdl_gl_attributes();

	run();

	cleanup_window();

	return 0;
}


