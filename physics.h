//
// Created by David Kilmer on 5/13/17.
//

#ifndef PHYSICS_H
#define PHYSICS_H

#include "game_util.h"

#define COLLIDE_LEFT    1
#define COLLIDE_RIGHT   2
#define COLLIDE_ABOVE   4
#define COLLIDE_BELOW   8

#if defined __cplusplus
extern "C" {
#endif

typedef struct aabb {
	float x;
	float y;
	float rx;
	float ry;
} aabb;

typedef struct phys_def {
	float jump_time; // the time to reach the top of the jump (constant)
	float max_jump_h; // maximum height of jump (constant)
	float min_jump_h; // minimum height of jump
	float v_jump; // initial jump velocity
	float v_term_jump; // the velocity to set when the player terminates a jump early
	float g; // gravity
	float ha_ground; // horizontal acceleration on ground
	float ha_air; // horizontal acceleration in air
	float v_term_x;
} phys_def;

void init_default_phys(phys_def *phys);
void init_phys(phys_def *phys);
void update_kobj(phys_def *phys, kobj *k, float dt, bool ldown, bool rdown, bool jdown, int (*cb_horz)(kobj *, float *fixx), int (*cb_vert)(kobj *, float *fixy));
bool collides(aabb *a, aabb *b);
bool collides_x(aabb *a, aabb *b);
bool collides_y(aabb *a, aabb *b);

#ifdef __cplusplus
}
#endif

#endif //PHYSICS_H
