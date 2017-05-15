#include "physics.h"

void init_default_phys(phys_def *phys) {
	phys->jump_time = 0.4f; // the time to reach the top of the jump (constant)
	phys->max_jump_h = 4.0f; // maximum height of jump (constant)
	phys->min_jump_h = 1.0f; // minimum height of jump
	phys->ha_ground = 40.0f; // horizontal acceleration on ground
	phys->ha_air = 40.0f; // horizontal acceleration in air
	phys->v_term_x = 6.0f;
	init_phys(phys);
}

void init_phys(phys_def *phys) {
	phys->g = (2 * phys->max_jump_h) / (phys->jump_time * phys->jump_time);
	phys->v_jump = sqrtf(2 * phys->g * phys->max_jump_h);
	phys->g = -phys->g;
	phys->v_term_jump = sqrtf((phys->v_jump * phys->v_jump) + (2 * phys->g * (phys->max_jump_h - phys->min_jump_h)));
}

void update_kobj(phys_def *phys, kobj *k, float dt, bool ldown, bool rdown, bool jdown, int (*cb_horz)(kobj *, float *fixx), int (*cb_vert)(kobj *, float *fixy)) {
	float goodx, goody;
	if (!jdown) k->can_jump = true;
	if (rdown) {
		k->moving_right = true;
	} else if (k->moving_right) {
		k->moving_right = false;
	}
	if (ldown) {
		k->moving_left = true;
	} else if (k->moving_left) {
		k->moving_left = false;
	}
	float old_vx = k->vx;
	if (k->moving_right) {
		float ha = (k->in_air) ? phys->ha_air : phys->ha_ground;
		k->vx = k->vx + (ha * dt);
		if (k->vx > phys->v_term_x) k->vx = phys->v_term_x;
		//printf("vx is %f\n",k->vx);
	} else if (!k->moving_right && k->vx > 0 && !k->in_air) {
		k->vx = k->vx - (phys->ha_ground * dt);
		if (k->vx < 0) k->vx = 0;
	}
	if (k->moving_left) {
		float ha = (k->in_air) ? phys->ha_air : phys->ha_ground;
		k->vx = k->vx - (ha * (float)dt);
		if (k->vx < -phys->v_term_x) k->vx = -phys->v_term_x;
		//printf("vx is %f\n",k->vx);
	} else if (!k->moving_left && k->vx < 0 && !k->in_air) {
		k->vx = k->vx + (phys->ha_ground * dt);
		if (k->vx > 0) k->vx = 0;
	}

	float x_dist = k->vx * dt;
	k->x += x_dist;

	if (!k->in_air) {
		kobj kk = {k->x, k->y-0.02f, k->vx, k->vy-0.1f, k->rx, k->ry, false, false, false, false, false, false};
		int kk_coll = cb_vert(&kk, &goody);
		if (!(kk_coll & COLLIDE_BELOW)) {
			k->jumping = true;
			k->in_air = true;
			k->cut_jump = false;
			//printf("falling...\n");
			//k->vy = phys->v_jump;
		}
	}


	if (k->can_jump && jdown) {
		if (k->jumping) {
			// continue a jump
		} else {
			// start a jump
			k->jumping = true;
			k->in_air = true;
			k->cut_jump = false;
			k->vy = phys->v_jump;
		}
	} else if (k->jumping && !k->cut_jump) {
		//printf("cutting short jump at y=%f: v=%f, vterm=%f\n", k->y, k->vy, v_term_jump);
		// cut short a jump
		k->cut_jump = true;
		if (k->vy > 0 && phys->v_term_jump < k->vy) {
			k->vy = phys->v_term_jump;
		}
	}
	// apply gravity
	if (k->in_air) {
		float old_v = k->vy;
		k->vy = k->vy +(phys->g * dt);
		//float dist = k->vy * (float)dt;
		float dist = (float)dt * (old_v + (phys->g * dt / 2.0f));
		k->y += dist;
		/*
		if (k->y < 1) {
			k->vy = 0;
			k->y = 1;
			//if (!k->moving_right && !k->moving_left) k->vx = 0;
			k->in_air = false;
			k->jumping = false;
			k->cut_jump = false;
			k->can_jump = false;
		}
		*/
	}
	int h_collision = cb_horz(k, &goodx);
	if ((h_collision & COLLIDE_LEFT) || (h_collision & COLLIDE_RIGHT)) {
		k->vx = 0;
		k->x = goodx;
		k->moving_left = false;
		k->moving_right = false;
	}
	int v_collision = cb_vert(k, &goody);
	if (k->in_air && (v_collision & COLLIDE_BELOW)) {
		k->vy = 0;
		k->y = goody;
		//if (!k->moving_right && !k->moving_left) k->vx = 0;
		k->in_air = false;
		k->jumping = false;
		k->cut_jump = false;
		k->can_jump = false;
	} else if (k->in_air && (v_collision & COLLIDE_ABOVE)) {
		k->vy = 0;
		k->y = goody;
		k->cut_jump = true;
	}
}

bool collides(aabb *a, aabb *b) {
	bool x = fabsf(a->x - b->x) <= (a->rx + b->rx);
	bool y = fabsf(a->y - b->y) <= (a->ry + b->ry);
	return x && y;
}

bool collides_x(aabb *a, aabb *b) {
	return fabsf(a->x - b->x) <= (a->rx + b->rx);
}

bool collides_y(aabb *a, aabb *b) {
	return fabsf(a->y - b->y) <= (a->ry + b->ry);
}

