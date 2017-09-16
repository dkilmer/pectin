#include "example_common.h"
#include "example_box.h"
#include "render_util.h"
#include "game_util.h"
#include "misc_util.h"
#include "window.h"
#include "qubicle.h"
#include "math_3d.h"

int ccol = 0;

bool raycast(vec3_t origin, vec3_t direction, float radius, qcolor *matrix, qsize *qs, vec3_t *block, int *face) {
	block->x = 0;
	block->y = 0;
	block->z = 0;
	*face = 0;
	float x = floorf(origin.x);
	float y = floorf(origin.y);
	float z = floorf(origin.z);
	float dx = direction.x;
	float dy = direction.y;
	float dz = direction.z;
	float stepX = signum(dx);
	float stepY = signum(dy);
	float stepZ = signum(dz);
	float tMaxX = intbound(origin.x, dx);
	float tMaxY = intbound(origin.y, dy);
	float tMaxZ = intbound(origin.z, dz);
	float tDeltaX = stepX / dx;
	float tDeltaY = stepY / dy;
	float tDeltaZ = stepZ / dz;
	radius /= sqrtf(dx*dx + dy*dy + dz*dz);
	qcolor qc;

	while ((x >= 0) && (x <= 32) && (y >=0) && (y <= 20) && (z >= -10)) {
		if (z <= 0) {
			get_qcolor(matrix, qs, (int)x, (int)y, -(int)z, &qc);
			if (qc.a != 0) {
				block->x = x;
				block->y = y;
				block->z = z;
				return true;
			}
		}
		if (tMaxX < tMaxY) {
			if (tMaxX < tMaxZ) {
				if (tMaxX > radius) {
					printf("x: %f > %f\n",tMaxX, radius);
					break;
				}
				x += stepX;
				tMaxX += tDeltaX;
				*face = (stepX < 0) ? 3 : 1;
			} else {
				if (tMaxZ > radius) {
					printf("z: %f > %f\n",tMaxZ, radius);
					break;
				}
				z += stepZ;
				tMaxZ += tDeltaZ;
				*face = (stepZ < 0) ? 0 : 5;
			}
		} else {
			if (tMaxY < tMaxZ) {
				if (tMaxY > radius) {
					printf("y: %f > %f\n",tMaxY, radius);
					break;
				}
				y += stepY;
				tMaxY += tDeltaY;
				*face = (stepY < 0) ? 2 : 4;
			} else {
				if (tMaxZ > radius) {
					printf("z: %f > %f\n",tMaxZ, radius);
					break;
				}
				z += stepZ;
				tMaxZ += tDeltaZ;
				*face = (stepZ < 0) ? 0 : 5;
			}
		}
	}
	//printf("exit with: %f, %f, %f\n",x,y,z);
	return false;
}

void add_light(GLuint shader, int idx, light *l) {
	char uname[1024];

	sprintf(uname, "all_lights[%d].position", idx);
	glUniform4f(glGetUniformLocation(shader, uname), l->position.x, l->position.y, l->position.z, l->position.w);

	sprintf(uname, "all_lights[%d].light_color", idx);
	glUniform3f(glGetUniformLocation(shader, uname), l->light_color.x, l->light_color.y, l->light_color.z);

	sprintf(uname, "all_lights[%d].attenuation", idx);
	glUniform1f(glGetUniformLocation(shader, uname), l->attenuation);

	sprintf(uname, "all_lights[%d].ambient_coefficient", idx);
	glUniform1f(glGetUniformLocation(shader, uname), l->ambient_coefficient);

	sprintf(uname, "all_lights[%d].cone_angle", idx);
	glUniform1f(glGetUniformLocation(shader, uname), l->cone_angle);

	sprintf(uname, "all_lights[%d].cone_direction", idx);
	glUniform3f(glGetUniformLocation(shader, uname), l->cone_direction.x, l->cone_direction.y, l->cone_direction.z);
}

GLint set_depth_mat(GLuint shader, int idx, mat4_t mat) {
	char uname[1024];
	sprintf(uname, "depth_vps[%d]", idx);
	GLint uloc = glGetUniformLocation(shader, uname);
	//printf("uloc for %d is %d\n", idx, uloc);
	glUniformMatrix4fv(uloc, 1, GL_FALSE, (GLfloat *)&mat);
	return uloc;
}

void sprite_for_hilite(float x, float y, float z, unsigned int extra, sprite *s, render_def *rd) {
	s->x = (x + 0.5f + rd->xoff);
	s->y = (y + 0.5f + rd->yoff);
	s->z = z;
	s->r = 1.0;
	s->g = 0.0;
	s->b = 0.0;
	s->scale_x = rd->xscale * 0.5f;
	s->scale_y = rd->yscale * 0.5f;
	s->rot = 0.0f;
	s->spr_idx = 0;
	s->spr_cols = rd->cols;
	s->spr_extra = extra;
}

void sprite_for_box(float x, float y, float z, float zscale, int ridx, int cidx, unsigned int extra, sprite *s, render_def *rd) {
	s->x = (x + 0.5f + rd->xoff);
	s->y = (y + 0.5f + rd->yoff);
	s->z = z;
	s->r = zscale;
	s->g = 0.0;
	s->b = 0.0;
	s->scale_x = rd->xscale * 0.5f;
	s->scale_y = rd->yscale * 0.5f;
	s->rot = 0.0f;
	//int col = rand_int(4)*2;
	//int col = ccol % 4;
	//ccol++;
	s->spr_idx = ((ridx * rd->cols) + cidx);
	s->spr_cols = rd->cols;
	s->spr_extra = extra;
}

void sprite_for_trif(float x, float y, float z, int ridx, int cidx, sprite *s, render_def *rd) {
	s->x = (x + 0.5f + rd->xoff);
	s->y = (y + 0.5f + rd->yoff);
	s->z = z;//((((level_h-y)*level_w)+x) * -0.0001f) + ((float)layer*-1.0f);
	s->r = 0.556863f;
	s->g = 0.443137f;
	s->b = 0.329412f;
	s->scale_x = rd->xscale * 0.5f;
	s->scale_y = rd->yscale * 0.5f;
	s->rot = 0.0f;
	s->spr_idx = ((ridx * rd->cols) + cidx);
	s->spr_cols = rd->cols;
	s->spr_extra = 15;
}

void set_line(line *ln, float x1, float y1, float z1, float x2, float y2, float z2) {
	ln->p1.x = x1;
	ln->p1.y = y1;
	ln->p1.z = z1;
	ln->p2.x = x2;
	ln->p2.y = y2;
	ln->p2.z = z2;
	ln->p1.r = 1.0f;
	ln->p1.g = 1.0f;
	ln->p1.b = 1.0f;
	ln->p2.r = 1.0f;
	ln->p2.g = 1.0f;
	ln->p2.b = 1.0f;
}

void get_chute(qcolor *matrix, qsize *qs, int x, int y, int z, int *row, int *col) {
	qcolor l, r, u, d;
	get_qcolor(matrix, qs, x-1, y, z, &l);
	get_qcolor(matrix, qs, x+1, y, z, &r);
	get_qcolor(matrix, qs, x, y+1, z, &u);
	get_qcolor(matrix, qs, x, y-1, z, &d);
	bool is_l = (l.b == 68 || l.b == 119);
	bool is_r = (r.b == 68 || r.b == 119);
	bool is_u = (u.b == 68 || u.b == 119);
	bool is_d = (d.b == 68 || d.b == 119);
	if (is_u && is_d) {
		*row = 2;
		*col = 2;
	} else if (is_l && is_r) {
		*row = 2;
		*col = 4;
	} else if (is_d && is_l) {
		*row = 3;
		*col = 0;
	} else if (is_d && is_r) {
		*row = 3;
		*col = 2;
	} else if (is_u && is_l) {
		*row = 3;
		*col = 4;
	} else if (is_u && is_r) {
		*row = 3;
		*col = 6;
	} else {
		*row = 2;
		*col = 2;
	}
}

void run_box() {
	screen_w = 1024;
	screen_h = 704;
	if (!init_window("test sdl_ogl", screen_w, screen_h)) return;
	print_sdl_gl_attributes();
	level_w = 32;
	level_h = 20;
	level_layers = 1;
	collision_layer = 0;
	//level = level_box;
	// variables for timing used in physics
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

	vec3_t dlpos = {16.0f, 10.0f, 10.0f};
	//vec3_t dlpos = {16.0f, 10.0f, 30.0f};
	vec3_t dlcat = {-0.0f, 0.0f, -1.0f};
	vec3_t dlup = {0.0f, -1.0f, 0.0f};
	vec3_t trif;
// initialize the screen view
	screen_def sd;
	init_screen(&sd, screen_w, 640, 32, false);
	//glViewport(-32, -32, 512, 320);
	print_screen_def(&sd);
	//mat4_t light_mat = get_point_light_mat(&sd, dlpos, v3_add(dlpos, dlcat), dlup);
	//mat4_t light_bias_mat = get_point_light_bias_mat(&sd, dlpos);

	mat4_t light_mats[6];
	vec3_t cats0 = {1.0f, 0.0f, 0.0f};
	vec3_t cats1 = {-1.0f, 0.0f, 0.0f};
	vec3_t cats2 = {0.0f, 1.0f, 0.0f};
	vec3_t cats3 = {0.0f, -1.0f, 0.0f};
	vec3_t cats4 = {0.0f, 0.0f, 1.0f};
	vec3_t cats5 = {0.0f, 0.0f, -1.0f};

	vec3_t ups0 = {0.0f, -1.0f, 0.0f};
	vec3_t ups1 = {0.0f, -1.0f, 0.0f};
	vec3_t ups2 = {0.0f, 0.0f, 1.0f};
	vec3_t ups3 = {0.0f, 0.0f, -1.0f};
	vec3_t ups4 = {0.0f, -1.0f, 0.0f};
	vec3_t ups5 = {0.0f, -1.0f, 0.0f};

	light_mats[0] = get_point_light_cube_mat(&sd, dlpos, v3_add(dlpos,cats0), ups0);
	light_mats[1] = get_point_light_cube_mat(&sd, dlpos, v3_add(dlpos,cats1), ups1);
	light_mats[2] = get_point_light_cube_mat(&sd, dlpos, v3_add(dlpos,cats2), ups2);
	light_mats[3] = get_point_light_cube_mat(&sd, dlpos, v3_add(dlpos,cats3), ups3);
	light_mats[4] = get_point_light_cube_mat(&sd, dlpos, v3_add(dlpos,cats4), ups4);
	light_mats[5] = get_point_light_cube_mat(&sd, dlpos, v3_add(dlpos,cats5), ups5);

	GLenum err;
	const char *config_filename = "/Users/dmk/code/pectin/config/box_render_defs.cfg";
	// set up rendering for blocks
	render_def *block_def = load_render_def(config_filename, "blocks", (GLfloat *)&sd.vp_mat, NULL, NULL);
	render_def *hilite_def = load_render_def(config_filename, "highlight", (GLfloat *)&sd.vp_mat, NULL, NULL);
	//render_def *line_def = load_render_def(config_filename, "lines", (GLfloat *)&sd.vp_mat, NULL, NULL);

	// set up rendering for font
	//render_def *font_def = load_render_def(config_filename, "font", (GLfloat *)&sd.vp_mat, NULL, NULL);
	render_def *trif_def = load_render_def(config_filename, "trifle", (GLfloat *)&sd.vp_mat, NULL, NULL);
	copy_depth(block_def, trif_def);

	char txt[255];
	float txt_x = 1.0f;
	float txt_y = 38.0f;

	light point_light0 = {
		{dlpos.x, dlpos.y, dlpos.z, 1.0f}, // position
		{1.0f, 0.9f, 0.8f},       // light_color
		0.0001f,                     // attenuation
		0.2f,                    // ambient_coefficient
		180.0f,                        // cone angle
		{0.0f, 0.0f, 0.0f}        // cone direction
	};

	// set up all the uniforms for lighting
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glUseProgram(block_def->shader);
	GLint num_lights_unif = glGetUniformLocation(block_def->shader, "num_lights");
	printf("num_lights_unif: %d\n", num_lights_unif);
	glUniform1i(num_lights_unif, 1);
	GLint camera_pos_unif = glGetUniformLocation(block_def->shader, "camera_pos");
	printf("camera_pos_unif: %d\n", camera_pos_unif);
	glUniform3fv(camera_pos_unif, 1, (GLfloat *)&sd.cam_pos);
	add_light(block_def->shader, 0, &point_light0);
	GLint lp2_unif = glGetUniformLocation(block_def->shader, "all_lights[0].position");
	printf("lp2_unif is %d\n",lp2_unif);

	float bias = 0.005f;
	float far_plane = 60.0f;

	block_def->far_plane_unif = glGetUniformLocation(block_def->shader, "far_plane");
	glUniform1f(block_def->far_plane_unif, far_plane);
	printf("block_def->far_plane_unif is %d\n",block_def->far_plane_unif);
	err = glGetError();
	if (err != GL_NO_ERROR) {
		printf("-=-= ERROR fp %d\n", err);
	}
	block_def->light_pos_unif = glGetUniformLocation(block_def->shader, "light_pos");
	glUniform3f(block_def->light_pos_unif, dlpos.x, dlpos.y, dlpos.z);
	printf("block_def->light_pos_unif is %d\n",block_def->light_pos_unif);
	err = glGetError();
	if (err != GL_NO_ERROR) {
		printf("-=-= ERROR lp %d\n", err);
	}
	block_def->bias_unif = glGetUniformLocation(block_def->shader, "bias");
	glUniform1f(block_def->bias_unif, bias);
	printf("block_def->bias_unif is %d\n",block_def->bias_unif);
	err = glGetError();
	if (err != GL_NO_ERROR) {
		printf("-=-= ERROR bias_unif %d\n", err);
	}
	//GLint depth_vp_unif = glGetUniformLocation(block_def->shader, "depth_vp");
	//printf("depth_vp_unif is %d\n",depth_vp_unif);
	//err = glGetError();
	//if (err != GL_NO_ERROR) {
	//	printf("-=-= ERROR depth_vp_unif %d\n", err);
	//}
	//for (int i=0; i<6; i++) {
	//	block_def->depth_vps_unif[i] = set_depth_mat(block_def->shader, i, light_mats[i]);
	//	printf("block_def->depth_vps_unif[%d] is %d\n", i, block_def->d_far_plane_unif);
	//}

	glUseProgram(trif_def->shader);
	GLint tnum_lights_unif = glGetUniformLocation(trif_def->shader, "num_lights");
	printf("tnum_lights_unif: %d\n", tnum_lights_unif);
	glUniform1i(tnum_lights_unif, 1);
	GLint tcamera_pos_unif = glGetUniformLocation(trif_def->shader, "camera_pos");
	printf("tcamera_pos_unif: %d\n", tcamera_pos_unif);
	glUniform3fv(tcamera_pos_unif, 1, (GLfloat *)&sd.cam_pos);
	add_light(trif_def->shader, 0, &point_light0);
	GLint tlp2_unif = glGetUniformLocation(trif_def->shader, "all_lights[0].position");
	printf("tlp2_unif is %d\n",tlp2_unif);

	glBindFramebuffer(GL_FRAMEBUFFER, block_def->depth_framebuffer);

	glUseProgram(block_def->depth_shader);
	block_def->d_far_plane_unif = glGetUniformLocation(block_def->depth_shader, "far_plane");
	glUniform1f(block_def->d_far_plane_unif, far_plane);
	printf("block_def->d_far_plane_unif is %d\n",block_def->d_far_plane_unif);
	err = glGetError();
	if (err != GL_NO_ERROR) {
		printf("-=-= ERROR fpd %d\n", err);
	}
	block_def->d_light_pos_unif = glGetUniformLocation(block_def->depth_shader, "light_pos");
	glUniform3f(block_def->d_light_pos_unif, dlpos.x, dlpos.y, dlpos.z);
	printf("block_def->d_light_pos_unif is %d\n",block_def->d_light_pos_unif);
	err = glGetError();
	if (err != GL_NO_ERROR) {
		printf("-=-= ERROR lpd %d\n", err);
	}
	for (int i=0; i<6; i++) {
		block_def->d_depth_vps_unif[i] = set_depth_mat(block_def->depth_shader, i, light_mats[i]);
		printf("block_def->depth_vps_unif[%d] is %d\n", i, block_def->d_depth_vps_unif[i]);
	}

	glUseProgram(trif_def->depth_shader);
	trif_def->d_far_plane_unif = glGetUniformLocation(trif_def->depth_shader, "far_plane");
	glUniform1f(trif_def->d_far_plane_unif, far_plane);
	printf("trif_def->d_far_plane_unif is %d\n",trif_def->d_far_plane_unif);
	err = glGetError();
	if (err != GL_NO_ERROR) {
		printf("-=-= ERROR fpd %d\n", err);
	}
	trif_def->d_light_pos_unif = glGetUniformLocation(trif_def->depth_shader, "light_pos");
	glUniform3f(trif_def->d_light_pos_unif, dlpos.x, dlpos.y, dlpos.z);
	printf("block_def->d_light_pos_unif is %d\n",trif_def->d_light_pos_unif);
	err = glGetError();
	if (err != GL_NO_ERROR) {
		printf("-=-= ERROR lpd %d\n", err);
	}
	for (int i=0; i<6; i++) {
		trif_def->d_depth_vps_unif[i] = set_depth_mat(trif_def->depth_shader, i, light_mats[i]);
		printf("trif_def->depth_vps_unif[%d] is %d\n", i, trif_def->d_depth_vps_unif[i]);
	}
	if (err != GL_NO_ERROR) {
		printf("-=-= ERROR what %d\n", err);
	}
	//GLuint sprite_tex;
	GLint texUnif = bind_texture_to_uniform("tex", trif_def->depth_shader, &trif_def->tex, GL_TEXTURE0, 0);
	err = glGetError();
	if (err != GL_NO_ERROR) {
		printf("-=-= ERROR texUnif %d\n", err);
	}
	GLint texMultUnif = glGetUniformLocation(trif_def->depth_shader, "tex_mult");
	float mx = 1.0f / (float)trif_def->cols;
	float my = 1.0f / (float)trif_def->rows;
	glUniform2f(texMultUnif, mx, my);
	if (err != GL_NO_ERROR) {
		printf("-=-= ERROR tex_mult %d\n", err);
	}

	qsize qs;
	qcolor *boxes = load_qubicle_file("/Users/dmk/code/pectin/config/cutout2.qb", &qs);
	int num_boxes = qs.size_x * qs.size_y * qs.size_z;
	/*
	printf("found %d objects\n", num_boxes);
	for (int i=0; i<num_boxes; i++) {
		printf("(%d,%d,%d) - %d %d\n", boxes[i].x, boxes[i].y, boxes[i].z, boxes[i].zsize, boxes[i].thing);
	}
	*/

	sprite *s = (sprite *)malloc(sizeof(sprite));
	sprite *hs = (sprite *)malloc(sizeof(sprite));
	tile_range lr = {0, 32, 0, 20};
	tile_range tr;
	get_tile_range(&sd, &tr, &lr);
	bool cam_moved = false;
	init_render_environment(true);
	/*
	for (int y=tr.b; y<tr.t; y++) {
		for (int x=tr.l; x<tr.r; x++) {
			if (rand_int(5) == 0) continue;
			sprite_for_box((float)x, (float)y, rand_float() * -3.0f, 6.0f, 0, 0, s, block_def);
			render_sprite(block_def, s);
		}
	}
	*/
	int qcnt = 0;
	unsigned int qmin = 0xFFFFFFFF;
	unsigned int qmax = 0;
	qcolor tc;
	for (int qz=0; qz<qs.size_z; qz++) {
		for (int qy=0; qy<qs.size_y; qy++) {
			for (int qx=0; qx<qs.size_x; qx++) {
				get_qcolor(boxes, &qs, qx, qy, qz, &tc);
				if (tc.a == 0) {
					//printf("0,");
					continue;
				}
				//printf("1,");
				unsigned int surround = get_surround(boxes, &qs, qx, qy, qz);
				if (surround >= 0x7ffffff) continue;
				if (surround < qmin) qmin = surround;
				if (surround > qmax) qmax = surround;
				int row = 0;
				int col = 0;
				switch (tc.b) {
					case 141: row = rand_int(2)+4; col = rand_int(4) * 2; break;
					case 68: row = 2; col = 0; break;
					case 119: {
						get_chute(boxes, &qs, qx, qy, qz, &row, &col);
						break;
					}
					default: row = rand_int(2)+4; col = rand_int(4) * 2;
				}
				sprite_for_box((float)qx, (float)qy, (float)-qz, 1.0f, row, col, surround, s, block_def);
				render_sprite(block_def, s);
				qcnt++;
			}
			//printf("\n");
		}
		//printf("\n");
	}
	printf("rendered %d blocks, min=%ud max=%ud\n",qcnt, qmin, qmax);
	/*
	for (int i=0; i<num_boxes; i++) {
		qbox b = boxes[i];
		sprite_for_box((float)b.x, (float)b.y, b.z, b.zsize, 0, b.thing, s, block_def);
		render_sprite(block_def, s);
	}
	*/

	trif.x = 16.0f;
	trif.y = 4.0f;
	trif.z = -0.5f;
	sprite ts;
	int trif_row = 2;
	int trif_col = 0;
	int mult = 4;

	vec3_t mouse_dir;
	int face;
	vec3_t hit_block;
	vec3_t start_pt;
	start_pt.x = sd.cam_pos.x+0.01f;
	start_pt.y = sd.cam_pos.y+0.01f;
	start_pt.z = sd.cam_pos.z+0.01f;

	clock_gettime(CLOCK_REALTIME, &t1);
	int frame = 0;
	bool mouse_down = false;
	while (loop) {
		get_input(kdown, kpress, key_map, &mouse);
		if (kpress[KEY_QUIT]) {
			loop = false;
		}
		bool moved = false;
		if (kdown[KEY_LEFT]) {
			//bias -= 0.0001f;
			dlpos.x -= 0.1f;
			moved = true;
		} else if (kdown[KEY_RIGHT]) {
			//bias += 0.0001f;
			dlpos.x += 0.1f;
			moved = true;
		} else if (kdown[KEY_UP]) {
			dlpos.y += 0.1f;
			moved = true;
		} else if (kdown[KEY_DOWN]) {
			dlpos.y -= 0.1f;
			moved = true;
		} else if (kdown[KEY_USE]) {
			//dlpos.z += 0.1f;
			moved = true;
		} else if (kdown[KEY_GRAB]) {
			//dlpos.z -= 0.1f;
			moved = true;
		}

		mouse_dir.x = (((float)mouse.x / 32.0f)-0.0f) - sd.cam_pos.x;
		mouse_dir.y = (((float)(640 - mouse.y) / 32.0f)-0.0f) - sd.cam_pos.y;
		mouse_dir.z = -start_pt.z + 1.0f;// - 0.5f;
		raycast(start_pt, mouse_dir, 40, boxes, &qs, &hit_block, &face);

		if (mouse.ldown) {
			if (!mouse_down) {
				/*
				mouse_dir.x = (((float)mouse.x / 32.0f)-0.5f) - sd.cam_pos.x;
				mouse_dir.y = (((float)(640 - mouse.y) / 32.0f)-0.5f) - sd.cam_pos.y;
				mouse_dir.z = -sd.cam_pos.z;
				if (raycast(sd.cam_pos, mouse_dir, 40, boxes, &qs, &hit_block, &face)) {
					printf("%f, %f\n", wmx, wmy);
					printf("hit: (%f,%f,%f), face(%d)\n",hit_block.x, hit_block.y, hit_block.z, face);
				} else {
					printf("no hit\n");
				}
				*/
			}
			mouse_down = true;
		} else {
			mouse_down = false;
		}

		if (moved) {
			//printf("light: (%f,%f,%f)\n", dlpos.x, dlpos.y, dlpos.z);
			//glUseProgram(block_def->shader);
			//glUniform1f(bias_unif, bias);
			//light_mat = get_point_light_mat(&sd, dlpos, v3_add(dlpos, dlcat), dlup);
			//light_mat = get_point_light_mat(&sd, dlpos);
			glUseProgram(block_def->shader);
			glUniform4f(lp2_unif, dlpos.x, dlpos.y, dlpos.z, 1.0f);
			glUniform3f(block_def->light_pos_unif, dlpos.x, dlpos.y, dlpos.z);
			//glUniformMatrix4fv(depth_vp_unif, 1, GL_FALSE, (GLfloat *)&light_mat);
			glUseProgram(trif_def->shader);
			glUniform4f(tlp2_unif, dlpos.x, dlpos.y, dlpos.z, 1.0f);
			//glUniform3f(trif_def->light_pos_unif, dlpos.x, dlpos.y, dlpos.z);

			light_mats[0] = get_point_light_cube_mat(&sd, dlpos, v3_add(dlpos,cats0), ups0);
			light_mats[1] = get_point_light_cube_mat(&sd, dlpos, v3_add(dlpos,cats1), ups1);
			light_mats[2] = get_point_light_cube_mat(&sd, dlpos, v3_add(dlpos,cats2), ups2);
			light_mats[3] = get_point_light_cube_mat(&sd, dlpos, v3_add(dlpos,cats3), ups3);
			light_mats[4] = get_point_light_cube_mat(&sd, dlpos, v3_add(dlpos,cats4), ups4);
			light_mats[5] = get_point_light_cube_mat(&sd, dlpos, v3_add(dlpos,cats5), ups5);

			glUseProgram(block_def->depth_shader);
			glUniform3f(block_def->d_light_pos_unif, dlpos.x, dlpos.y, dlpos.z);
			for (int i=0; i<6; i++) {
				glUniformMatrix4fv(block_def->d_depth_vps_unif[i], 1, GL_FALSE, (GLfloat *)&light_mats[i]);
			}
			glUseProgram(trif_def->depth_shader);
			glUniform3f(trif_def->d_light_pos_unif, dlpos.x, dlpos.y, dlpos.z);
			for (int i=0; i<6; i++) {
				glUniformMatrix4fv(trif_def->d_depth_vps_unif[i], 1, GL_FALSE, (GLfloat *)&light_mats[i]);
			}

			err = glGetError();
			if (err != GL_NO_ERROR) {
				printf("-=-= ERROR ??: %d\n", err);
			}
		}
		//glUseProgram(block_def->shader);
		//glUniform4f(lp0_unif, point_light0.position.x, point_light0.position.y, point_light0.position.z, point_light0.position.w);
		//glUniform4f(lp1_unif, point_light1.position.x, point_light1.position.y, point_light1.position.z, point_light1.position.w);
		//glUniform4f(lp2_unif, dlpos.x, dlpos.y, dlpos.z, 0);

		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		/*


		sprintf(txt, "%0.2f %0.2f %0.2f", dlpos.x, dlpos.y, dlpos.z);
		int txt_len = (int)strlen(txt);
		render_advance(font_def);
		int advance = 0;
		float tx = txt_x;
		for (int i=0; i<txt_len; i++) {
			int ch = txt[i] - 32;
			int trow = ch / 24;
			int tcol = ch % 24;
			int back = font_back[ch];
			float kern = (((float)font_kern / 8.0f) * font_def->xscale);
			tx += (((float)advance / 8.0f) * font_def->xscale);
			tx += kern;
			if (back < 0) tx += (((float)back / 8.0f) * font_def->xscale);
			sprite_for(tx, txt_y, 0, trow, tcol, s, font_def);
			render_sprite(font_def, s);
			advance = font_space[ch];
		}
		*/
		render_start_depth_pass(block_def);
		render_depth_render_only(block_def, false);
		render_depth_render_only(trif_def, true);
		//render_buffer(block_def);

		trif_col = ((trif_col + 1) % (mult * 10));

		render_start_screen_pass(block_def);
		render_advance(trif_def);
		sprite_for_trif(trif.x, trif.y, trif.z, trif_row, (trif_col / mult), &ts, trif_def);
		render_sprite(trif_def, &ts);

		render_advance(hilite_def);
		sprite_for_hilite(hit_block.x, hit_block.y, hit_block.z, (unsigned int)face, hs, hilite_def);
		render_sprite(hilite_def, hs);

		//render_buffer(font_def);
		render_screen_render_only(block_def);
		render_screen_render_only(trif_def);
		render_screen_render_only(hilite_def);

		swap_window();
		frame += 1;
		if (frame == 60) frame = 0;
	}
	free(s);
	free(hs);
	free(boxes);
	free_render_def(block_def);
	//free_render_def(font_def);
	free_render_def(trif_def);
	free_render_def(hilite_def);
}

