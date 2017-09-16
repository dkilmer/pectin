#include "example_common.h"

//int screen_w = 1024;
//int screen_h = 640;

int font_space[] = {
	6,2,4,6,6,7,7,2,4,4,6,6,3,6,2,6,6,4,6,6,6,6,6,6,
	6,6,2,3,6,6,6,6,7,6,6,6,6,6,6,6,6,6,6,6,6,8,7,6,
	6,6,6,6,6,6,8,8,6,6,6,4,6,4,6,7,5,6,6,6,6,6,6,6,
	6,2,5,6,2,8,6,6,6,6,6,6,6,6,6,8,6,6,6,4,2,4,6,8
};

int font_back[] = {
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,-3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

void sprite_for(float x, float y, int layer, int ridx, int cidx, sprite *s, render_def *rd) {
	s->x = (x + 0.5f + rd->xoff) + (rd->layer_xoff * (float)layer);
	s->y = (y + 0.5f + rd->yoff) + (rd->layer_yoff * (float)layer);
	s->z = ((((level_h-y)*level_w)+x) * -0.0001f) + ((float)layer*-1.0f);
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

double time_diff_d(timespec start, timespec end)
{
	timespec temp;
	if ((end.tv_nsec-start.tv_nsec)<0) {
		temp.tv_sec = end.tv_sec-start.tv_sec-1;
		temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
	} else {
		temp.tv_sec = end.tv_sec-start.tv_sec;
		temp.tv_nsec = end.tv_nsec-start.tv_nsec;
	}
	return (double)temp.tv_sec + ((double)temp.tv_nsec / 1000000000.0);
}

int thing_at(int x, int y, int layer) {
	int off = (layer * level_w * level_h)+(((level_h-1)-y)*level_w)+x;
	if (off < 0 || off >= (level_w * level_h * level_layers)) return 0;
	return level[off];
}
