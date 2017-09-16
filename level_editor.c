#include <stdlib.h>
#include "level_editor.h"
#include "example_common.h"
#include "level.h"
#include "game_util.h"
#include "render_util.h"
#include "window.h"
#include "math_3d.h"
#include "misc_util.h"

bool vraycast(vec3_t origin, vec3_t direction, float radius, int left_x, lev *l, vec3_t *block, int *face) {
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

	while ((x >= 0) && (x <= 32) && (y >=0) && (y <= 20) && (z >= -10)) {
		if (z <= 0) {
			int xx = left_x + (int)x;
			if (xx >= (int)l->size_x) xx -= (int)l->size_x;
			int32_t v = vox_at(l, xx, (int)y, -(int)z);
			if (v >= 0) {
				block->x = xx;
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

void save_blank_level(const char *filename) {
	lev *l = (lev *)malloc(sizeof(lev));
	l->size_x = 320;
	l->size_y = 20;
	l->size_z = 4;
	int lev_size = l->size_x * l->size_y * l->size_z;
	l->vox = (int32_t *)malloc(sizeof(int32_t) * lev_size);
	for (int z=0; z<l->size_z; z++) {
		for (int y=0; y<l->size_y; y++) {
			for (int x=0; x<l->size_x; x++) {
				int32_t v = (z == 3) ? 0 : -1;
				set_vox_at(l, x, y, z, v);
			}
		}
	}
	save_level(l, filename);
}

void set_light(GLuint shader, light *l) {
	glUniform4f(glGetUniformLocation(shader, "pt_light.position"), l->position.x, l->position.y, l->position.z, l->position.w);
	glUniform3f(glGetUniformLocation(shader, "pt_light.light_color"), l->light_color.x, l->light_color.y, l->light_color.z);
	glUniform1f(glGetUniformLocation(shader, "pt_light.attenuation"), l->attenuation);
	glUniform1f(glGetUniformLocation(shader, "pt_light.ambient_coefficient"), l->ambient_coefficient);
	glUniform1f(glGetUniformLocation(shader, "pt_light.cone_angle"), l->cone_angle);
	glUniform3f(glGetUniformLocation(shader, "pt_light.cone_direction"), l->cone_direction.x, l->cone_direction.y, l->cone_direction.z);
}

void sprite_for_highlight(float x, float y, float z, unsigned int extra, sprite *s, render_def *rd) {
	s->x = (x + (rd->xscale * 0.5f) + rd->xoff);
	s->y = (y + (rd->yscale * 0.5f) + rd->yoff);
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

void sprite_for_vox(float x, float y, float z, float zscale, int idx, unsigned int extra, sprite *s, render_def *rd) {
	s->x = (x + (rd->xscale * 0.5f) + rd->xoff);
	s->y = (y + (rd->yscale * 0.5f) + rd->yoff);
	s->z = -z;
	s->r = zscale;
	s->g = 0.0;
	s->b = 0.0;
	s->scale_x = rd->xscale * 0.5f;
	s->scale_y = rd->yscale * 0.5f;
	s->rot = 0.0f;
	s->spr_idx = idx;
	s->spr_cols = rd->cols;
	s->spr_extra = extra;
}

void sprite_for_font(float x, float y, float z, float r, float g, float b, int ridx, int cidx, sprite *s, render_def *rd) {
	s->x = (x + (rd->xscale * 0.5f) + rd->xoff);
	s->y = (y + (rd->yscale * 0.5f) + rd->yoff);
	s->z = z;
	s->r = r; //0.556863f;
	s->g = g; //0.443137f;
	s->b = b; //0.329412f;
	s->scale_x = rd->xscale * 0.5f;
	s->scale_y = rd->yscale * 0.5f;
	s->rot = 0.0f;
	s->spr_idx = ((ridx * rd->cols) + cidx);
	s->spr_cols = rd->cols;
	s->spr_extra = 0;
}

float text_extent(const char *txt, render_def *font_def) {
	int txt_len = (int)strlen(txt);
	int advance = 0;
	float tx = 0;
	for (int i=0; i<txt_len; i++) {
		int ch = txt[i] - 32;
		int back = font_back[ch];
		float kern = (((float)font_kern / 8.0f) * font_def->xscale);
		tx += (((float)advance / 8.0f) * font_def->xscale);
		tx += kern;
		if (back < 0) tx += (((float)back / 8.0f) * font_def->xscale);
		advance = font_space[ch];
	}
	tx += (((float)advance / 8.0f) * font_def->xscale);
	return tx;
}

void render_text(const char *txt, line_point *txt_pos, render_def *font_def, sprite *s) {
	int txt_len = (int)strlen(txt);
	int advance = 0;
	float tx = txt_pos->x;
	for (int i=0; i<txt_len; i++) {
		int ch = txt[i] - 32;
		int trow = ch / font_def->cols;
		int tcol = ch % font_def->cols;
		int back = font_back[ch];
		float kern = (((float)font_kern / 8.0f) * font_def->xscale);
		tx += (((float)advance / 8.0f) * font_def->xscale);
		tx += kern;
		if (back < 0) tx += (((float)back / 8.0f) * font_def->xscale);
		sprite_for_font(tx, txt_pos->y, 0, txt_pos->r, txt_pos->g, txt_pos->b, trow, tcol, s, font_def);
		render_sprite(font_def, s);
		advance = font_space[ch];
	}
}

void render_button(button *b, render_def *box_def, render_def *rct_def, render_def *font_def, sprite *s) {
	render_rect_box(rct_def, &b->fill);
	render_line(box_def, &b->outline);
	render_text(b->txt, &b->txt_pos, font_def, s);
}

void set_button_state(button *b, int state) {
	if (state < 0 || state > BST_DOWN) state = BST_NORMAL;
	switch (state) {
		case BST_NORMAL: {
			b->outline.p1.r = 0.5f;
			b->outline.p1.g = 0.5f;
			b->outline.p1.b = 0.5f;
			b->outline.p2.r = 0.5f;
			b->outline.p2.g = 0.5f;
			b->outline.p2.b = 0.5f;
			b->fill.r = 0.1f;
			b->fill.g = 0.1f;
			b->fill.b = 0.1f;
			b->txt_pos.r = 0.7f;
			b->txt_pos.g = 0.7f;
			b->txt_pos.b = 0.7f;
			break;
		}
		case BST_HOVER: {
			b->outline.p1.r = 0.7f;
			b->outline.p1.g = 0.7f;
			b->outline.p1.b = 0.7f;
			b->outline.p2.r = 0.7f;
			b->outline.p2.g = 0.7f;
			b->outline.p2.b = 0.7f;
			b->fill.r = 0.3f;
			b->fill.g = 0.3f;
			b->fill.b = 0.3f;
			b->txt_pos.r = 0.9f;
			b->txt_pos.g = 0.9f;
			b->txt_pos.b = 0.9f;
			break;
		}
		case BST_DOWN: {
			b->outline.p1.r = 0.9f;
			b->outline.p1.g = 0.9f;
			b->outline.p1.b = 0.9f;
			b->outline.p2.r = 0.9f;
			b->outline.p2.g = 0.9f;
			b->outline.p2.b = 0.9f;
			b->fill.r = 0.5f;
			b->fill.g = 0.5f;
			b->fill.b = 0.5f;
			b->txt_pos.r = 1.0f;
			b->txt_pos.g = 1.0f;
			b->txt_pos.b = 1.0f;
			break;
		}
	}
}

void setup_button(button *b, int id, float x, float y, float w, float h, const char *txt, render_def *font_def) {
	float margin = 0.01f;
	b->id = id;
	b->outline.p1.x = x;
	b->outline.p1.y = y;
	b->outline.p1.z = 0;
	b->outline.p1.r = 0;
	b->outline.p1.g = 0;
	b->outline.p1.b = 0;
	b->outline.p2.x = x+w;
	b->outline.p2.y = y+h;
	b->outline.p2.z = 0;
	b->outline.p2.r = 0;
	b->outline.p2.g = 0;
	b->outline.p2.b = 0;
	b->fill.x1 = x+margin;
	b->fill.x2 = x+(w-margin);
	b->fill.y1 = y+margin;
	b->fill.y2 = y+(h-margin);
	b->fill.z = -0.01f;
	b->fill.r = 0;
	b->fill.g = 0;
	b->fill.b = 0;
	strcpy(b->txt, txt);
	float extent = text_extent(txt, font_def);
	b->txt_pos.x = x+((w-extent)/2.0f);
	b->txt_pos.y = y+((h-font_def->yscale)/2.0f);
	b->txt_pos.z = 0;
	b->txt_pos.r = 0;
	b->txt_pos.g = 0;
	b->txt_pos.b = 0;
}

void set_button_txt(button *b, const char *txt, render_def *font_def) {
	float x = b->outline.p1.x;
	float y = b->outline.p1.y;
	float w = b->outline.p2.x - x;
	float h = b->outline.p2.y - y;
	strcpy(b->txt, txt);
	float extent = text_extent(txt, font_def);
	b->txt_pos.x = x+((w-extent)/2.0f);
	b->txt_pos.y = y+((h-font_def->yscale)/2.0f);
	b->txt_pos.z = 0;
}

bool in_button(button *b, float mx, float my) {
	return (mx >= b->outline.p1.x && mx <= b->outline.p2.x && my >= b->outline.p1.y && my <= b->outline.p2.y);
}

void run_level_editor() {
	save_blank_level("/Users/dmk/code/pectin/config/test_level.dat");
	screen_w = 1024;
	screen_h = 704;
	level_w = 32;
	if (!init_window("level_editor", screen_w, screen_h)) return;
	print_sdl_gl_attributes();
	level_h = 22;
	level_layers = 1;
	collision_layer = 0;
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

	GLenum err;
	const char *config_filename = "/Users/dmk/code/pectin/config/level_editor_defs.cfg";

	screen_def gd;
	init_screen(&gd, screen_w, screen_h, 32, true);

	render_def *font_def = load_render_def(config_filename, "font", (GLfloat *)&gd.vp_mat, NULL, NULL);
	render_def *box_def = load_render_def(config_filename, "boxes", (GLfloat *)&gd.vp_mat, NULL, NULL);
	render_def *rct_def = load_render_def(config_filename, "rects", (GLfloat *)&gd.vp_mat, NULL, NULL);

	screen_def sd;
	init_screen(&sd, screen_w, screen_h, 32, false);
	print_screen_def(&sd);

	render_def *block_def = load_render_def(config_filename, "blocks", (GLfloat *)&sd.vp_mat, NULL, NULL);
	render_def *hilite_def = load_render_def(config_filename, "highlight", (GLfloat *)&sd.vp_mat, NULL, NULL);

	vec3_t dlpos = {16.0f, 10.0f, 10.0f};
	light pt_light = {
		{dlpos.x, dlpos.y, dlpos.z, 1.0f}, // position
		{1.0f, 0.9f, 0.8f},       // light_color
		0.0001f,                     // attenuation
		0.2f,                    // ambient_coefficient
		180.0f,                        // cone angle
		{0.0f, 0.0f, 0.0f}        // cone direction
	};

	glUseProgram(block_def->shader);
	GLint camera_pos_unif = glGetUniformLocation(block_def->shader, "camera_pos");
	printf("camera_pos_unif: %d\n", camera_pos_unif);
	glUniform3fv(camera_pos_unif, 1, (GLfloat *)&sd.cam_pos);
	set_light(block_def->shader, &pt_light);

	lev *l = load_level("/Users/dmk/code/pectin/config/test_level.dat");
	init_render_environment(true);

	sprite *s = (sprite *)malloc(sizeof(sprite));
	vec3_t mouse_dir;
	int face = 0;
	vec3_t hit_block;
	vec3_t start_pt;
	start_pt.x = sd.cam_pos.x+0.01f;
	start_pt.y = sd.cam_pos.y+0.01f;
	start_pt.z = sd.cam_pos.z+0.01f;

	button btns[NUM_BTNS];

	setup_button(&btns[BTN_LEFT], BTN_LEFT, 0.2f, 20.5f, 1, 1, "<", font_def);
	setup_button(&btns[BTN_RIGHT], BTN_RIGHT, 27, 20.5f, 1, 1, ">", font_def);
	setup_button(&btns[BTN_BRUSH], BTN_BRUSH, 28.4f, 20.5f, 1, 1, "add", font_def);
	setup_button(&btns[BTN_LAYERS], BTN_LAYERS, 29.6f, 20.5f, 1, 1, "4", font_def);
	setup_button(&btns[BTN_SAVE], BTN_SAVE, 30.8f, 20.5f, 1, 1, "save", font_def);
	for (int i=0; i<NUM_BTNS; i++) {
		set_button_state(&btns[i], BST_NORMAL);
	}

	err = glGetError();
	if (err != GL_NO_ERROR) {
		printf("-=-= ERROR %d\n", err);
	}

	float ex = text_extent("<", font_def);
	printf("< is %f\n",ex);
	ex = text_extent(">", font_def);
	printf("> is %f\n",ex);
	ex = text_extent("save", font_def);
	printf("save is %f\n",ex);
	ex = text_extent("paint", font_def);
	printf("paint is %f\n",ex);
	float ptx = 1.0f+((1.0f-ex)/2.0f);
	printf("ptx is %f\n", ptx);

	clock_gettime(CLOCK_REALTIME, &t1);
	bool mouse_down = false;
	int frame = 0;
	int left_x = (int)sd.cam_pos.x - (int)sd.half_w;
	int mv_step = 0;
	int mv_dir = 0;
	int mv_frames = 5;
	int bs;
	int show_layers = 4;
	char digit[4];
	while (loop) {
		get_input(kdown, kpress, key_map, &mouse);
		float mx = ((float)mouse.x / 32.0f);
		float my = ((float)((704 - mouse.y)) / 32.0f);
		if (mv_step == 0) {
			if (kpress[KEY_QUIT]) {
				loop = false;
			} else if (kdown[KEY_LEFT]) {
				//left_x--;
				//if (left_x < 0) left_x = (int)l->size_x;
				mv_dir = 1;
				mv_step = mv_frames;
			} else if (kdown[KEY_RIGHT]) {
				//left_x++;
				//if (left_x >= (int)l->size_x) left_x = 0;
				mv_dir = -1;
				mv_step = mv_frames;
			}
			if (mouse.ldown) {
				for (int i=0; i<NUM_BTNS; i++) {
					bs = (in_button(&btns[i], mx, my)) ? BST_DOWN : BST_NORMAL;
					set_button_state(&btns[i], bs);
				}
				if (!mouse_down) {
					int nx = (int)hit_block.x;
					int ny = (int)hit_block.y;
					int nz = -((int)hit_block.z);
					switch (face) {
						case 0: nz -= 1; break;
						case 1: nx -= 1; break;
						case 2: ny += 1; break;
						case 3: nx += 1; break;
						case 4: ny -= 1; break;
						default: break;
					}
					if (nx >= 0 && nx < (int)l->size_x && ny >= 0 && ny < (int)l->size_y && nz >= 0 && nz < (int)l->size_z) {
						set_vox_at(l, nx, ny, nz, 0);
					}
					// do something
				}
				mouse_down = true;
			} else {
				int the_down = -1;
				for (int i=0; i<NUM_BTNS; i++) {
					bs = (in_button(&btns[i], mx, my)) ? BST_HOVER : BST_NORMAL;
					set_button_state(&btns[i], bs);
					if (bs) the_down = i;
				}
				if (mouse_down && the_down >= 0) {
					switch (the_down) {
						case BTN_LEFT: {
							break;
						}
						case BTN_RIGHT: {
							break;
						}
						case BTN_BRUSH: {
							break;
						}
						case BTN_LAYERS: {
							show_layers--;
							if (show_layers < 1) show_layers = 4;
							sprintf(digit, "%d", show_layers);
							set_button_txt(&btns[the_down], digit, font_def);
							break;
						}
						case BTN_SAVE: {
							break;
						}
						default: break;
					}
				}
				mouse_down = false;
			}
		}
		if (mv_step > 0) {
			mv_step--;
			if (mv_step == 0) {
				left_x -= mv_dir;
				if (left_x >= (int)l->size_x) {
					left_x -= (int)l->size_x;
				} else if (left_x < 0) {
					left_x += (int)l->size_x;
				}
				mv_dir = 0;
			}
			mouse_down = false;
		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, block_def->viewport_w, block_def->viewport_h);

		float dist_x = ((float)(mv_frames-mv_step) / (float)mv_frames) * (float)mv_dir;

		mouse_dir.x = mx - sd.half_w;
		mouse_dir.y = my - sd.half_h;
		mouse_dir.z = -start_pt.z + 1.0f;// - 0.5f;
		vraycast(start_pt, mouse_dir, 40, left_x, l, &hit_block, &face);

		//render_start_screen_pass(block_def);
		render_advance(block_def);
		int sx = (int)sd.cam_pos.x - (level_w / 2);
		int sy = (int)sd.cam_pos.y - (level_h / 2);
		int minz = (l->size_z-show_layers);
		for (int z=l->size_z-1; z>=minz; z--) {
			for (int y=sy; y<(sy+(level_h-2)); y++) {
				for (int x=left_x-2; x<(left_x+level_w+2); x++) {
					int xx = x;
					if (xx >= (int)l->size_x) {
						xx -= (int)l->size_x;
					} else if (xx < 0) {
						xx += (int)l->size_x;
					}
					int v = vox_at(l, xx, y, z);
					if (v < 0) continue;
					unsigned int surround = vox_surround(l, xx, y, z);
					if (surround >= 0x7ffffff) continue;
					sprite_for_vox(((float)x-left_x)+dist_x, (float)y, (float)z, 1.0f, v, surround, s, block_def);
					render_sprite(block_def, s);
				}
			}
		}

		render_advance(hilite_def);
		int hx = (int)hit_block.x - left_x;
		if (hx < 0) hx += (int)l->size_x;
		sprite_for_highlight((float)hx + dist_x, hit_block.y, hit_block.z, (unsigned int)face, s, hilite_def);
		render_sprite(hilite_def, s);

		render_advance(font_def);
		render_advance(box_def);
		render_advance(rct_def);
		for (int i=0; i<NUM_BTNS; i++) {
			render_button(&btns[i], box_def, rct_def, font_def, s);
		}

		render_buffer(block_def);
		render_buffer(hilite_def);
		render_buffer(box_def);
		render_buffer(rct_def);
		render_buffer(font_def);

		swap_window();
		frame += 1;
		if (frame == 60) frame = 0;
	}
	free(s);
	free_level(l);
	free_render_def(block_def);
	free_render_def(hilite_def);
	free_render_def(box_def);
	free_render_def(rct_def);
	free_render_def(font_def);
}
