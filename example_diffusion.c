#include <stdlib.h>
#include "example_diffusion.h"
#include "render_util.h"
#include "game_util.h"
#include "example_common.h"
#include "window.h"

GLfloat diff_full_screen_quad[] = {
	0.0f, 1.0f, 0.0f,
	1.0f, 1.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f
};

void run_diffusion() {
	screen_w = 320;
	screen_h = 320;
	if (!init_window("test sdl_ogl", screen_w, screen_h)) return;
	print_sdl_gl_attributes();

	GLuint calc_shader = create_shader_program(
		"/Users/dmk/code/pectin/shaders/diff_vert.glsl",
		"/Users/dmk/code/pectin/shaders/diff_calc_frag.glsl",
		false);
	show_err("calc shader");
	GLuint screen_shader = create_shader_program(
		"/Users/dmk/code/pectin/shaders/diff_vert.glsl",
		"/Users/dmk/code/pectin/shaders/diff_screen_frag.glsl",
		false);
	show_err("screen shader");

	vec3_t cpos = {0.5f, 0.5f, 10.0f};
	vec3_t cat = {0.5f, 0.5f, 0.0f};
	vec3_t up = {0.0f, 1.0f, 0.0f};
	mat4_t p_mat = m4_ortho(-0.5f, 0.5f, -0.5f, 0.5f, 20.0f, 1.0f);
	mat4_t v_mat = m4_look_at(cpos, cat, up);
	mat4_t m_mat = m4_identity();
	mat4_t vp_mat = m4_mul(m4_mul(p_mat, v_mat), m_mat);

	GLuint qao;
	GLuint qbo;
	glGenVertexArrays(1, &qao);
	glGenBuffers(1, &qbo);
	glBindVertexArray(qao);
	glBindBuffer(GL_ARRAY_BUFFER, qbo);
	glBufferData(GL_ARRAY_BUFFER, 6 * 3 * sizeof(GLfloat), diff_full_screen_quad, GL_STATIC_DRAW);
	show_err("shader buffer");

	GLfloat *tex_data = malloc(screen_w * screen_h * 3 * sizeof(GLfloat));
	int td_idx = 0;
	for (int y=0; y<screen_h; y++) {
		for (int x=0; x<screen_w; x++) {
			tex_data[td_idx++] = 1.0f;
			tex_data[td_idx++] = 0.0f;
			tex_data[td_idx++] = 0.0f;
		}
	}

	int splotch_w = (screen_w / 20);
	int splotch_h = (screen_h / 20);
	int startx = (screen_w - splotch_w) / 2;
	int starty = (screen_h - splotch_h) / 2;
	for (int y=starty; y<(starty+splotch_h); y++) {
		for (int x=startx; x<(startx+splotch_w); x++) {
			int idx = ((y * screen_w) + x) * 3;
			tex_data[idx++] = 0.0f;
			tex_data[idx++] = 0.0f;
			tex_data[idx++] = 1.0f;
		}
	}

	startx = (screen_w - splotch_w) / 3;
	starty = (screen_h - splotch_h) / 3;
	for (int y=starty; y<(starty+splotch_h); y++) {
		for (int x=startx; x<(startx+splotch_w); x++) {
			int idx = ((y * screen_w) + x) * 3;
			tex_data[idx++] = 1.0f;
			tex_data[idx++] = 0.0f;
			tex_data[idx++] = 1.0f;
		}
	}

	GLuint ppFb[2];
	GLuint ppTex[2];
	glGenFramebuffers(2, ppFb);
	glGenTextures(2, ppTex);
	for (GLuint i=0; i<2; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, ppFb[i]);
		glBindTexture(GL_TEXTURE_2D, ppTex[i]);
		glTexImage2D(
			GL_TEXTURE_2D, 0, GL_RGB16F, screen_w, screen_w, 0, GL_RGB, GL_FLOAT, tex_data
		);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glFramebufferTexture2D(
			GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ppTex[i], 0
		);
	}

	float step_x = (1.0f / (float)screen_w);
	float step_y = (1.0f / (float)screen_h);
	float da = 1.0f;
	float db = 0.5f;
	// regular
	//float f = 0.037f;
	//float k = 0.06f;
	float f = 0.055f;
	float k = 0.062f;
	// mitosis
	//float f = 0.043f;
	//float k = 0.0612f;
	float lap[] = {0.05f, 0.2f, 0.05f, 0.2f, -1.0f, 0.2f, 0.05f, 0.2f, 0.05f};

	glUseProgram(calc_shader);
	GLint lposAttrib = glGetAttribLocation(calc_shader, "position");
	glEnableVertexAttribArray(lposAttrib);
	glVertexAttribPointer(lposAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	show_err("calc shader position");
	GLint lviewProjUnif = glGetUniformLocation(calc_shader, "vp");
	glUniformMatrix4fv(lviewProjUnif, 1, GL_FALSE, (GLfloat  *)&vp_mat);
	show_err("calc shader vp");
	GLint lTexUnif = glGetUniformLocation(calc_shader, "tex");
	glUniform1i(lTexUnif, 0);
	show_err("calc shader tex");
	GLint lStepXUnif = glGetUniformLocation(calc_shader, "step_x");
	show_err("calc shader step_x");
	GLint lStepYUnif = glGetUniformLocation(calc_shader, "step_y");
	show_err("calc shader step_y");
	GLint lDaUnif = glGetUniformLocation(calc_shader, "da");
	show_err("da shader da");
	GLint lDbUnif = glGetUniformLocation(calc_shader, "db");
	show_err("db shader db");
	GLint lFUnif = glGetUniformLocation(calc_shader, "f");
	show_err("f shader f");
	GLint lKUnif = glGetUniformLocation(calc_shader, "k");
	show_err("k shader k");

	glUniform1f(lStepXUnif, step_x);
	glUniform1f(lStepYUnif, step_y);
	glUniform1f(lDaUnif, da);
	glUniform1f(lDbUnif, db);
	glUniform1f(lFUnif, f);
	glUniform1f(lKUnif, k);

	glUseProgram(screen_shader);
	GLint dposAttrib = glGetAttribLocation(screen_shader, "position");
	glEnableVertexAttribArray(dposAttrib);
	glVertexAttribPointer(dposAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	show_err("screen shader position");
	GLint dviewProjUnif = glGetUniformLocation(screen_shader, "vp");
	glUniformMatrix4fv(dviewProjUnif, 1, GL_FALSE, (GLfloat  *)&vp_mat);
	show_err("screen shader vp");
	GLint dTexUnif = glGetUniformLocation(screen_shader, "tex");
	glUniform1i(dTexUnif, 0);
	show_err("screen shader tex");

	int cur_fb = 0;

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glViewport(0, 0, screen_w, screen_w);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	bool loop = true;
	bool kdown[NUM_KEYS];
	bool kpress[NUM_KEYS];
	for (int i=0; i<NUM_KEYS; i++) {
		kdown[i] = false;
		kpress[i] = false;
	}
	int key_map[NUM_KEYS];
	set_default_key_map(key_map);
	mouse_input mouse;

	while (loop) {
		get_input(kdown, kpress, key_map, &mouse);
		if (kpress[KEY_QUIT]) {
			loop = false;
		}

		glUseProgram(calc_shader);
		glBindVertexArray(qao);
		glBindBuffer(GL_ARRAY_BUFFER, qbo);
		for (int i=0; i<60; i++) {
			glBindFramebuffer(GL_FRAMEBUFFER, ppFb[(cur_fb + 1) % 2]);
			glClear(GL_COLOR_BUFFER_BIT);
			glBindTexture(GL_TEXTURE_2D, ppTex[cur_fb]);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			cur_fb = (cur_fb + 1) % 2;
		}

		glUseProgram(screen_shader);
		glBindVertexArray(qao);
		glBindBuffer(GL_ARRAY_BUFFER, qbo);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT);
		glBindTexture(GL_TEXTURE_2D, ppTex[cur_fb]);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		swap_window();
	}
	free(tex_data);
}