#include "grid.h"
#include <stdio.h>

// A grid that acts as a sliding window within a larger space.
//
// The basic principle is that each cell the grid holds an index
// into the array of sprite points that will be drawn. Rather than
// copying a bunch of stuff every time the window slides, the grid
// is implemented as a two-dimensional linked list, which allows
// us to detach rows and columns and reattach them on the other
// side of the grid. So the whole thing is allocated once, and
// never needs to allocate again or copy memory.
//
// When the window slides up, for example, the row representing
// the bottom, which is no longer visible, is shifted to the top
// so that those indices can be used to draw the newly visible
// sprites above.
//
// The grid is actually 2 tiles wider and 2 tiles higher than the
// specified dimensions. This way, you can scroll the camera fractions
// of tiles and still render everything you need to render. But
// you need to be careful and render blanks (or skip rendering) when
// gx and gy are out of range (<0, >=full_w, >=full_h).
//
// In this grid, "head" is at the bottom left

grid *create_grid(int w, int h, int full_w, int full_h, int sx, int sy) {
	grid *g = (grid *)malloc(sizeof(grid));
	g->w = w;
	g->h = h;
	g->full_w = full_w;
	g->full_h = full_h;
	// create the first row
	g->head = (tile *)malloc(sizeof(tile));
	tile *ss = g->head;
	ss->thing = 0;
	ss->gx = sx-1;
	ss->gy = sy-1;
	add_cols(ss, w+1);
	// now add the rows
	int cnt = 0;
	while (cnt < (h+1)) {
		// create a new row
		tile *ns = (tile *)malloc(sizeof(tile));
		ns->thing = 0;
		ns->gx = ss->gx;
		ns->gy = ss->gy+1;
		add_cols(ns, w+1);
		// now hook up this row to the previous one
		tile *sss = ss;
		tile *nss = ns;
		while (nss != NULL) {
			sss->next_row = nss;
			nss->prev_row = sss;
			sss = sss->next_col;
			nss = nss->next_col;
		}
		// new row becomes the current row
		ss = ns;
		cnt++;
	}
	g->cols_foot = get_col_foot(g->head);
	g->rows_foot = get_row_foot(g->head);
	return g;
}

void add_cols(tile *s, int num) {
	tile *ss = s;
	ss->prev_col = NULL;
	int cnt = 0;
	while (cnt < num) {
		tile *ns = (tile *)malloc(sizeof(tile));
		ns->thing = 0;
		ns->gx = ss->gx + 1;
		ns->gy = ss->gy;
		ss->next_col = ns;
		ns->prev_col = ss;
		ns->next_col = NULL;
		ns->prev_row = NULL;
		ns->next_row = NULL;
		ss = ns;
		cnt++;
	}
}

tile *get_col_foot(tile *s) {
	tile *ss = s;
	while (ss != NULL) {
		if (ss->next_col == NULL) return ss;
		ss = ss->next_col;
	}
	return NULL;
}

tile *get_row_foot(tile *s) {
	tile *ss = s;
	while (ss != NULL) {
		if (ss->next_row == NULL) return ss;
		ss = ss->next_row;
	}
	return NULL;
}

void destroy_grid(grid *g) {
	tile *h = g->head;
	while (h != NULL) {
		tile *s = h;
		// move h to the next row before we start nuking the row
		h = h->next_row;
		while (s != NULL) {
			tile *d = s;
			// move s to the next col before we nuke the tile
			s = s->next_col;
			free(d);
		}
	}
	free(g);
}

int move_grid_left(grid *g) {
	if (g->head->gx == 0) return 0;
	tile *new_head = g->cols_foot;
	tile *new_cols_foot = new_head->prev_col;
	tile *new_rows_foot = NULL;

	// h walks down the left side, s walks down the right
	tile *h = g->head;
	tile *s = g->cols_foot;
	while (s != NULL) {
		s->prev_col->next_col = NULL;
		s->next_col = h;
		h->prev_col = s;
		s->prev_col = NULL;
		if (s->next_row == NULL) new_rows_foot = s;
		s = s->next_row;
		h = h->next_row;
	}
	g->head = new_head;
	g->cols_foot = new_cols_foot;
	g->rows_foot = new_rows_foot;
	if (g->rows_foot == NULL) {
		printf("ROWS FOOT IS NULL!\n");
	}
	return 1;
}

int move_grid_right(grid *g) {
	if (g->cols_foot->gx >= (g->full_w-1)) return 0;
	tile *new_head = g->head->next_col;
	tile *new_cols_foot = new_head->prev_col;
	tile *new_rows_foot = NULL;

	// h walks down the left side, s walks down the right
	tile *h = g->head;
	tile *s = g->cols_foot;
	while (s != NULL) {
		h->next_col->prev_col = NULL;
		s->next_col = h;
		h->prev_col = s;
		h->next_col = NULL;
		s = s->next_row;
		h = h->next_row;
		if (h != NULL && h->next_row == NULL) new_rows_foot = h->next_col;
	}
	g->head = new_head;
	g->cols_foot = new_cols_foot;
	g->rows_foot = new_rows_foot;
	if (g->rows_foot == NULL) {
		printf("ROWS FOOT IS NULL!\n");
	}
	return 1;
}

int move_grid_down(grid *g) {
	if (g->head->gy == 0) return 0;
	tile *new_head = g->rows_foot;
	tile *new_cols_foot = NULL;
	tile *new_rows_foot = new_head->prev_row;

	// h walks across the top, s walks across the bottom
	tile *h = g->head;
	tile *s = g->rows_foot;
	while (s != NULL) {
		s->prev_row->next_row = NULL;
		s->next_row = h;
		h->prev_row = s;
		if (s->next_col == NULL) new_cols_foot = s;
		s = s->next_col;
		h = h->next_col;
	}
	g->head = new_head;
	g->cols_foot = new_cols_foot;
	g->rows_foot = new_rows_foot;
	return 1;
}

int move_grid_up(grid *g) {
	if (g->rows_foot->gy >= (g->full_h-1)) return 0;
	tile *new_head = g->head->next_row;
	tile *new_cols_foot = NULL;
	tile *new_rows_foot = new_head->prev_row;

	// h walks across the top, s walks across the bottom
	tile *h = g->head;
	tile *s = g->rows_foot;
	while (s != NULL) {
		h->next_row->prev_row = NULL;
		s->next_row = h;
		h->prev_row = s;
		if (h->next_col == NULL) new_cols_foot = h->next_row;
		h->next_row = NULL;
		s = s->next_col;
		h = h->next_col;
	}
	g->head = new_head;
	g->cols_foot = new_cols_foot;
	g->rows_foot = new_rows_foot;
	return 1;
}

void print_grid(grid *g) {
	tile *s = g->head;
	while (s != NULL) {
		tile *cs = s;
		while (cs != NULL) {
			printf("(%02d,%02d) ", cs->gx, cs->gy);
			cs = cs->next_col;
		}
		printf("\n");
		s = s->next_row;
	}
}

unsigned int tile_count(grid *g) {
	return (unsigned int)((g->w + 2) * (g->h + 2));
}
