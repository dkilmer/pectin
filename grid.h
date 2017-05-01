#ifndef GRID_H
#define GRID_H

#include <stdlib.h>

#if defined __cplusplus
extern "C" {
#endif

struct tile {
	int thing;
	int gx;
	int gy;
	int buf_idx;

	struct tile *prev_col;
	struct tile *next_col;
	struct tile *prev_row;
	struct tile *next_row;
};

typedef struct tile tile;

typedef struct grid {
	int w;        // the number of tiles that will be visible across the screen
	int h;        // the number of tiles that will be visible up and down the screen
	int full_w;   // the total number of tiles across the level
	int full_h;   // the total number of tiles up and down the level
	tile *head;
	tile *rows_foot;
	tile *cols_foot;
} grid;

grid * create_grid(int w, int h, int full_w, int full_h, int sx, int sy);
void add_cols(tile *s, int num);
tile *get_col_foot(tile *s);
tile *get_row_foot(tile *s);
void destroy_grid(grid *g);
int move_grid_left(grid *g);
int move_grid_right(grid *g);
int move_grid_up(grid *g);
int move_grid_down(grid *g);
void print_grid(grid *g);
unsigned int tile_count(grid *g);

#ifdef __cplusplus
}
#endif

#endif //GRID_H
