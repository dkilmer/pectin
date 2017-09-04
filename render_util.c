#include "render_util.h"
#include "misc_util.h"
#include "ini.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


static int rdef_handler(void *uobj, const char* section, const char* name, const char* value) {
	//printf("loading section %s, name %s, value %s\n", section, name, value);
	render_def *rd = (render_def *)uobj;
	#define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
	if (MATCH(rd->section_name, "type")) {
		rd->shader_type = strdup(value);
	} else if (MATCH(rd->section_name, "vert_shader")) {
		rd->vert_shader = strdup(value);
	} else if (MATCH(rd->section_name, "geom_shader")) {
		rd->geom_shader = strdup(value);
	} else if (MATCH(rd->section_name, "frag_shader")) {
		rd->frag_shader = strdup(value);
	} else if (MATCH(rd->section_name, "depth_vert_shader")) {
		rd->depth_vert_shader = strdup(value);
	} else if (MATCH(rd->section_name, "depth_geom_shader")) {
		rd->depth_geom_shader = strdup(value);
	} else if (MATCH(rd->section_name, "depth_frag_shader")) {
		rd->depth_frag_shader = strdup(value);
	} else if (MATCH(rd->section_name, "sprite_sheet")) {
		rd->sprite_sheet = strdup(value);
	} else if (MATCH(rd->section_name, "cols")) {
		rd->cols = atoi(value);
	} else if (MATCH(rd->section_name, "rows")) {
		rd->rows = atoi(value);
	} else if (MATCH(rd->section_name, "x_scale")) {
		rd->xscale = (float)atof(value);
	} else if (MATCH(rd->section_name, "y_scale")) {
		rd->yscale = (float)atof(value);
	} else if (MATCH(rd->section_name, "line_scale")) {
		rd->line_scale = (float)atof(value);
	} else if (MATCH(rd->section_name, "x_offset")) {
		rd->xoff = (float)atof(value);
	} else if (MATCH(rd->section_name, "y_offset")) {
		rd->yoff = (float)atof(value);
	} else if (MATCH(rd->section_name, "layer_x_offset")) {
		rd->layer_xoff = (float)atof(value);
	} else if (MATCH(rd->section_name, "layer_y_offset")) {
		rd->layer_yoff = (float)atof(value);
	} else if (MATCH(rd->section_name, "render_buf_size")) {
		rd->num_items = atoi(value);
	} else if (MATCH(rd->section_name, "viewport_w")) {
		rd->viewport_w = atoi(value);
	} else if (MATCH(rd->section_name, "viewport_h")) {
		rd->viewport_h = atoi(value);
	} else if (MATCH(rd->section_name, "depth_viewport_w")) {
		rd->depth_viewport_w = atoi(value);
	} else if (MATCH(rd->section_name, "depth_viewport_h")) {
		rd->depth_viewport_h = atoi(value);
	}
	return 1;
}

render_def *load_render_def(const char *filename, const char *section, GLfloat *vp_matrix,
                            GLfloat *depth_vp_matrix, GLfloat *depth_bias_matrix) {
	render_def *rd = (render_def *)malloc(sizeof(render_def));
	rd->section_name = strdup(section);
	rd->shader = 0;
	rd->depth_shader = 0;
	rd->num_items = 0;
	rd->use_depth = false;
	if (ini_parse(filename, rdef_handler, rd) < 0) {
		printf("Can't load '%s'\n", filename);
		return NULL;
	}

	// allocate memory for and copy over transformation matrices
	rd->vp_mat = (GLfloat *)malloc(sizeof(GLfloat) * 16);
	memcpy(rd->vp_mat, vp_matrix, 16*sizeof(GLfloat));
	if (depth_vp_matrix != NULL) {
		rd->depth_vp_mat = (GLfloat *)malloc(sizeof(GLfloat) * 16);
		memcpy(rd->depth_vp_mat, depth_vp_matrix, 16*sizeof(GLfloat));
	} else {
		rd->depth_vp_mat = NULL;
	}
	if (depth_bias_matrix != NULL) {
		rd->depth_bias_mat = (GLfloat *)malloc(sizeof(GLfloat) * 16);
		memcpy(rd->depth_bias_mat, depth_bias_matrix, 16*sizeof(GLfloat));
	} else {
		rd->depth_bias_mat = NULL;
	}

	// create the shaders and buffers depending on the type
	if (strcmp(rd->shader_type, "sprite") == 0) {
		create_sprite_shader_program(rd);
	} else if (strcmp(rd->shader_type, "line") == 0) {
		create_line_shader_program(rd);
	} else if (strcmp(rd->shader_type, "vector") == 0) {
		create_vector_shader_program(rd);
	} else if (strcmp(rd->shader_type, "vbox") == 0) {
		create_vbox_shader_program(rd);
	} else if (strcmp(rd->shader_type, "rect") == 0) {
		create_rect_shader_program(rd);
	} else if (strcmp(rd->shader_type, "depth") == 0) {
		create_depth_shader_program(rd);
	} else {
		printf("ERROR - Invalid shader type %s\n", rd->shader_type);
	}
	return rd;
}

GLuint create_depth_map(int w, int h) {
	GLuint depth_map;
	glGenTextures(1, &depth_map);
	glBindTexture(GL_TEXTURE_CUBE_MAP, depth_map);
	for (unsigned int i = 0; i < 6; ++i) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
		             w, h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		GLenum err = glGetError();
		if (err != GL_NO_ERROR) {
			printf("-=-= ERROR bind framebuffer: %d\n", err);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_FUNC, GL_EQUAL);
	//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	GLenum err = glGetError();
	if (err != GL_NO_ERROR) {
		printf("-=-= ERROR bind framebuffer: %d\n", err);
	}
	/*
	glBindTexture(GL_TEXTURE_2D, depth_map);
	glTexImage2D(GL_TEXTURE_2D, 0,GL_DEPTH_COMPONENT16, w, h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	*/
	/*
	glBindTexture(GL_TEXTURE_2D, depth_map);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
	             w, h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LESS);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	*/
	return depth_map;
}

GLuint create_depth_buffer(int w, int h, GLuint *depth_map) {
	GLuint depth_framebuffer = 0;
	glGenFramebuffers(1, &depth_framebuffer);
	*depth_map = create_depth_map(w, h);

	glBindFramebuffer(GL_FRAMEBUFFER, depth_framebuffer);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D ,*depth_map, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, *depth_map, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		printf("-=-= ERROR: creating depth framebuffer failed\n");
	}
	GLenum err = glGetError();
	if (err != GL_NO_ERROR) {
		printf("-=-= ERROR bind framebuffer: %d\n", err);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return depth_framebuffer;
}


GLint load_texture_to_uniform(const char *filename, const char *unif_name, GLuint shaderProgram, GLuint *tex, GLenum tex_num, GLint tex_idx) {
	GLenum err;
	//glUseProgram(shaderProgram);
	int tw,th,tn;
	unsigned char *image_data = stbi_load(filename, &tw, &th, &tn, 0);
	printf("image %s is %d x %d with %d components\n",filename, tw, th, tn);
	glGenTextures(1, tex);
	err = glGetError();
	if (err != GL_NO_ERROR) {
		printf("-=-= ERROR 1: %d\n", err);
	}

	glActiveTexture(tex_num);
	err = glGetError();
	if (err != GL_NO_ERROR) {
		printf("-=-= ERROR 2: %d\n", err);
	}
	glBindTexture(GL_TEXTURE_2D, *tex);
	err = glGetError();
	if (err != GL_NO_ERROR) {
		printf("-=-= ERROR 3: %d\n", err);
	}

	printf("TEXTURE ID is %d, tex_num is %d, tex_idx is %d\n", *tex, tex_num, tex_idx);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tw, th, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
	err = glGetError();
	if (err != GL_NO_ERROR) {
		printf("-=-= ERROR 4: %d\n", err);
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	err = glGetError();
	if (err != GL_NO_ERROR) {
		printf("-=-= ERROR 5.1: %d\n", err);
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	err = glGetError();
	if (err != GL_NO_ERROR) {
		printf("-=-= ERROR 5.2: %d\n", err);
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	err = glGetError();
	if (err != GL_NO_ERROR) {
		printf("-=-= ERROR 5.2: %d\n", err);
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	err = glGetError();
	if (err != GL_NO_ERROR) {
		printf("-=-= ERROR 5.3: %d\n", err);
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	err = glGetError();
	if (err != GL_NO_ERROR) {
		printf("-=-= ERROR 5.4: %d\n", err);
	}
	glGenerateMipmap(GL_TEXTURE_2D);
	err = glGetError();
	if (err != GL_NO_ERROR) {
		printf("-=-= ERROR 5.5: %d\n", err);
	}
	stbi_image_free(image_data);
	err = glGetError();
	if (err != GL_NO_ERROR) {
		printf("-=-= ERROR 6: %d\n", err);
	}

	GLint texUnif;
	texUnif = glGetUniformLocation(shaderProgram, unif_name);
	glUniform1i(texUnif, tex_idx);
	err = glGetError();
	if (err != GL_NO_ERROR) {
		printf("-=-= ERROR 7: %d\n", err);
	}
	return texUnif;
}

GLint bind_texture_to_uniform(const char *unif_name, GLuint shaderProgram, GLuint *tex, GLenum tex_num, GLint tex_idx) {
	//glUseProgram(shaderProgram);
	GLint texUnif;
	glActiveTexture(tex_num);
	glBindTexture(GL_TEXTURE_2D, *tex);
	texUnif = glGetUniformLocation(shaderProgram, unif_name);
	glUniform1i(texUnif, tex_idx);
	return texUnif;
}

GLint bind_depth_cube_texture_to_uniform(const char *unif_name, GLuint shaderProgram, GLuint *tex, GLenum tex_num, GLint tex_idx) {
	//glUseProgram(shaderProgram);
	GLint texUnif;
	glActiveTexture(tex_num);
	glBindTexture(GL_TEXTURE_CUBE_MAP, *tex);
	texUnif = glGetUniformLocation(shaderProgram, unif_name);
	glUniform1i(texUnif, tex_idx);
	return texUnif;
}

GLuint create_shader_program(const char *vert_file_name, const char *frag_file_name, bool depth) {
	const GLchar* vertex_shader = load_file(vert_file_name);
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
	glAttachShader(shaderProgram, fragmentShader);
	if (!depth) glBindFragDataLocation(shaderProgram, 0, "outColor");
	glLinkProgram(shaderProgram);

	GLint isLinked = 0;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &isLinked);
	if(isLinked == GL_FALSE) {
		printf("ERROR: shader program failed to link\n");
		GLint maxLength = 0;
		glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &maxLength);
		char buffer[maxLength];
		glGetProgramInfoLog(shaderProgram, maxLength, &maxLength, buffer);
		printf("%s\n", buffer);
	}

	return shaderProgram;
}


GLuint create_geom_shader_program(const char *vert_file_name, const char *geom_file_name, const char *frag_file_name, bool depth) {
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
		printf("ERROR compiling the geometry shader: %s\n",buffer);
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
	if (!depth) glBindFragDataLocation(shaderProgram, 0, "outColor");
	glLinkProgram(shaderProgram);

	GLint isLinked = 0;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &isLinked);
	if(isLinked == GL_FALSE) {
		printf("ERROR: shader program failed to link\n");
		GLint maxLength = 0;
		glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &maxLength);
		char buffer[maxLength];
		glGetProgramInfoLog(shaderProgram, maxLength, &maxLength, buffer);
		printf("%s\n", buffer);
	}

	return shaderProgram;
}

void create_depth_shader_program(render_def *rd) {
	rd->use_depth = true;
	rd->depth_framebuffer = create_depth_buffer(rd->depth_viewport_w, rd->depth_viewport_h, &rd->depth_tex);

	glBindFramebuffer(GL_FRAMEBUFFER, rd->depth_framebuffer);
	rd->depth_shader = create_geom_shader_program(rd->depth_vert_shader, rd->depth_geom_shader, rd->depth_frag_shader, true);
	//glUseProgram(rd->depth_shader);
	//GLint depthViewProjUnif = glGetUniformLocation(rd->depth_shader, "depth_vp");
	//printf("depthViewProjUnif in depth shader is %d\n", depthViewProjUnif);
	//glUniformMatrix4fv(depthViewProjUnif, 1, GL_FALSE, rd->depth_vp_mat);
	//rd->d_depth_vp_unif = depthViewProjUnif;
	//GLint depthTexMultUnif = glGetUniformLocation(rd->depth_shader, "tex_mult");
	//float dmx = 1.0f / (float)rd->cols;
	//float dmy = 1.0f / (float)rd->rows;
	//glUniform2f(depthTexMultUnif, dmx, dmy);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	rd->shader = create_geom_shader_program(rd->vert_shader, rd->geom_shader, rd->frag_shader, false);
	glUseProgram(rd->shader);
	GLint viewProjUnif = glGetUniformLocation(rd->shader, "vp");
	glUniformMatrix4fv(viewProjUnif, 1, GL_FALSE, rd->vp_mat);
	rd->vp_unif = viewProjUnif;
	//GLint depthBiasUnif = glGetUniformLocation(rd->shader, "depth_bias_vp");
	//glUniformMatrix4fv(depthBiasUnif, 1, GL_FALSE, rd->depth_vp_mat);
	//GLint depthVpUnif = glGetUniformLocation(rd->shader, "depth_vp");
	//glUniformMatrix4fv(depthVpUnif, 1, GL_FALSE, rd->depth_vp_mat);
	//rd->depth_vp_unif = depthVpUnif;
	//printf("depthVpUnif in normal shader is %d\n", depthVpUnif);

	GLint texMultUnif = glGetUniformLocation(rd->shader, "tex_mult");
	float mx = 1.0f / (float)rd->cols;
	float my = 1.0f / (float)rd->rows;
	glUniform2f(texMultUnif, mx, my);
	rd->tex_mult_unif = texMultUnif;
	GLint texUnif = load_texture_to_uniform(rd->sprite_sheet, "tex", rd->shader, &rd->tex, GL_TEXTURE0, 0);
	GLint depthTexUnif = bind_depth_cube_texture_to_uniform("shadow_map", rd->shader, &rd->depth_tex, GL_TEXTURE1, 1);
	create_sprite_render_buf(rd);
	glBindBuffer(GL_ARRAY_BUFFER, rd->vbo);
	set_sprite_float_render_attribs(rd->depth_shader, rd->item_size);
	if (rd->uitem_size > 0) {
		glBindBuffer(GL_ARRAY_BUFFER, rd->vbou);
		set_sprite_uint_render_attribs(rd->depth_shader, rd->uitem_size);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, rd->depth_framebuffer);
	glUseProgram(rd->depth_shader);
	glBindBuffer(GL_ARRAY_BUFFER, rd->vbo);
	set_sprite_float_render_attribs(rd->depth_shader, rd->item_size);
	if (rd->uitem_size > 0) {
		glBindBuffer(GL_ARRAY_BUFFER, rd->vbou);
		set_sprite_uint_render_attribs(rd->depth_shader, rd->uitem_size);
	}
}

void copy_depth(render_def *rd_src, render_def *rd_dst) {
	rd_dst->use_depth = true;
	rd_dst->depth_viewport_w = rd_src->depth_viewport_w;
	rd_dst->depth_viewport_h = rd_src->depth_viewport_h;
	rd_dst->depth_framebuffer = rd_src->depth_framebuffer;
	glBindFramebuffer(GL_FRAMEBUFFER, rd_dst->depth_framebuffer);
	rd_dst->depth_tex = rd_src->depth_tex;
	rd_dst->depth_shader = create_geom_shader_program(rd_dst->depth_vert_shader, rd_dst->depth_geom_shader, rd_dst->depth_frag_shader, true);
	glUseProgram(rd_dst->depth_shader);
	glBindBuffer(GL_ARRAY_BUFFER, rd_dst->vbo);
	set_sprite_float_render_attribs(rd_dst->depth_shader, rd_dst->item_size);
	if (rd_dst->uitem_size > 0) {
		glBindBuffer(GL_ARRAY_BUFFER, rd_dst->vbou);
		set_sprite_uint_render_attribs(rd_dst->depth_shader, rd_dst->uitem_size);
	}
}

void create_sprite_shader_program(render_def *rd) {
	rd->shader = create_geom_shader_program(rd->vert_shader, rd->geom_shader, rd->frag_shader, false);
	glUseProgram(rd->shader);
	GLint viewProjUnif = glGetUniformLocation(rd->shader, "vp");
	glUniformMatrix4fv(viewProjUnif, 1, GL_FALSE, rd->vp_mat);
	rd->vp_unif = viewProjUnif;
	GLint texMultUnif = glGetUniformLocation(rd->shader, "tex_mult");
	float mx = 1.0f / (float)rd->cols;
	float my = 1.0f / (float)rd->rows;
	glUniform2f(texMultUnif, mx, my);
	rd->tex_mult_unif = texMultUnif;
	GLint texUnif = load_texture_to_uniform(rd->sprite_sheet, "tex", rd->shader, &rd->tex, GL_TEXTURE0, 0);
	create_sprite_render_buf(rd);
	glBindVertexArray(rd->vao);
	glBindBuffer(GL_ARRAY_BUFFER, rd->vbo);
	set_sprite_float_render_attribs(rd->shader, rd->item_size);
	if (rd->uitem_size > 0) {
		glBindBuffer(GL_ARRAY_BUFFER, rd->vbou);
		set_sprite_uint_render_attribs(rd->shader, rd->uitem_size);
		if (rd->depth_shader > 0) {
			glUseProgram(rd->depth_shader);
			set_sprite_uint_render_attribs(rd->depth_shader, rd->uitem_size);
		}
	}
}

void create_line_shader_program(render_def *rd) {
	rd->shader = create_geom_shader_program(rd->vert_shader, rd->geom_shader, rd->frag_shader, false);
	glUseProgram(rd->shader);
	GLint viewProjUnif = glGetUniformLocation(rd->shader, "vp");
	glUniformMatrix4fv(viewProjUnif, 1, GL_FALSE, rd->vp_mat);
	rd->vp_unif = viewProjUnif;
	GLint lineScaleUnif = glGetUniformLocation(rd->shader, "scale");
	glUniform1f(lineScaleUnif, rd->line_scale);
	create_line_render_buf(rd);
}

void create_vector_shader_program(render_def *rd) {
	rd->shader = create_shader_program(rd->vert_shader, rd->frag_shader, false);
	glUseProgram(rd->shader);
	GLint viewProjUnif = glGetUniformLocation(rd->shader, "vp");
	glUniformMatrix4fv(viewProjUnif, 1, GL_FALSE, rd->vp_mat);
	rd->vp_unif = viewProjUnif;
	create_line_render_buf(rd);
}

void create_vbox_shader_program(render_def *rd) {
	rd->shader = create_geom_shader_program(rd->vert_shader, rd->geom_shader ,rd->frag_shader, false);
	glUseProgram(rd->shader);
	GLint viewProjUnif = glGetUniformLocation(rd->shader, "vp");
	glUniformMatrix4fv(viewProjUnif, 1, GL_FALSE, rd->vp_mat);
	rd->vp_unif = viewProjUnif;
	create_line_render_buf(rd);
}

void create_rect_shader_program(render_def *rd) {
	rd->shader = create_shader_program(rd->vert_shader, rd->frag_shader, false);
	glUseProgram(rd->shader);
	GLint viewProjUnif = glGetUniformLocation(rd->shader, "vp");
	glUniformMatrix4fv(viewProjUnif, 1, GL_FALSE, rd->vp_mat);
	rd->vp_unif = viewProjUnif;
	create_line_render_buf(rd);
	rd->draw_type = GL_TRIANGLE_STRIP;
}

void update_view_mat(render_def *rd, GLfloat *mat) {
	glUniformMatrix4fv(rd->vp_unif, 1, GL_FALSE, mat);
}

void free_shader_program(GLuint shader_program) {
	glDeleteProgram(shader_program);
}

void init_render_environment() {
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	//glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	//glFrontFace(GL_CW);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glDisable(GL_CLIP_PLANE0);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void set_sprite_float_render_attribs(GLuint shader, int item_size) {
	GLenum err;
	GLuint posAttrib = (GLuint)glGetAttribLocation(shader, "position");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, item_size * sizeof(GLfloat), 0);
	err = glGetError();
	if (err != GL_NO_ERROR) {
		printf("posAttrib is %d: %d\n", posAttrib, err);
	}

	// map the color data to an input on the shader
	GLuint colorAttrib = (GLuint)glGetAttribLocation(shader, "color");
	glEnableVertexAttribArray(colorAttrib);
	glVertexAttribPointer(colorAttrib, 3, GL_FLOAT, GL_FALSE, item_size * sizeof(GLfloat), (void *)(3 * sizeof(GLfloat)));
	err = glGetError();
	if (err != GL_NO_ERROR) {
		printf("colorAttrib is %d: %d\n", colorAttrib, err);
	}

	// map the scale/rotation data to an input on the shader
	GLuint scaleRotAttrib = (GLuint)glGetAttribLocation(shader, "scale_rot");
	glEnableVertexAttribArray(scaleRotAttrib);
	glVertexAttribPointer(scaleRotAttrib, 3, GL_FLOAT, GL_FALSE, item_size * sizeof(GLfloat), (void *)(6 * sizeof(GLfloat)));
	err = glGetError();
	if (err != GL_NO_ERROR) {
		printf("scaleRotAttrib is %d: %d\n", scaleRotAttrib, err);
	}
}


void set_sprite_uint_render_attribs(GLuint shader, int item_size) {
	GLenum err;
	// map the sprite location data to an input on the shader
	GLuint sprOffsetAttrib = (GLuint)glGetAttribLocation(shader, "spr_offset");
	glEnableVertexAttribArray(sprOffsetAttrib);
	glVertexAttribIPointer(sprOffsetAttrib, 3, GL_UNSIGNED_INT, item_size * sizeof(GLuint), 0);
	err = glGetError();
	if (err != GL_NO_ERROR) {
		printf("sprOffsetAttrib is %d: %d\n", sprOffsetAttrib, err);
	}
}

void create_sprite_render_buf(render_def *rd) {
	rd->draw_type = GL_POINTS;
	rd->item_size = 9;
	rd->array_size = rd->num_items * rd->item_size * sizeof(GLfloat);
	rd->uitem_size = 3;
	rd->uarray_size = rd->num_items * rd->uitem_size * sizeof(GLuint);
	rd->num_bufs = 3;
	rd->buf_idx = 0;
	rd->item_idx = 0;
	rd->fences = (GLsync *)malloc(rd->num_bufs * sizeof(GLsync));
	for (int i=0; i<rd->num_bufs; i++) rd->fences[i] = NULL;
	//rb->buf = (GLfloat *)malloc((size_t)rb->array_size);
	glGenVertexArrays(1, &rd->vao);
	glGenBuffers(1, &rd->vbo);
	glGenBuffers(1, &rd->vbou);

	GLenum err;

	glBindVertexArray(rd->vao);
	// the VBO of points, colors, and scale/rotation
	glBindBuffer(GL_ARRAY_BUFFER, rd->vbo);
	//set_sprite_float_render_attribs(rd->shader, rd->item_size);
	glBufferData(GL_ARRAY_BUFFER, rd->array_size * rd->num_bufs, NULL, GL_STREAM_DRAW);
	err = glGetError();
	if (err != GL_NO_ERROR) {
		printf("float buffer data: %d\n", err);
	}

	glBindVertexArray(rd->vao);
	// the VBO of sprite information (index, cols, extra)
	glBindBuffer(GL_ARRAY_BUFFER, rd->vbou);
	//set_sprite_uint_render_attribs(rd->shader, rd->uitem_size);
	glBufferData(GL_ARRAY_BUFFER, rd->uarray_size * rd->num_bufs, NULL, GL_STREAM_DRAW);
	err = glGetError();
	if (err != GL_NO_ERROR) {
		printf("uint buffer data: %d\n", err);
	}
}

void create_line_render_buf(render_def *rd) {
	rd->draw_type = GL_LINES;
	rd->item_size = 6;
	rd->array_size = rd->num_items * rd->item_size * sizeof(GLfloat);
	rd->uitem_size = 0;
	rd->uarray_size = 0;
	rd->num_bufs = 3;
	rd->buf_idx = 0;
	rd->item_idx = 0;
	rd->fences = (GLsync *)malloc(rd->num_bufs * sizeof(GLsync));
	for (int i=0; i<rd->num_bufs; i++) rd->fences[i] = NULL;
	//rb->buf = (GLfloat *)malloc((size_t)rb->array_size);
	glGenVertexArrays(1, &rd->vao);
	glGenBuffers(1, &rd->vbo);

	GLenum err;
	glBindVertexArray(rd->vao);
	// the VBO of points (locations of sprites)
	glBindBuffer(GL_ARRAY_BUFFER, rd->vbo);

	GLuint posAttrib = (GLuint)glGetAttribLocation(rd->shader, "position");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, rd->item_size * sizeof(GLfloat), 0);
	err = glGetError();
	if (err != GL_NO_ERROR) {
		printf("posAttrib is %d: %d\n", posAttrib, err);
	}

	// map the color data to an input on the shader
	GLuint colorAttrib = (GLuint)glGetAttribLocation(rd->shader, "color");
	glEnableVertexAttribArray(colorAttrib);
	glVertexAttribPointer(colorAttrib, 3, GL_FLOAT, GL_FALSE, rd->item_size * sizeof(GLfloat), (void *)(3 * sizeof(GLfloat)));
	err = glGetError();
	if (err != GL_NO_ERROR) {
		printf("colorAttrib is %d: %d\n", colorAttrib, err);
	}

	glBufferData(GL_ARRAY_BUFFER, rd->array_size * rd->num_bufs, NULL, GL_STREAM_DRAW);
	err = glGetError();
	if (err != GL_NO_ERROR) {
		printf("buffer data: %d\n", err);
	}
}

void free_render_def(render_def *rd) {
	glDeleteBuffers(1, &rd->vbo);
	if (rd->uitem_size > 0) glDeleteBuffers(1, &rd->vbou);
	glDeleteVertexArrays(1, &rd->vao);
	free(rd->fences);
	free_shader_program(rd->shader);
	if (rd->depth_shader != 0) free_shader_program(rd->depth_shader);
	if (rd->vp_mat != NULL) free(rd->vp_mat);
	if (rd->depth_vp_mat != NULL) free(rd->depth_vp_mat);
	if (rd->depth_bias_mat != NULL) free(rd->depth_bias_mat);
}

int init_render(render_def *rd) {
	// just starting a buffer. we need to wait and map on that shit
	if (rd->item_idx == 0) {
		glBindVertexArray(rd->vao);
		if (rd->fences[rd->buf_idx] != NULL) {
			GLenum state = glClientWaitSync(rd->fences[rd->buf_idx], GL_SYNC_FLUSH_COMMANDS_BIT, 1000000000);
			if (state == GL_TIMEOUT_EXPIRED || state == GL_WAIT_FAILED) {
				printf("wait for fence on buf_idx %d failed with error %d\n", rd->buf_idx, state);
			}
			glDeleteSync(rd->fences[rd->buf_idx]);
			rd->fences[rd->buf_idx] = NULL;
		}
		glBindBuffer(GL_ARRAY_BUFFER, rd->vbo);
		int map_start = rd->buf_idx * rd->num_items * rd->item_size * sizeof(GLfloat);
		int buf_len = rd->num_items * rd->item_size * sizeof(GLfloat);
		rd->buf = (GLfloat *)glMapBufferRange(GL_ARRAY_BUFFER, map_start, buf_len, GL_MAP_WRITE_BIT|GL_MAP_UNSYNCHRONIZED_BIT);
		if (rd->buf == NULL) printf("failed to map float buffer for buf_idx %d\n", rd->buf_idx);
		if (rd->uitem_size > 0) {
			glBindBuffer(GL_ARRAY_BUFFER, rd->vbou);
			map_start = rd->buf_idx * rd->num_items * rd->uitem_size * sizeof(GLuint);
			buf_len = rd->num_items * rd->uitem_size * sizeof(GLuint);
			rd->ubuf = (GLuint *)glMapBufferRange(GL_ARRAY_BUFFER, map_start, buf_len, GL_MAP_WRITE_BIT|GL_MAP_UNSYNCHRONIZED_BIT);
			if (rd->ubuf == NULL) printf("failed to map uint buffer for buf_idx %d\n", rd->buf_idx);
		}
	}
	if (rd->item_idx == rd->num_items) {
		printf("can't render to buf_idx %d: overflow\n", rd->buf_idx);
		return -1;
	}
	return 1;
}

void render_sprite(render_def *rd, sprite *s) {
	if (init_render(rd) < 0) return;
	int idx = rd->item_idx * rd->item_size;
	rd->buf[idx++] = s->x;
	rd->buf[idx++] = s->y;
	rd->buf[idx++] = s->z;
	rd->buf[idx++] = s->r;
	rd->buf[idx++] = s->g;
	rd->buf[idx++] = s->b;
	rd->buf[idx++] = s->scale_x;
	rd->buf[idx++] = s->scale_y;
	rd->buf[idx] = s->rot;
	if (rd->uitem_size > 0) {
		idx = rd->item_idx * rd->uitem_size;
		rd->ubuf[idx++] = (GLuint)s->spr_idx;
		rd->ubuf[idx++] = (GLuint)s->spr_cols;
		rd->ubuf[idx] = (GLuint)s->spr_extra;
	}
	rd->item_idx++;
}

void render_line(render_def *rd, line *l) {
	if (init_render(rd) < 0) return;
	int idx = rd->item_idx * rd->item_size;
	rd->buf[idx++] = l->p1.x;
	rd->buf[idx++] = l->p1.y;
	rd->buf[idx++] = l->p1.z;
	rd->buf[idx++] = l->p1.r;
	rd->buf[idx++] = l->p1.g;
	rd->buf[idx++] = l->p1.b;
	rd->buf[idx++] = l->p2.x;
	rd->buf[idx++] = l->p2.y;
	rd->buf[idx++] = l->p2.z;
	rd->buf[idx++] = l->p2.r;
	rd->buf[idx++] = l->p2.g;
	rd->buf[idx] = l->p2.b;
	rd->item_idx+=2;
}

void render_line_box(render_def *rd, line_box *l) {
	if (init_render(rd) < 0) return;
	rd->draw_type = GL_LINE_LOOP;
	int idx = rd->item_idx * rd->item_size;
	rd->buf[idx++] = l->x1;
	rd->buf[idx++] = l->y1;
	rd->buf[idx++] = l->z;
	rd->buf[idx++] = l->r;
	rd->buf[idx++] = l->g;
	rd->buf[idx++] = l->b;

	rd->buf[idx++] = l->x1;
	rd->buf[idx++] = l->y2;
	rd->buf[idx++] = l->z;
	rd->buf[idx++] = l->r;
	rd->buf[idx++] = l->g;
	rd->buf[idx++] = l->b;

	rd->buf[idx++] = l->x2;
	rd->buf[idx++] = l->y2;
	rd->buf[idx++] = l->z;
	rd->buf[idx++] = l->r;
	rd->buf[idx++] = l->g;
	rd->buf[idx++] = l->b;

	rd->buf[idx++] = l->x2;
	rd->buf[idx++] = l->y1;
	rd->buf[idx++] = l->z;
	rd->buf[idx++] = l->r;
	rd->buf[idx++] = l->g;
	rd->buf[idx] = l->b;

	rd->item_idx+=4;
}

void render_rect_box(render_def *rd, line_box *l) {
	if (init_render(rd) < 0) return;
	rd->draw_type = GL_TRIANGLES;
	int idx = rd->item_idx * rd->item_size;
	rd->buf[idx++] = l->x1;
	rd->buf[idx++] = l->y1;
	rd->buf[idx++] = l->z;
	rd->buf[idx++] = l->r;
	rd->buf[idx++] = l->g;
	rd->buf[idx++] = l->b;

	rd->buf[idx++] = l->x1;
	rd->buf[idx++] = l->y2;
	rd->buf[idx++] = l->z;
	rd->buf[idx++] = l->r;
	rd->buf[idx++] = l->g;
	rd->buf[idx++] = l->b;

	rd->buf[idx++] = l->x2;
	rd->buf[idx++] = l->y1;
	rd->buf[idx++] = l->z;
	rd->buf[idx++] = l->r;
	rd->buf[idx++] = l->g;
	rd->buf[idx++] = l->b;

	rd->buf[idx++] = l->x2;
	rd->buf[idx++] = l->y1;
	rd->buf[idx++] = l->z;
	rd->buf[idx++] = l->r;
	rd->buf[idx++] = l->g;
	rd->buf[idx++] = l->b;

	rd->buf[idx++] = l->x1;
	rd->buf[idx++] = l->y2;
	rd->buf[idx++] = l->z;
	rd->buf[idx++] = l->r;
	rd->buf[idx++] = l->g;
	rd->buf[idx++] = l->b;

	rd->buf[idx++] = l->x2;
	rd->buf[idx++] = l->y2;
	rd->buf[idx++] = l->z;
	rd->buf[idx++] = l->r;
	rd->buf[idx++] = l->g;
	rd->buf[idx] = l->b;

	rd->item_idx+=6;
	/*
	rd->draw_type = GL_TRIANGLE_STRIP;
	int idx = rd->item_idx * rd->item_size;
	rd->buf[idx++] = l->x1;
	rd->buf[idx++] = l->y1;
	rd->buf[idx++] = l->z;
	rd->buf[idx++] = l->r;
	rd->buf[idx++] = l->g;
	rd->buf[idx++] = l->b;

	rd->buf[idx++] = l->x1;
	rd->buf[idx++] = l->y2;
	rd->buf[idx++] = l->z;
	rd->buf[idx++] = l->r;
	rd->buf[idx++] = l->g;
	rd->buf[idx++] = l->b;

	rd->buf[idx++] = l->x2;
	rd->buf[idx++] = l->y1;
	rd->buf[idx++] = l->z;
	rd->buf[idx++] = l->r;
	rd->buf[idx++] = l->g;
	rd->buf[idx++] = l->b;

	rd->buf[idx++] = l->x2;
	rd->buf[idx++] = l->y2;
	rd->buf[idx++] = l->z;
	rd->buf[idx++] = l->r;
	rd->buf[idx++] = l->g;
	rd->buf[idx] = l->b;

	rd->item_idx+=4;
	*/
}

void render_start_depth_pass(render_def *rd) {
	glViewport(0, 0, rd->depth_viewport_w, rd->depth_viewport_h);
	glBindFramebuffer(GL_FRAMEBUFFER, rd->depth_framebuffer);
	glClear(GL_DEPTH_BUFFER_BIT);
}

void render_start_screen_pass(render_def *rd) {
	glViewport(0, 0, rd->viewport_w, rd->viewport_h);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void render_depth_render_only(render_def *rd, bool use_tex) {
	glUseProgram(rd->depth_shader);
	if (use_tex && rd->tex > 0) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, rd->tex);
	}
	glBindVertexArray(rd->vao);
	if (rd->item_idx > 0) {
		glDrawArrays(rd->draw_type, rd->buf_idx * rd->num_items, rd->item_idx);
	}
}

void render_screen_render_only(render_def *rd) {
	glUseProgram(rd->shader);
	if (rd->tex > 0) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, rd->tex);
	}
	if (rd->use_depth && rd->depth_tex > 0) {
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, rd->depth_tex);
	}
	glBindVertexArray(rd->vao);
	if (rd->item_idx > 0) {
		glDrawArrays(rd->draw_type, rd->buf_idx * rd->num_items, rd->item_idx);
	}
}

void render_buffer_normal(render_def *rd) {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(rd->shader);
	glBindVertexArray(rd->vao);
	if (rd->tex > 0) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, rd->tex);
	}
	if (rd->item_idx > 0) {
		glDrawArrays(rd->draw_type, rd->buf_idx * rd->num_items, rd->item_idx);
	}
}

void render_buffer_depth(render_def *rd) {
	glViewport(0, 0, rd->depth_viewport_w, rd->depth_viewport_h);
	glBindFramebuffer(GL_FRAMEBUFFER, rd->depth_framebuffer);
	glClear(GL_DEPTH_BUFFER_BIT);
	glUseProgram(rd->depth_shader);
	//GLint depthViewProjUnif = glGetUniformLocation(rd->depth_shader, "depth_vp");
	//glUniformMatrix4fv(depthViewProjUnif, 1, GL_FALSE, rd->depth_vp_mat);

	glBindVertexArray(rd->vao);
	if (rd->item_idx > 0) {
		glDrawArrays(rd->draw_type, rd->buf_idx * rd->num_items, rd->item_idx);
	}

	glViewport(0, 0, rd->viewport_w, rd->viewport_h);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(rd->shader);
	//GLint depthBiasUnif = glGetUniformLocation(rd->shader, "depth_vp");
	//glUniformMatrix4fv(depthBiasUnif, 1, GL_FALSE, rd->depth_vp_mat);
	//GLint viewProjUnif = glGetUniformLocation(rd->shader, "vp");
	//glUniformMatrix4fv(viewProjUnif, 1, GL_FALSE, rd->vp_mat);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, rd->tex);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, rd->depth_tex);

	glBindVertexArray(rd->vao);
	if (rd->item_idx > 0) {
		glDrawArrays(rd->draw_type, rd->buf_idx * rd->num_items, rd->item_idx);
	}
}

void render_buffer(render_def *rd) {
	if (rd->use_depth) {
		render_buffer_depth(rd);
	} else {
		render_buffer_normal(rd);
	}
}

void render_advance(render_def *rd) {
	glBindVertexArray(rd->vao);
	glBindBuffer(GL_ARRAY_BUFFER, rd->vbo);
	glUnmapBuffer(GL_ARRAY_BUFFER);
	if (rd->uitem_size > 0) {
		glBindBuffer(GL_ARRAY_BUFFER, rd->vbou);
		glUnmapBuffer(GL_ARRAY_BUFFER);
	}
	rd->fences[rd->buf_idx] = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	rd->buf_idx = ((rd->buf_idx + 1) % rd->num_bufs);
	rd->item_idx = 0;
};
