#include <stdlib.h>
#include "example_pbox.h"
#include "window.h"
#include "example_common.h"
#include "render_util.h"
#include "game_util.h"

#define TPI 6.2831853072f

void sprite_for_pbox(float x, float y, float z, float r, float g, float b, sprite *s, render_def *rd) {
	s->x = (x + (rd->xscale * 0.5f) + rd->xoff);
	s->y = (y + (rd->yscale * 0.5f) + rd->yoff);
	s->z = z;
	s->r = r;
	s->g = g;
	s->b = b;
	s->scale_x = rd->xscale * 0.5f;
	s->scale_y = rd->yscale * 0.5f;
	s->rot = 0.0f;
	s->spr_idx = 0;
	s->spr_cols = 0;
	s->spr_extra = 0;
}

void run_pbox() {
	screen_w = 1024;
	screen_h = 640;
	if (!init_window("test sdl_ogl", screen_w, screen_h)) return;
	print_sdl_gl_attributes();
	font_kern = 1;

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
	int pix_per_unit = 32;
	screen_def sd;
	init_screen(&sd, screen_w, screen_h, pix_per_unit, true);
	print_screen_def(&sd);

	const char *config_filename = "/Users/dmk/code/pectin/config/pbox_render_defs.cfg";
	// set up rendering for lines
	render_def *pb_def = load_render_def(config_filename, "points", (GLfloat *)&sd.vp_mat, NULL, NULL);
	render_def *line_def = load_render_def(config_filename, "lines", (GLfloat *)&sd.vp_mat, NULL, NULL);

	sprite *s = (sprite *)malloc(sizeof(sprite));
	line l1 = {{16.0f, 2.0f, -0.01f, 0.6f, 0.6f, 0.6f}, {16.0f, 18.0f, -0.01f, 0.6f, 0.6f, 0.6f}};
	line l2 = {{8.0f, 10.0f, -0.01f, 0.6f, 0.6f, 0.6f}, {24.0f, 10.0f, -0.01f, 0.6f, 0.6f, 0.6f}};
	init_render_environment(false);

	clock_gettime(CLOCK_REALTIME, &t1);
	int frame = 0;
	float xx;
	float yy;
	while (loop) {
		get_input(kdown, kpress, key_map, &mouse);
		if (kpress[KEY_QUIT]) {
			loop = false;
		}
		clock_gettime(CLOCK_REALTIME, &t2);
		double dt = time_diff_d(t1, t2);

		if (mouse.ldown) {
			mouse_to_world(&sd, mouse.x, mouse.y, &wmx, &wmy);
		}

		glClear(GL_COLOR_BUFFER_BIT);
		render_advance(pb_def);
		render_advance(line_def);

		float angle = ((float)frame / 360.0f) * TPI;
		xx = cosf(angle) * sinf(angle * 2.0f) * 4.0f;
		yy = sinf(angle) * cosf(angle * 2.0f) * 4.0f;

		render_line(line_def, &l1);
		render_line(line_def, &l2);

		sprite_for_pbox(16.0f+xx, 10.0f+yy, 0, 1, 1, 1, s, pb_def);
		render_sprite(pb_def, s);

		render_buffer(line_def);
		render_buffer(pb_def);
		swap_window();
		frame += 1;
		if (frame == 360) frame = 0;
	}
	free_render_def(pb_def);
	free_render_def(line_def);
	free(s);
}
