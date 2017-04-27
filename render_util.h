#ifndef RENDER_UTIL_H
#define RENDER_UTIL_H

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <glad/glad.h>

#if defined __cplusplus
extern "C" {
#endif

typedef struct render_buf {
	int num_points;
	int array_size;
	GLuint vao;
	GLuint vert_bo;
	GLuint color_bo;
	GLuint scale_rot_bo;
	GLuint sprite_loc_bo;
	GLfloat *points;
	GLfloat *colors;
	GLfloat *norm_scale_rots;
	GLfloat *scale_rots;
	GLfloat *sprite_locs;
} render_buf;

GLint load_texture_to_uniform(const char *filename, const char *unif_name, GLuint shaderProgram, GLuint *tex, GLenum tex_num, GLint tex_idx);
GLuint create_geom_shader_program(const char *vert_file_name, const char *geom_file_name, const char *frag_file_name);
void free_geom_shader_program(GLuint shader_program);
render_buf *create_render_buf(int num_points);
void bind_render_buf(render_buf *rb, GLuint shader_program);
void free_render_buf(render_buf *rb);

#ifdef __cplusplus
}
#endif

#endif //RENDER_UTIL_H
