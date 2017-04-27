#include "render_util.h"
#include "util.h"

GLint load_texture_to_uniform(const char *filename, const char *unif_name, GLuint shaderProgram, GLuint *tex, GLenum tex_num, GLint tex_idx) {
	int tw,th,tn;
	unsigned char *image_data = stbi_load(filename, &tw, &th, &tn, 0);
	printf("image %s is %d x %d with %d components\n",filename, tw, th, tn);
	GLint texUnif;
	glGenTextures(1, tex);
	glActiveTexture(tex_num);
	glBindTexture(GL_TEXTURE_2D, *tex);
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

GLuint create_geom_shader_program(const char *vert_file_name, const char *geom_file_name, const char *frag_file_name) {
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
	return shaderProgram;
}

void free_geom_shader_program(GLuint shader_program) {
	glDeleteProgram(shader_program);
}

render_buf *create_render_buf(int num_points) {
	render_buf *rb = (render_buf *)malloc(sizeof(render_buf));
	rb->num_points = num_points;
	rb->array_size = num_points * 3 * sizeof(GLfloat);
	rb->points = (GLfloat *)malloc((size_t)rb->array_size);
	rb->colors = (GLfloat *)malloc((size_t)rb->array_size);
	rb->norm_scale_rots = (GLfloat *)malloc((size_t)rb->array_size);
	rb->scale_rots = (GLfloat *)malloc((size_t)rb->array_size);
	rb->sprite_locs = (GLfloat *)malloc((size_t)rb->array_size);
	glGenVertexArrays(1, &rb->vao);
	glGenBuffers(1, &rb->vert_bo);
	glGenBuffers(1, &rb->color_bo);
	glGenBuffers(1, &rb->scale_rot_bo);
	glGenBuffers(1, &rb->sprite_loc_bo);

	GLenum err;
	glBindVertexArray(rb->vao);
	// the VBO of points (locations of sprites)
	glBindBuffer(GL_ARRAY_BUFFER, rb->vert_bo);
	glBufferData(GL_ARRAY_BUFFER, rb->array_size, rb->points, GL_STATIC_DRAW);
	err = glGetError();
	if (err != GL_NO_ERROR) {
		printf("points buffer data: %d\n", err);
	}

	// the VBO of colors
	glBindBuffer(GL_ARRAY_BUFFER, rb->color_bo);
	glBufferData(GL_ARRAY_BUFFER, rb->array_size, rb->colors, GL_STATIC_DRAW);
	err = glGetError();
	if (err != GL_NO_ERROR) {
		printf("colors buffer data: %d\n", err);
	}

	// the VBO of x/y scale and rotation
	glBindBuffer(GL_ARRAY_BUFFER, rb->scale_rot_bo);
	glBufferData(GL_ARRAY_BUFFER, rb->array_size, rb->scale_rots, GL_STATIC_DRAW);
	err = glGetError();
	if (err != GL_NO_ERROR) {
		printf("scale/rot buffer data: %d\n", err);
	}

	// the VBO of sprite row and columns
	glBindBuffer(GL_ARRAY_BUFFER, rb->sprite_loc_bo);
	glBufferData(GL_ARRAY_BUFFER, rb->array_size, rb->sprite_locs, GL_STATIC_DRAW);
	err = glGetError();
	if (err != GL_NO_ERROR) {
		printf("sprite location buffer data: %d\n", err);
	}

	return rb;
}

void bind_render_buf(render_buf *rb, GLuint shader_program) {
	GLenum err;
	glBindVertexArray(rb->vao);
	// map the position data to an input on the shader
	glBindBuffer(GL_ARRAY_BUFFER, rb->vert_bo);
	GLuint posAttrib = (GLuint)glGetAttribLocation(shader_program, "position");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	err = glGetError();
	if (err != GL_NO_ERROR) {
		printf("posAttrib is %d: %d\n", posAttrib, err);
	}

	// map the color data to an input on the shader
	glBindBuffer(GL_ARRAY_BUFFER, rb->color_bo);
	GLuint colorAttrib = (GLuint)glGetAttribLocation(shader_program, "color");
	glEnableVertexAttribArray(colorAttrib);
	glVertexAttribPointer(colorAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	err = glGetError();
	if (err != GL_NO_ERROR) {
		printf("colorAttrib is %d: %d\n", colorAttrib, err);
	}

	// map the scale/rotation data to an input on the shader
	glBindBuffer(GL_ARRAY_BUFFER, rb->scale_rot_bo);
	GLuint scaleRotAttrib = (GLuint)glGetAttribLocation(shader_program, "scale_rot");
	glEnableVertexAttribArray(scaleRotAttrib);
	glVertexAttribPointer(scaleRotAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	err = glGetError();
	if (err != GL_NO_ERROR) {
		printf("scaleRotAttrib is %d: %d\n", scaleRotAttrib, err);
	}

	glBindBuffer(GL_ARRAY_BUFFER, rb->sprite_loc_bo);
	GLuint sprOffsetAttrib = (GLuint)glGetAttribLocation(shader_program, "spr_offset");
	glEnableVertexAttribArray(sprOffsetAttrib);
	glVertexAttribPointer(sprOffsetAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	err = glGetError();
	if (err != GL_NO_ERROR) {
		printf("sprOffsetAttrib is %d: %d\n", sprOffsetAttrib, err);
	}
}


void free_render_buf(render_buf *rb) {
	glDeleteBuffers(1, &rb->vert_bo);
	glDeleteBuffers(1, &rb->color_bo);
	glDeleteBuffers(1, &rb->scale_rot_bo);
	glDeleteBuffers(1, &rb->sprite_loc_bo);
	glDeleteVertexArrays(1, &rb->vao);
	free(rb->points);
	free(rb->colors);
	free(rb->norm_scale_rots);
	free(rb->scale_rots);
	free(rb->sprite_locs);
}
