#include <stdlib.h>
#include "example_pingpong.h"
#include "render_util.h"
#include "game_util.h"
#include "example_common.h"
#include "misc_util.h"
#include "window.h"

GLfloat full_screen_quad[] = {
	0.0f, 1.0f, 0.0f,
	1.0f, 1.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f
};

void show_err(const char *prefix) {
	GLenum err = glGetError();
	if (err != GL_NO_ERROR) {
		printf("%s: %d\n", prefix, err);
	}
}

void run_pingpong() {
	screen_w = 512;
	screen_h = 512;
	if (!init_window("test sdl_ogl", screen_w, screen_h)) return;
	print_sdl_gl_attributes();
	//screen_def sd;
	//init_screen(&sd, SCREEN_W, SCREEN_W, SCREEN_W, true);
	//print_screen_def(&sd);

	GLuint point_shader = create_shader_program(
		"/Users/dmk/code/pectin/shaders/pp_vert.glsl",
		"/Users/dmk/code/pectin/shaders/pp_pt_frag.glsl",
	false);
	show_err("point shader");
	GLuint loop_shader = create_shader_program(
		"/Users/dmk/code/pectin/shaders/pp_vert.glsl",
		"/Users/dmk/code/pectin/shaders/pp_lp_frag.glsl",
		false);
	show_err("loop shader");
	GLuint dist_shader = create_shader_program(
		"/Users/dmk/code/pectin/shaders/pp_vert.glsl",
		"/Users/dmk/code/pectin/shaders/pp_ds_frag.glsl",
		false);
	show_err("dist shader");

	int array_size = NUM_POINTS * 3 * sizeof(GLfloat);
	GLfloat *pt_buf = (GLfloat *)malloc(array_size);
	GLfloat *mv_buf = (GLfloat *)malloc(NUM_POINTS * 2 * sizeof(GLfloat));
	for (int i=0; i<NUM_POINTS; i++) {
		int idx = (i*3);
		pt_buf[idx] = rand_float();
		pt_buf[idx+1] = rand_float();
		pt_buf[idx+2] = 0.0f;
		int mdx = (i*2);
		mv_buf[mdx] = (rand_float() * 0.01f) - 0.005f;
		mv_buf[mdx+1] = (rand_float() * 0.01f) - 0.005f;
		//printf("x=%f, y=%f, z=%f\n",pt_buf[idx], pt_buf[idx+1], pt_buf[idx+2]);
	}

	vec3_t cpos = {0.5f, 0.5f, 10.0f};
	vec3_t cat = {0.5f, 0.5f, 0.0f};
	vec3_t up = {0.0f, 1.0f, 0.0f};
	mat4_t p_mat = m4_ortho(-0.5f, 0.5f, -0.5f, 0.5f, 20.0f, 1.0f);
	mat4_t v_mat = m4_look_at(cpos, cat, up);
	mat4_t m_mat = m4_identity();
	mat4_t vp_mat = m4_mul(m4_mul(p_mat, v_mat), m_mat);

	GLuint vao;
	GLuint vbo;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, array_size, pt_buf, GL_STATIC_DRAW);
	show_err("point shader buffer");

	glUseProgram(point_shader);
	GLint posAttrib = glGetAttribLocation(point_shader, "position");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	show_err("point shader position");
	GLint viewProjUnif = glGetUniformLocation(point_shader, "vp");
	glUniformMatrix4fv(viewProjUnif, 1, GL_FALSE, (GLfloat  *)&vp_mat);
	show_err("point shader vp");

	GLuint qao;
	GLuint qbo;
	glGenVertexArrays(1, &qao);
	glGenBuffers(1, &qbo);
	glBindVertexArray(qao);
	glBindBuffer(GL_ARRAY_BUFFER, qbo);
	glBufferData(GL_ARRAY_BUFFER, 6 * 3 * sizeof(GLfloat), full_screen_quad, GL_STATIC_DRAW);
	show_err("loop shader buffer");

	GLuint ppFb[2];
	GLuint ppTex[2];
	glGenFramebuffers(2, ppFb);
	glGenTextures(2, ppTex);
	for (GLuint i=0; i<2; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, ppFb[i]);
		glBindTexture(GL_TEXTURE_2D, ppTex[i]);
		glTexImage2D(
			GL_TEXTURE_2D, 0, GL_RGB16F, screen_w, screen_w, 0, GL_RGB, GL_FLOAT, NULL
		);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(
			GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ppTex[i], 0
		);
	}

	glUseProgram(loop_shader);
	GLint lposAttrib = glGetAttribLocation(loop_shader, "position");
	glEnableVertexAttribArray(lposAttrib);
	glVertexAttribPointer(lposAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	show_err("loop shader position");
	GLint lviewProjUnif = glGetUniformLocation(loop_shader, "vp");
	glUniformMatrix4fv(lviewProjUnif, 1, GL_FALSE, (GLfloat  *)&vp_mat);
	show_err("loop shader vp");
	GLint lTexUnif = glGetUniformLocation(loop_shader, "tex");
	glUniform1i(lTexUnif, 0);
	show_err("loop shader tex");
	GLint lStepUnif = glGetUniformLocation(loop_shader, "step");
	show_err("loop shader step");

	glUseProgram(dist_shader);
	GLint dposAttrib = glGetAttribLocation(dist_shader, "position");
	glEnableVertexAttribArray(dposAttrib);
	glVertexAttribPointer(dposAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	show_err("dist shader position");
	GLint dviewProjUnif = glGetUniformLocation(dist_shader, "vp");
	glUniformMatrix4fv(dviewProjUnif, 1, GL_FALSE, (GLfloat  *)&vp_mat);
	show_err("dist shader vp");
	GLint dTexUnif = glGetUniformLocation(dist_shader, "tex");
	glUniform1i(dTexUnif, 0);
	show_err("dist shader tex");

	int cur_fb = 0;
	show_err("loop shader tex");

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
		for (int i=0; i<NUM_POINTS; i++) {
			int mdx = (i*2);
			int idx = (i*3);
			GLfloat nx = pt_buf[idx] + mv_buf[mdx];
			GLfloat ny = pt_buf[idx+1] + mv_buf[mdx+1];
			if (nx > 1.0f) {
				nx = nx - 1.0f;
			} else if (nx < 0.0f) {
				nx = 1.0f + nx;
			}
			if (ny > 1.0f) {
				ny = ny - 1.0f;
			} else if (ny < 0.0f) {
				ny = 1.0f + ny;
			}
			pt_buf[idx] = nx;
			pt_buf[idx+1] = ny;
		}
		glBindFramebuffer(GL_FRAMEBUFFER, ppFb[cur_fb]);
		glClear(GL_COLOR_BUFFER_BIT);
		glBindVertexArray(vao);
		glUseProgram(point_shader);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, array_size, pt_buf, GL_STATIC_DRAW);
		glDrawArrays(GL_POINTS, 0, NUM_POINTS);

		glUseProgram(loop_shader);
		glBindVertexArray(qao);
		glBindBuffer(GL_ARRAY_BUFFER, qbo);
		GLfloat step = 1.0f;
		for (int i=0; i<10; i++) {
			glUniform1f(lStepUnif, step);
			glBindFramebuffer(GL_FRAMEBUFFER, ppFb[(cur_fb + 1) % 2]);
			glClear(GL_COLOR_BUFFER_BIT);
			glBindTexture(GL_TEXTURE_2D, ppTex[cur_fb]);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			cur_fb = (cur_fb + 1) % 2;
			step = step / 2.0f;
		}
		glUseProgram(dist_shader);
		glBindVertexArray(qao);
		glBindBuffer(GL_ARRAY_BUFFER, qbo);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT);
		glBindTexture(GL_TEXTURE_2D, ppTex[cur_fb]);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		swap_window();
	}
	free(pt_buf);
}