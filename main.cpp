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

void run()
{
	bool loop = true;
	bool kdown[NUM_KEYS];
	bool kpress[NUM_KEYS];
	int key_map[NUM_KEYS];
	set_default_key_map(key_map);

	screen_def sd;
	init_screen(&sd, 1024, 768, 32);
	print_screen_def(&sd);

	GLuint shader = create_geom_shader_program(
		"/Users/dmk/code/pectin/shaders/sprite_vert.glsl",
		"/Users/dmk/code/pectin/shaders/sprite_geom.glsl",
		"/Users/dmk/code/pectin/shaders/sprite_frag_simple.glsl",
		"/Users/dmk/code/gl-sandbox/images/sprites_diff.png",
	  8, 8,
		(GLfloat *)&sd.vp_mat
	);
	render_buf *rb = create_render_buf(1000, shader);
	sprite *s = (sprite *)malloc(sizeof(sprite));
	init_render_environment();
	while (loop) {
		get_input(kdown, kpress, key_map);
		if (kpress[KEY_QUIT]) {
			loop = false;
		}
		glClear(GL_COLOR_BUFFER_BIT);
		for (int i=0; i<500; i++) {
			random_sprite(s);
			render_sprite(rb, s);
		}
		render_sprites(rb);
		render_advance(rb);
		swap_window();
	}
}

int main(int argc, char *argv[])
{
	if (!init_window("test sdl_ogl", 1024, 768)) {
		return -1;
	}
	print_sdl_gl_attributes();

	run();

	cleanup_window();

	return 0;
}


