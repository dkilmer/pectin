#ifndef PECTIN_LEVEL_EDITOR_H
#define PECTIN_LEVEL_EDITOR_H

#include "render_util.h"

#define BST_NORMAL  0
#define BST_HOVER   1
#define BST_DOWN    2

#define BTN_LEFT    0
#define BTN_RIGHT   1
#define BTN_BRUSH   2
#define BTN_LAYERS  3
#define BTN_SAVE    4
#define NUM_BTNS    5

typedef struct {
	int id;
	line outline;
	line_box fill;
	line_point txt_pos;
	char txt[20];
} button;

void run_level_editor();

#endif //PECTIN_LEVEL_EDITOR_H
