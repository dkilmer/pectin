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

void update_kobj(phys_def *phys, dobj *d, float dt, bool ldown, bool rdown, bool jdown, int (*cb_horz)(dobj *, float *fixx), int (*cb_vert)(dobj *, float *fixy)) {
	float goodx, goody;
	if (!jdown) d->can_jump = true;
	if (rdown) {
		d->moving_right = true;
	} else if (d->moving_right) {
		d->moving_right = false;
	}
	if (ldown) {
		d->moving_left = true;
	} else if (d->moving_left) {
		d->moving_left = false;
	}
	float old_vx = d->vx;
	if (d->moving_right) {
		float ha = (d->in_air) ? phys->ha_air : phys->ha_ground;
		d->vx = d->vx + (ha * dt);
		if (d->vx > phys->v_term_x) d->vx = phys->v_term_x;
		//printf("vx is %f\n",d->vx);
	} else if (!d->moving_right && d->vx > 0 && !d->in_air) {
		d->vx = d->vx - (phys->ha_ground * dt);
		if (d->vx < 0) d->vx = 0;
	}
	if (d->moving_left) {
		float ha = (d->in_air) ? phys->ha_air : phys->ha_ground;
		d->vx = d->vx - (ha * (float)dt);
		if (d->vx < -phys->v_term_x) d->vx = -phys->v_term_x;
		//printf("vx is %f\n",d->vx);
	} else if (!d->moving_left && d->vx < 0 && !d->in_air) {
		d->vx = d->vx + (phys->ha_ground * dt);
		if (d->vx > 0) d->vx = 0;
	}

	float x_dist = d->vx * dt;
	d->x += x_dist;

	if (!d->in_air) {
		dobj kk = {d->x, d->y-0.02f, d->vx, d->vy-0.1f, d->rx, d->ry, false, false, false, false, false, false};
		int kk_coll = cb_vert(&kk, &goody);
		if (!(kk_coll & COLLIDE_BELOW)) {
			d->jumping = true;
			d->in_air = true;
			d->cut_jump = false;
			//printf("falling...\n");
			//d->vy = phys->v_jump;
		}
	}


	if (d->can_jump && jdown) {
		if (d->jumping) {
			// continue a jump
		} else {
			// start a jump
			d->jumping = true;
			d->in_air = true;
			d->cut_jump = false;
			d->vy = phys->v_jump;
		}
	} else if (d->jumping && !d->cut_jump) {
		//printf("cutting short jump at y=%f: v=%f, vterm=%f\n", d->y, d->vy, v_term_jump);
		// cut short a jump
		d->cut_jump = true;
		if (d->vy > 0 && phys->v_term_jump < d->vy) {
			d->vy = phys->v_term_jump;
		}
	}
	// apply gravity
	if (d->in_air) {
		float old_v = d->vy;
		d->vy = d->vy +(phys->g * dt);
		//float dist = d->vy * (float)dt;
		float dist = (float)dt * (old_v + (phys->g * dt / 2.0f));
		d->y += dist;
		/*
		if (d->y < 1) {
			d->vy = 0;
			d->y = 1;
			//if (!d->moving_right && !d->moving_left) d->vx = 0;
			d->in_air = false;
			d->jumping = false;
			d->cut_jump = false;
			d->can_jump = false;
		}
		*/
	}
	int h_collision = cb_horz(d, &goodx);
	if ((h_collision & COLLIDE_LEFT) || (h_collision & COLLIDE_RIGHT)) {
		d->vx = 0;
		d->x = goodx;
		d->moving_left = false;
		d->moving_right = false;
	}
	int v_collision = cb_vert(d, &goody);
	if (d->in_air && (v_collision & COLLIDE_BELOW)) {
		d->vy = 0;
		d->y = goody;
		//if (!d->moving_right && !d->moving_left) d->vx = 0;
		d->in_air = false;
		d->jumping = false;
		d->cut_jump = false;
		d->can_jump = false;
	} else if (d->in_air && (v_collision & COLLIDE_ABOVE)) {
		d->vy = 0;
		d->y = goody;
		d->cut_jump = true;
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

