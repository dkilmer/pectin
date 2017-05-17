#include "render_util.h"
#include "misc_util.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


GLint load_texture_to_uniform(const char *filename, const char *unif_name, GLuint shaderProgram, GLuint *tex, GLenum tex_num, GLint tex_idx) {
	int tw,th,tn;
	unsigned char *image_data = stbi_load(filename, &tw, &th, &tn, 0);
	printf("image %s is %d x %d with %d components\n",filename, tw, th, tn);
	GLint texUnif;
	glGenTextures(1, tex);
	glActiveTexture(tex_num);
	glBindTexture(GL_TEXTURE_2D, *tex);
	printf("TEXTURE ID is %d, tex_num is %d, tex_idx is %d\n", *tex, tex_num, tex_idx);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tw, th, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(image_data);
	texUnif = glGetUniformLocation(shaderProgram, unif_name);
	glUniform1i(texUnif, tex_idx);
	return texUnif;
}

GLuint create_geom_shader_program(const char *vert_file_name, const char *geom_file_name, const char *frag_file_name, const char *tex_file_name, int tex_w, int tex_h, GLfloat *vp_mat, GLuint *tex_id) {
	const GLchar* vertex_shader = load_file(vert_file_name);
	const GLchar* geom_shader = load_file(geom_file_name);
	const GLchar* fragment_shader = load_file(frag_file_name);

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertex_shader, NULL);
	glCompileShader(vertexShader);
	GLint status;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE) {
		char buffer[512];
		glGetShaderInfoLog(vertexShader, 512, NULL, buffer);
		printf("ERROR compiling the vertex shader: %s\n",buffer);
	}

	GLuint geomShader = glCreateShader(GL_GEOMETRY_SHADER);
	glShaderSource(geomShader, 1, &geom_shader, NULL);
	glCompileShader(geomShader);
	glGetShaderiv(geomShader, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE) {
		char buffer[512];
		glGetShaderInfoLog(geomShader, 512, NULL, buffer);
		printf("ERROR compiling the vertex shader: %s\n",buffer);
	}

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragment_shader, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE) {
		char buffer[512];
		glGetShaderInfoLog(fragmentShader, 512, NULL, buffer);
		printf("ERROR compiling the fragment shader: %s\n",buffer);
	}

	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, geomShader);
	glAttachShader(shaderProgram, fragmentShader);
	glBindFragDataLocation(shaderProgram, 0, "outColor");
	glLinkProgram(shaderProgram);

	glUseProgram(shaderProgram);
	GLint viewProjUnif = glGetUniformLocation(shaderProgram, "vp");
	glUniformMatrix4fv(viewProjUnif, 1, GL_FALSE, vp_mat);
	GLint texMultUnif = glGetUniformLocation(shaderProgram, "tex_mult");
	float mx = 1.0f / (float)tex_w;
	float my = 1.0f / (float)tex_h;
	glUniform2f(texMultUnif, mx, my);
	GLuint tex;
	GLint texUnif = load_texture_to_uniform(tex_file_name, "tex", shaderProgram, &tex, GL_TEXTURE0, 0);
	*tex_id = tex;

	return shaderProgram;
}

void update_view_mat(render_buf *rb, GLfloat *mat) {
	glUseProgram(rb->shader);
	GLint viewProjUnif = glGetUniformLocation(rb->shader, "vp");
	glUniformMatrix4fv(viewProjUnif, 1, GL_FALSE, mat);
}

void free_geom_shader_program(GLuint shader_program) {
	glDeleteProgram(shader_program);
}

void init_render_environment() {
	glClearColor(0.72f, 0.72f, 0.72f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_FRONT);
	glDisable(GL_CLIP_PLANE0);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

render_buf *create_render_buf(int max_items, GLuint shader_program, GLuint tex_id) {
	render_buf *rb = (render_buf *)malloc(sizeof(render_buf));
	rb->num_items = max_items;
	rb->array_size = rb->num_items * 12 * sizeof(GLfloat);
	rb->num_bufs = 3;
	rb->buf_idx = 0;
	rb->spr_idx = 0;
	rb->shader = shader_program;
	rb->tex_id = tex_id;
	rb->fences = (GLsync *)malloc(rb->num_bufs * sizeof(GLsync));
	for (int i=0; i<rb->num_bufs; i++) rb->fences[i] = NULL;
	//rb->buf = (GLfloat *)malloc((size_t)rb->array_size);
	glGenVertexArrays(1, &rb->vao);
	glGenBuffers(1, &rb->vbo);

	GLenum err;
	glBindVertexArray(rb->vao);
	// the VBO of points (locations of sprites)
	glBindBuffer(GL_ARRAY_BUFFER, rb->vbo);

	GLuint posAttrib = (GLuint)glGetAttribLocation(shader_program, "position");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(GLfloat), 0);
	err = glGetError();
	if (err != GL_NO_ERROR) {
		printf("posAttrib is %d: %d\n", posAttrib, err);
	}

	// map the color data to an input on the shader
	GLuint colorAttrib = (GLuint)glGetAttribLocation(shader_program, "color");
	glEnableVertexAttribArray(colorAttrib);
	glVertexAttribPointer(colorAttrib, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(GLfloat), (void *)(3 * sizeof(GLfloat)));
	err = glGetError();
	if (err != GL_NO_ERROR) {
		printf("colorAttrib is %d: %d\n", colorAttrib, err);
	}

	// map the scale/rotation data to an input on the shader
	GLuint scaleRotAttrib = (GLuint)glGetAttribLocation(shader_program, "scale_rot");
	glEnableVertexAttribArray(scaleRotAttrib);
	glVertexAttribPointer(scaleRotAttrib, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(GLfloat), (void *)(6 * sizeof(GLfloat)));
	err = glGetError();
	if (err != GL_NO_ERROR) {
		printf("scaleRotAttrib is %d: %d\n", scaleRotAttrib, err);
	}

	// map the sprite location data to an input on the shader
	GLuint sprOffsetAttrib = (GLuint)glGetAttribLocation(shader_program, "spr_offset");
	glEnableVertexAttribArray(sprOffsetAttrib);
	glVertexAttribPointer(sprOffsetAttrib, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(GLfloat), (void *)(9 * sizeof(GLfloat)));
	err = glGetError();
	if (err != GL_NO_ERROR) {
		printf("sprOffsetAttrib is %d: %d\n", sprOffsetAttrib, err);
	}

	glBufferData(GL_ARRAY_BUFFER, rb->array_size * rb->num_bufs, NULL, GL_STREAM_DRAW);
	err = glGetError();
	if (err != GL_NO_ERROR) {
		printf("buffer data: %d\n", err);
	}

	return rb;
}

void free_render_buf(render_buf *rb) {
	glDeleteBuffers(1, &rb->vbo);
	glDeleteVertexArrays(1, &rb->vao);
	free(rb->fences);
	//free(rb->buf);
}

void render_sprite(render_buf *rb, sprite *s) {
	// just starting a buffer. we need to wait and map on that shit
	if (rb->spr_idx == 0) {
		glBindVertexArray(rb->vao);
		if (rb->fences[rb->buf_idx] != NULL) {
			GLenum state = glClientWaitSync(rb->fences[rb->buf_idx], GL_SYNC_FLUSH_COMMANDS_BIT, 1000000000);
			if (state == GL_TIMEOUT_EXPIRED || state == GL_WAIT_FAILED) {
				printf("wait for fence on buf_idx %d failed with error %d\n", rb->buf_idx, state);
			}
			glDeleteSync(rb->fences[rb->buf_idx]);
			rb->fences[rb->buf_idx] = NULL;
		}
		glBindBuffer(GL_ARRAY_BUFFER, rb->vbo);
		int map_start = rb->buf_idx * rb->num_items * 12 * sizeof(GLfloat);
		int buf_len = rb->num_items * 12 * sizeof(GLfloat);
		rb->buf = (GLfloat *)glMapBufferRange(GL_ARRAY_BUFFER, map_start, buf_len, GL_MAP_WRITE_BIT|GL_MAP_UNSYNCHRONIZED_BIT);
		if (rb->buf == NULL) printf("failed to map buffer for buf_idx %d\n", rb->buf_idx);
	}
	if (rb->spr_idx == rb->num_items) {
		printf("can't render to buf_idx %d: overflow\n", rb->buf_idx);
		return;
	}
	int idx = rb->spr_idx * 12;
	rb->buf[idx++] = s->x;
	rb->buf[idx++] = s->y;
	rb->buf[idx++] = s->z;
	rb->buf[idx++] = s->r;
	rb->buf[idx++] = s->g;
	rb->buf[idx++] = s->b;
	rb->buf[idx++] = s->scale_x;
	rb->buf[idx++] = s->scale_y;
	rb->buf[idx++] = s->rot;
	rb->buf[idx++] = s->spr_row;
	rb->buf[idx++] = s->spr_col;
	rb->buf[idx] = s->spr_extra;
	rb->spr_idx++;
}

void render_sprites(render_buf *rb) {
	//glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, rb->tex_id);
	glBindVertexArray(rb->vao);
	glBindBuffer(GL_ARRAY_BUFFER, rb->vbo);
	glUseProgram(rb->shader);
	if (rb->spr_idx > 0) {
		glDrawArrays(GL_POINTS, rb->buf_idx * rb->num_items, rb->spr_idx*4);
	}
}

void render_advance(render_buf *rb) {
	glBindVertexArray(rb->vao);
	glBindBuffer(GL_ARRAY_BUFFER, rb->vbo);
	glUnmapBuffer(GL_ARRAY_BUFFER);
	rb->fences[rb->buf_idx] = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	rb->buf_idx = ((rb->buf_idx + 1) % rb->num_bufs);
	rb->spr_idx = 0;
};

