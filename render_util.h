#ifndef RENDER_UTIL_H
#define RENDER_UTIL_H

#include <stdbool.h>
#include <glad/glad.h>

#if defined __cplusplus
extern "C" {
#endif

// a structure that represents a sprite that is rendered to the screen using
// geometry shader extrusion, rotation and scaling
typedef struct sprite {
	float x;
	float y;
	float z;
	float r;
	float g;
	float b;
	float scale_x;
	float scale_y;
	float rot;
	float spr_row;
	float spr_col;
	float spr_extra;
} sprite;

// a structure that represents a line that is rendered to the screen using
// geometry shader extrusion
typedef struct line_point {
	float x;
	float y;
	float z;
	float r;
	float g;
	float b;
} line_point;

typedef struct line {
	line_point p1;
	line_point p2;
} line;

// a structure to hold the definition for a group of things that will be rendered
// with a single render buffer. This is used both to load up the shader and to define
// the geometry of the sprite sheet, and to hold the buffer and supporting information
// that will be needed when rendering.
typedef struct render_def {
	const char *section_name;
	const char *shader_type;
	const char *vert_shader;
	const char *geom_shader;
	const char *frag_shader;
	const char *depth_vert_shader;
	const char *depth_geom_shader;
	const char *depth_frag_shader;
	const char *sprite_sheet;
	int cols;
	int rows;
	float xscale;
	float yscale;
	float line_scale;
	float xoff;
	float yoff;
	float layer_xoff;
	float layer_yoff;
	int rbuf_size;
	bool use_depth;
	GLuint tex;
	GLuint shader;
	GLuint depth_tex;
	GLuint depth_shader;
	GLfloat *vp_mat;
	GLfloat *depth_vp_mat;
	GLfloat *depth_bias_mat;
	GLenum draw_type;
	int num_items;
	int item_size;
	int array_size;
	int buf_idx;
	int num_bufs;
	int item_idx;
	GLuint vao;
	GLuint vbo;
	GLfloat *buf;
	GLsync *fences;
	GLuint depth_framebuffer;
	int viewport_w;
	int viewport_h;
	int depth_viewport_w;
	int depth_viewport_h;
} render_def;

render_def *load_render_def(const char *filename, const char *section, GLfloat *vp_matrix, GLfloat *depth_vp_matrix, GLfloat *depth_bias_matrix);
GLint load_texture_to_uniform(const char *filename, const char *unif_name, GLuint shaderProgram, GLuint *tex, GLenum tex_num, GLint tex_idx);
void create_sprite_shader_program(render_def *rd);
void create_line_shader_program(render_def *rd);
void free_shader_program(GLuint shader_program);
GLuint create_depth_buffer(int w, int h, GLuint *depth_map);
void update_view_mat(render_def *rd, GLfloat *mat);
void init_render_environment();
void set_sprite_render_attribs(GLuint shader, int item_size);
void create_sprite_render_buf(render_def *rd);
void create_line_render_buf(render_def *rd);
void create_depth_shader_program(render_def *rd);
void free_render_def(render_def *rd);
void render_sprite(render_def *rd, sprite *s);
void render_line(render_def *rd, line *l);
void render_buffer(render_def *rd);
void render_advance(render_def *rd);

#ifdef __cplusplus
}
#endif

#endif //RENDER_UTIL_H
