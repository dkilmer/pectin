#include "example_common.h"
#include "example_trifle.h"
#include "window.h"
#include "render_util.h"
#include "game_util.h"
#include "physics.h"
#include "misc_util.h"
#include "boids.h"
#include "colors.h"

int level_trifle[] = {
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,
	2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,
	3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,
	3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,
	3,1,1,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,
	3,3,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,
	2,4,4,2,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	3,0,0,0,0,1,3,0,0,0,0,0,0,0,52,51,57,49,51,49,57,49,0,0,0,0,0,0,0,0,0,2,
	3,0,0,0,0,0,2,2,0,0,0,0,0,0,52,0,0,0,0,0,0,53,0,0,3,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,3,2,0,0,0,0,0,53,0,0,0,0,0,0,54,0,0,1,0,0,0,0,0,0,2,
	3,0,0,0,0,0,0,0,0,0,0,0,0,0,54,0,0,0,0,0,0,49,0,0,4,0,0,0,0,0,0,1,
	4,3,4,3,2,2,4,2,2,3,3,4,2,2,3,4,1,1,1,2,2,4,4,1,4,1,4,1,3,4,2,3,
	0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,
	4,2,4,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,
	3,2,3,3,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	2,1,2,2,2,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,
	3,3,4,4,2,1,1,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,
	3,3,3,2,2,1,3,1,1,0,0,0,0,0,56,55,53,53,51,54,57,57,0,0,0,0,0,0,0,0,0,2,
	1,4,0,0,0,0,0,4,4,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,0,0,0,0,0,0,3,
	4,3,0,0,0,0,0,0,3,4,1,0,0,0,0,0,0,0,0,0,0,0,0,0,4,0,0,0,0,0,0,1,
	4,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,4,
	3,1,1,2,2,1,3,4,1,2,1,3,3,4,4,4,2,1,1,2,4,3,1,4,3,3,4,3,1,2,3,1,
	0,0,3,2,2,4,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,3,4,1,2,0,0,
	0,0,0,3,3,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,1,2,0,0,0,
	0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,
	3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,
	3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,
	2,2,1,4,3,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,
	3,4,2,2,2,3,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,
	4,3,1,2,2,2,3,1,0,0,0,0,0,0,0,0,0,0,51,0,0,0,0,0,0,0,0,0,0,0,0,2,
	1,2,1,2,1,1,3,1,1,0,0,0,0,0,0,0,0,50,51,49,0,0,0,0,0,0,0,0,0,0,0,2,
	1,1,4,3,1,1,2,2,3,2,0,0,0,0,0,0,51,56,55,57,51,0,0,0,0,0,0,0,0,0,0,4,
	2,4,4,3,3,3,4,3,1,1,4,0,0,0,58,55,51,54,58,57,56,55,0,0,0,0,0,0,0,0,0,1,
	4,2,1,2,4,4,1,2,3,1,4,1,0,0,0,0,0,0,0,0,0,0,0,0,2,3,2,1,3,3,1,4,
	4,1,2,4,2,2,4,3,3,3,4,3,4,0,0,0,0,0,0,0,0,0,0,0,4,4,1,1,2,1,1,1,
	3,3,4,4,3,1,3,2,3,4,4,2,1,3,0,0,0,0,0,0,0,0,0,0,1,2,3,4,4,3,2,4,
	3,1,4,4,1,3,3,1,3,1,4,1,2,1,1,2,4,3,2,1,3,4,2,1,2,1,4,1,1,4,4,3,
	1,2,3,3,2,4,1,4,3,0,0,2,3,1,2,1,2,1,3,3,3,1,0,3,1,2,3,3,1,4,2,3,
	0,3,3,1,2,4,4,1,0,0,0,2,2,2,1,3,4,2,1,1,4,2,0,0,1,4,4,2,2,3,1,0,
	0,0,3,3,1,1,4,0,0,0,0,0,4,2,1,3,4,3,4,3,4,2,0,0,0,1,2,2,1,3,0,0,
	0,0,0,4,3,1,0,0,0,0,0,0,4,2,1,3,1,2,4,1,3,0,0,0,0,0,4,3,3,0,0,0,
	0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,0,0,0,0,
	19,17,18,17,19,17,17,17,18,17,18,20,18,18,18,20,18,17,17,20,18,20,18,19,17,19,19,20,20,19,19,18,
	19,20,17,19,17,19,19,20,19,18,18,17,19,17,19,17,18,20,19,20,18,17,18,19,17,19,18,19,18,20,17,18,
	20,17,17,18,18,18,17,20,20,18,18,18,19,19,17,19,17,20,18,17,19,18,19,19,19,17,19,19,18,17,19,19,
	20,17,20,18,20,17,20,17,18,18,17,20,20,20,17,20,17,18,20,19,19,20,18,20,20,19,20,18,18,18,19,17,
	18,17,18,18,18,17,19,19,17,19,19,17,18,19,18,19,20,19,19,19,17,20,19,18,18,20,19,17,19,19,17,18,
	20,18,19,19,20,19,17,20,19,17,18,20,19,20,17,20,19,18,20,19,18,17,20,17,17,19,20,20,17,20,17,19,
	18,17,20,18,20,17,19,19,20,18,17,20,17,17,17,19,17,19,18,18,18,17,20,17,19,17,20,17,20,19,19,17,
	18,19,20,20,18,20,18,17,17,18,20,20,17,17,19,20,19,19,17,18,20,20,18,18,18,20,18,18,17,19,18,18,
	17,18,18,18,19,17,18,17,19,17,17,18,17,18,17,18,17,19,18,18,17,17,17,17,17,18,17,20,18,18,17,18,
	20,18,17,19,17,20,19,17,17,18,20,20,17,18,18,18,18,17,20,19,19,20,19,17,19,19,18,20,18,20,18,20,
	20,20,17,19,20,19,19,19,20,18,17,18,20,18,20,20,18,17,17,18,19,19,17,18,20,19,17,19,19,20,20,18,
	20,18,18,17,18,20,19,19,18,20,17,20,20,20,18,20,17,20,19,20,17,20,19,18,18,20,19,20,18,18,17,18,
	17,17,17,20,17,17,17,20,17,20,20,17,18,18,18,18,17,18,18,17,20,17,18,18,17,20,17,17,18,20,17,20,
	18,18,18,20,18,20,20,20,20,18,19,20,20,20,17,19,17,18,17,18,17,17,17,17,20,17,19,18,19,20,17,17,
	19,18,19,19,19,20,19,17,19,17,20,19,18,18,19,19,20,17,18,18,19,17,17,20,18,18,17,20,17,18,18,18,
	18,19,17,17,20,17,19,19,19,17,18,19,17,19,18,18,20,17,20,20,19,20,19,19,18,18,18,19,18,17,17,20,
	18,20,17,18,19,17,19,17,20,20,17,18,18,18,20,20,20,17,18,18,20,20,19,17,17,19,17,19,19,19,20,19,
	19,19,17,19,20,17,20,18,20,19,17,18,19,19,17,19,17,18,18,20,18,20,20,20,18,17,19,19,17,19,17,18,
	17,17,17,19,19,19,19,17,17,18,19,19,18,17,20,17,17,17,20,19,19,18,18,19,17,19,17,19,19,18,20,19,
	18,17,20,18,19,18,20,19,19,19,20,17,19,20,18,19,20,20,20,18,20,17,17,20,20,17,17,19,19,18,19,20,
	17,18,19,18,17,19,19,19,17,19,19,19,18,19,20,17,18,19,19,19,17,19,18,17,17,19,19,20,20,20,19,20,
	19,19,19,18,20,19,19,17,17,19,18,17,17,17,18,19,19,19,20,20,18,19,20,18,17,19,17,20,18,19,18,18,
	20,17,19,17,20,19,20,19,17,20,19,19,18,19,18,19,19,18,18,18,19,18,19,18,19,18,18,18,19,18,17,19,
	17,20,19,18,20,19,18,19,18,17,20,17,20,20,18,18,18,18,20,18,18,18,20,17,17,19,20,19,19,20,17,20,
	20,18,20,17,19,18,18,19,18,17,17,18,18,20,20,18,20,17,17,17,17,18,18,17,17,17,18,20,18,19,19,18,
	19,18,18,19,20,18,18,17,19,20,19,17,19,20,20,18,19,20,20,19,19,20,18,17,20,17,17,19,20,19,20,17,
	20,20,19,20,17,19,20,17,20,18,18,19,17,18,20,19,20,17,19,17,18,20,20,18,19,19,20,17,18,18,18,19,
	20,20,18,20,18,17,17,19,20,20,20,18,17,20,19,18,20,19,20,19,19,18,18,19,18,17,19,20,17,18,19,17,
	20,17,19,20,17,17,20,18,17,20,17,17,18,19,17,17,19,19,19,19,20,19,17,19,17,17,19,19,18,20,17,18,
	18,18,19,18,18,18,18,20,18,20,18,20,19,17,18,19,18,17,20,17,18,18,18,20,18,19,18,18,18,19,20,19,
	18,20,18,19,18,17,17,19,20,20,20,20,20,18,17,17,17,17,20,18,18,20,20,17,18,18,19,18,20,18,19,19,
	17,18,18,20,19,18,20,18,17,20,17,19,17,20,19,20,18,20,19,18,20,19,20,18,17,20,20,18,17,20,20,19,
	19,18,18,18,20,19,20,19,17,20,18,17,20,20,17,17,17,18,19,18,19,18,18,18,19,18,20,17,19,20,19,18,
	17,19,20,18,20,19,17,17,17,19,19,18,17,18,20,20,20,18,19,20,19,17,18,19,17,17,17,20,20,17,18,17,
	17,20,19,17,19,19,17,19,18,18,18,19,20,17,19,20,17,18,18,17,19,17,20,18,19,18,18,17,17,20,18,17,
	20,17,19,19,20,17,20,20,19,20,20,19,19,20,19,17,17,20,19,19,18,17,18,20,19,18,20,19,18,18,17,18,
	17,17,20,20,17,18,17,17,19,20,17,18,18,17,17,17,19,20,18,20,18,17,18,20,17,20,19,17,19,20,17,18,
	18,18,20,18,17,17,19,18,19,17,20,19,20,17,19,20,20,19,19,19,18,19,19,18,17,19,18,17,18,17,20,20,
	18,18,20,17,17,17,18,18,18,18,17,18,20,19,20,20,19,19,18,20,20,18,17,20,18,20,18,20,20,17,17,18,
	19,18,19,17,18,20,18,19,17,20,18,19,19,18,19,20,20,19,19,19,18,17,17,17,18,19,17,19,18,20,19,18
};

void aabb_for(float x, float y, aabb *b) {
	b->x = x;
	b->y = y;
	b->rx = 0.5f;
	b->ry = 0.5f;
}

void sprite_for_pobj(pobj *p, sprite *s, int layer, render_def *rd) {
	s->x = p->x;
	s->y = p->y;
	s->z = ((((level_h-p->y)*level_w)+p->x) * -0.0001f) + ((float)layer*-1.0f);
	//s->z -= ((0.1f * oscillate(p->scale_norm_x, 0.0f, 1.0f)) - 0.05f);
	//s->z += (0.004f * oscillate(p->z_norm, 0.0f, 1.0f));
	s->r = p->r;
	s->g = p->g;
	s->b = p->b;
	s->scale_x = rd->xscale * oscillate(p->scale_norm_x, 0.8f, 1.0f);
	s->scale_y = rd->yscale * oscillate(p->scale_norm_y, 0.8f, 1.0f);
	//s->scale_x = (CubicEaseInOut(p->scale_norm_x) * 0.3f) + 0.08f;
	//s->scale_y = (CubicEaseInOut(p->scale_norm_y) * 0.3f) + 0.08f;
	s->rot = p->rot_norm;
	s->spr_idx = 126;
	s->spr_cols = rd->cols;
	s->spr_extra = 0;
}

void sprite_for_boid(boid *b, sprite *s, int layer, render_def *rd) {
	s->x = b->x;
	s->y = b->y;
	s->z = ((((level_h-b->y)*level_w)+b->x) * -0.00001f) + ((float)layer*-1.0f);
	s->r = 0.0f;
	s->g = 0.0f;
	s->b = 0.0f;
	//s->scale_x = rd->xscale * fabsf(b->vx) * 4.0f;
	//s->scale_y = rd->yscale * fabsf(b->vy) * 4.0f;
	s->scale_x = rd->xscale + ((float)b->frame * 0.02f);
	s->scale_y = rd->yscale + ((float)b->frame * 0.02f);
	s->rot = b->ang;
	s->spr_idx = (147 + b->frame);
	s->spr_cols = rd->cols;
	s->spr_extra = 0;
}

void randomize_particle(pobj *p) {
	p->x = p->x + ((rand_float() * 0.02f) - 0.01f);
	p->y = p->y + ((rand_float() * 0.02f) - 0.01f);
	p->z_norm = rand_float();
	p->scale_norm_x = rand_float();
	p->scale_norm_y = rand_float();
	unsigned char rr = 62;
	unsigned char gg = 78;
	unsigned char bb = 164;
	rgb rclr = {(float)rr / 255.0f, (float)gg / 255.0f, (float)bb / 255.0f};
	hsv hclr = rgb2hsv(rclr);
	hclr.v += ((rand_float() * 0.1f) - 0.05f);
	if (hclr.v > 1.0f) hclr.v = 1.0f;
	if (hclr.v < 0.0f) hclr.v = 0.0f;
	rgb aclr = hsv2rgb(hclr);
	p->r = aclr.r;
	p->g = aclr.g;
	p->b = aclr.b;
}

void update_particle(pobj *p) {
	p->z_norm = p->z_norm + 0.01f;
	if (p->z_norm > 1.0f) p->z_norm = 0.0f;
	p->scale_norm_x = p->scale_norm_x + 0.02f;
	if (p->scale_norm_x > 1.0f) p->scale_norm_x = 0.0f;
	p->scale_norm_y = p->scale_norm_y + 0.02f;
	if (p->scale_norm_y > 1.0f) p->scale_norm_y = 0.0f;
}

timespec time_diff(timespec start, timespec end)
{
	timespec temp;
	if ((end.tv_nsec-start.tv_nsec)<0) {
		temp.tv_sec = end.tv_sec-start.tv_sec-1;
		temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
	} else {
		temp.tv_sec = end.tv_sec-start.tv_sec;
		temp.tv_nsec = end.tv_nsec-start.tv_nsec;
	}
	return temp;
}

bool block_at(int x, int y, int layer) {
	int off = (layer * level_w * level_h)+(((level_h-1)-y)*level_w)+x;
	if (off < 0 || off >= (level_w * level_h * level_layers)) return true;
	return (level[off] != 0);
}

int blat(int x, int y, int layer) {
	if (x < 0 || x >= level_w) return 0;
	if (y < 0 || y >= level_h) return 0;
	if (layer < 0 || layer >= level_layers) return 0;
	int off = (layer * level_w * level_h)+(((level_h-1)-y)*level_w)+x;
	if (off < 0 || off >= (level_w * level_h * level_layers)) return 0;
	return (level[off] != 0) ? 1 : 0;
}

int line_idx_for(int x, int y, int l) {
	return (blat(x-1, y, l) * LEFT) +
	       (blat(x, y+1, l) * ABOVE) +
	       (blat(x+1, y, l) * RIGHT) +
	       (blat(x, y-1, l) * BELOW) +
	       (blat(x, y, l-1) * FRONT) +
	       (blat(x, y, l+1) * BACK) +
	       192;
}

bool collides_with(aabb *b, int x, int y, int layer) {
	if (!block_at(x, y, layer)) return false;
	aabb sb;
	aabb_for(x, y, &sb);
	return collides(b, &sb);
}

int handle_horz_collision(dobj *k, float *fixx) {
	int coll = 0;
	// these are used to determine what blocks to check in the grid
	int kx = (int)floorf(k->x+0.5f);
	int ky = (int)floorf(k->y+0.5f);
	aabb kb = {k->x, k->y, k->rx, k->ry};
	bool u = block_at(kx, ky+1, collision_layer);
	bool d = block_at(kx, ky-1, collision_layer);

	bool ul = collides_with(&kb, kx-1, ky+1, collision_layer);
	bool ur = collides_with(&kb, kx+1, ky+1, collision_layer);
	bool dl = collides_with(&kb, kx-1, ky-1, collision_layer);
	bool dr = collides_with(&kb, kx+1, ky-1, collision_layer);
	bool l = collides_with(&kb, kx-1, ky, collision_layer);
	bool r = collides_with(&kb, kx+1, ky, collision_layer);

	if (l || (!u && ul) || (!d && dl)) {
		coll = (COLLIDE_LEFT);
		*fixx = (float)kx;//(kx - 1)+(0.5f + k->rx);
		//printf("COLLIDE_LEFT\n");
	}
	if (r || (!u && ur) || (!d && dr)) {
		coll = (COLLIDE_RIGHT);
		*fixx = (float)kx;//(kx + 1)-(0.5f + k->rx);
		//printf("COLLIDE_RIGHT\n");
	}
	//if (coll == 0) printf("COLLIDE_NONE\n");
	return coll;
}

int handle_vert_collision(dobj *k, float *fixy) {
	int coll = 0;
	// these are used to determine what blocks to check in the grid
	int kx = (int)floorf(k->x+0.5f);
	int ky = (int)floorf(k->y+0.5f);
	aabb kb = {k->x, k->y, k->rx, k->ry};
	bool ul = collides_with(&kb, kx-1, ky+1, collision_layer);
	bool u = collides_with(&kb, kx, ky+1, collision_layer);
	bool ur = collides_with(&kb, kx+1, ky+1, collision_layer);
	bool dl = collides_with(&kb, kx-1, ky-1, collision_layer);
	bool d = collides_with(&kb, kx, ky-1, collision_layer);
	bool dr = collides_with(&kb, kx+1, ky-1, collision_layer);

	if (k->vy > 0.0f && (u || ul || ur)) {
		coll = (coll | COLLIDE_ABOVE);
		*fixy = (float)(ky + 1)-(0.5f + k->ry);
		//printf("COLLIDE_ABOVE\n");
	}
	if (k->vy < 0.0f && (d || dl || dr)) {
		coll = (coll | COLLIDE_BELOW);
		*fixy = (float)ky;//(ky - 1)+(0.5f + k->ry);
		//printf("COLLIDE_BELOW %f\n", *fixy);
	}
	//if (coll == 0) printf("COLLIDE_NONE\n");
	return coll;
}

void run_trifle() {
	screen_w = 1024;
	screen_h = 640;
	if (!init_window("test sdl_ogl", screen_w, screen_h)) return;
	print_sdl_gl_attributes();
	level_w = 32;
	level_h = 20;
	level_layers = 5;
	collision_layer = 1;
	int num_boids = 50;
	level = level_trifle;
	// setup physics - use the defaults
	phys_def phys;
	init_default_phys(&phys);
	// check physics - last second the player can terminate the jump
	float tt = phys.jump_time - ((2 * (phys.max_jump_h - phys.min_jump_h)) / (phys.v_jump + phys.v_term_jump) );
	printf("g is %f\n", phys.g);
	printf("v_jump is %f\n", phys.v_jump);
	printf("v_term_jump is %f\n", phys.v_term_jump);
	printf("abort is %f\n", tt);

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

	// initialize the screen view
	screen_def sd;
	init_screen(&sd, screen_w, screen_h, 32, true);
	//glViewport(-32, -32, 512, 320);
	print_screen_def(&sd);

	const char *config_filename = "/Users/dmk/code/pectin/config/render_defs.cfg";
	// setup rendering for particles (water and boids)
	render_def *part_def = load_render_def(config_filename, "particles", (GLfloat *)&sd.vp_mat, NULL, NULL);
	// set up rendering for the player character
	render_def *trif_def = load_render_def(config_filename, "player", (GLfloat *)&sd.vp_mat, NULL, NULL);
	// set up rendering for blocks
	render_def *block_def = load_render_def(config_filename, "blocks", (GLfloat *)&sd.vp_mat, NULL, NULL);

	// create some particles using the pobj structure (used for non-interactive things)
	pobj parts[360];
	float px = 25.0f;
	float py = 6.2f;
	for (int i=0; i<360; i++) {
		parts[i].x = px;
		parts[i].y = py;
		randomize_particle(&parts[i]);
		px += 0.2f;
		py += 0.2f;
		if (px > 31.0f) px = 25.0f;
		if (py > 8.6f) py = 6.2f;
	}

	// create some boids
	boid_list *bl = create_boids(
		num_boids,
		150.0f, // coziness
		150.0f, // conformity
		100.0f, // attraction
		0.15f, // speed
		0.25f, // repel distance
		0.0f, // min x
		0.0f, // min y
		32.0f, // max x
		16.0f // max y
	);

	// create the player, using the dobj structure (for dynamic objects that have physics and collide)
	dobj trif = {5.4f, 8, 0, 0, 0.499f, 0.499f, 0, 0, false, false, false, true, false, false};

	sprite *s = (sprite *)malloc(sizeof(sprite));
	tile_range lr = {0, 32, 0, 20};
	tile_range tr;
	get_tile_range(&sd, &tr, &lr);
	bool cam_moved = false;
	init_render_environment(false);
	printf("l=%d, r=%d, t=%d, b=%d\n", tr.l, tr.r, tr.t, tr.b);
	for (int l=0; l<level_layers; l++) {
		for (int y=tr.b; y<tr.t; y++) {
			for (int x=tr.l; x<tr.r; x++) {
				int tidx = thing_at(x, y, l);
				if (tidx == 0) continue;
				int ridx = (tidx - 1) / block_def->cols;
				int cidx = (tidx - 1) % block_def->cols;
				sprite_for((float)x, (float)y, l, ridx, cidx, s, block_def);
				render_sprite(block_def, s);
			}
		}
	}
	clock_gettime(CLOCK_REALTIME, &t1);
	int mult = 2;
	int frame = 0;
	while (loop) {
		get_input(kdown, kpress, key_map, &mouse);
		if (kpress[KEY_QUIT]) {
			loop = false;
		} else if (kdown[KEY_DOWN]) {
			sd.cam_pos.y -= 0.25f;
			if (sd.cam_pos.y < 5.0f) sd.cam_pos.y = 5.0f;
			cam_moved = true;
		}
		/*
		if (cam_moved) {
			update_proj_mat(&sd);
			update_view_mat(rb, (GLfloat *)&sd.vp_mat);
			update_view_mat(trb, (GLfloat *)&sd.vp_mat);
			int cnt = 0;
			render_advance(rb);
			get_tile_range(&sd, &tr, &lr);
			//printf("l=%d, r=%d, t=%d, b=%d\n", tr.l, tr.r, tr.t, tr.b);
			for (int x=tr.l; x<tr.r; x++) {
				for (int y=tr.b; y<tr.t; y++) {
					int off = (((level_h-1)-y)*level_w)+x;
					int ridx = level[off];
					sprite_for((float)x, (float)y, (float)cnt*-0.02f, ridx, 0, s, &block_def);
					render_sprite(rb, s);
					cnt++;
				}
			}
			cam_moved = false;
		}
		*/
		clock_gettime(CLOCK_REALTIME, &t2);
		double dt = time_diff_d(t1, t2);
		update_dobj(&phys, &trif, (float) dt, kdown[KEY_LEFT], kdown[KEY_RIGHT], (kdown[KEY_FIRE] || kdown[KEY_UP]),
		            &handle_horz_collision, &handle_vert_collision);
		if (kdown[KEY_LEFT]) {
			trif.row = 4;
			mult = 3;
		} else if (kdown[KEY_RIGHT]) {
			trif.row = 5;
			mult = 3;
		} else {
			trif.row = 2;
			mult = 4;
		}
		trif.col = ((trif.col + 1) % (mult * 10));

		clock_gettime(CLOCK_REALTIME, &t1);

		render_advance(trif_def);
		sprite_for(trif.x, trif.y, 1, trif.row, (trif.col / mult), s, trif_def);
		render_sprite(trif_def, s);
		if ((frame % 2) == 0) update_boids(bl);
		bl->ax = s->x;
		bl->ay = s->y;

		render_advance(part_def);
		for (int i=0; i<360; i++) {
			update_particle(&parts[i]);
			sprite_for_pobj(&parts[i], s, 1, part_def);
			render_sprite(part_def, s);
		}
		for (int i=0; i<bl->num_boids; i++) {
			sprite_for_boid(bl->boids[i], s, 0, part_def);
			render_sprite(part_def, s);
		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		render_buffer(block_def);
		render_buffer(trif_def);
		render_buffer(part_def);
		swap_window();
		frame += 1;
		if (frame == 60) frame = 0;
	}
	free(s);
	free_render_def(block_def);
	free_render_def(trif_def);
	free_render_def(part_def);
}


