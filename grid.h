#ifndef GRID_H
#define GRID_H

#include <stdlib.h>

#if defined __cplusplus
extern "C" {
#endif

struct sprot {
	int thing;
	int gx;
	int gy;
	int buf_idx;

	struct sprot *prev_col;
	struct sprot *next_col;
	struct sprot *prev_row;
	struct sprot *next_row;
};

typedef struct sprot sprot;

typedef struct grid {
	int w;
	int h;
	int r;
	int b;
	sprot *head;
	sprot *rows_foot;
	sprot *cols_foot;
} grid;

grid * create_grid(int w, int h);
void add_cols(sprot *s, int num);
sprot *get_col_foot(sprot *s);
sprot *get_row_foot(sprot *s);
void destroy_grid(grid *g);
int move_grid_left(grid *g);
int move_grid_right(grid *g);
int move_grid_up(grid *g);
int move_grid_down(grid *g);
void print_grid(grid *g);

#ifdef __cplusplus
}
#endif

#endif //GRID_H
