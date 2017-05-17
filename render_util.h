#ifndef RENDER_UTIL_H
#define RENDER_UTIL_H

#include <glad/glad.h>

#if defined __cplusplus
extern "C" {
#endif

typedef struct render_buf {
	int num_items;
	int array_size;
	int buf_idx;
	int num_bufs;
	int spr_idx;
	GLuint shader;
	GLuint tex_id;
	GLuint vao;
	GLuint vbo;
	GLfloat *buf;
	GLsync *fences;
} render_buf;

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

typedef struct spritesheet_def {
	int rows;
	int cols;
	float xscale;
	float yscale;
	float xoff;
	float yoff;
	float layer_xoff;
	float layer_yoff;
} spritesheet_def;

GLint load_texture_to_uniform(const char *filename, const char *unif_name, GLuint shaderProgram, GLuint *tex, GLenum tex_num, GLint tex_idx);
GLuint create_geom_shader_program(const char *vert_file_name, const char *geom_file_name, const char *frag_file_name, const char *tex_file_name, int tex_w, int tex_h, GLfloat *vp_mat, GLuint *tex_id);
void free_geom_shader_program(GLuint shader_program);
void update_view_mat(render_buf *rb, GLfloat *mat);
void init_render_environment();
render_buf *create_render_buf(int max_items, GLuint shader_program, GLuint tex_id);
void free_render_buf(render_buf *rb);
void render_sprite(render_buf *rb, sprite *s);
void render_sprites(render_buf *rb);
void render_advance(render_buf *rb);

#ifdef __cplusplus
}
#endif

#endif //RENDER_UTIL_H
