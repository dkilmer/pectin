#include <stdlib.h>
#include <math.h>
#include "boids.h"
#include "misc_util.h"


boid_list *create_boids(int num_boids, float coziness, float conformity, float attraction, float speed, float repel_dist, float minx, float miny, float maxx, float maxy) {
	boid_list *bl = (boid_list *)malloc(sizeof(boid_list));
	bl->num_boids = num_boids;
	bl->coziness = coziness;
	bl->conformity = conformity;
	bl->attraction = attraction;
	bl->repel_dist = repel_dist;
	bl->speed = speed;
	bl->boids = (boid **)malloc(sizeof(boid *) * num_boids);
	bl->minx = minx;
	bl->miny = miny;
	bl->maxx = maxx;
	bl->maxy = maxy;
	for (int i=0; i<num_boids; i++) {
		bl->boids[i] = (boid *)malloc(sizeof(boid));
	}
	randomize_boids(bl, minx, miny, maxx, maxy);
	return bl;
}

void randomize_boids(boid_list *bl, float minx, float miny, float maxx, float maxy) {
	for (int i=0; i<bl->num_boids; i++) {
		boid *b = bl->boids[i];
		b->x = (rand_float() * (maxx - minx)) + minx;
		b->y = (rand_float() * (maxy - miny)) + miny;
		b->vx = ((rand_float() * 2.0f) - 1.0f) * bl->speed;
		b->vy = ((rand_float() * 2.0f) - 1.0f) * bl->speed;
		b->frame = rand_int(6);
	}
}

void calc_centroid_rule(boid_list *bl, int idx, float *cvx, float *cvy) {
	*cvx = 0;
	*cvy = 0;
	boid *tb = bl->boids[idx];
	for (int i=0; i<bl->num_boids; i++) {
		if (i == idx) continue;
		boid *b = bl->boids[i];
		*cvx += b->x;
		*cvy += b->y;
	}
	*cvx = *cvx / ((float)(bl->num_boids-1));
	*cvy = *cvy / ((float)(bl->num_boids-1));
	*cvx = *cvx - tb->x;
	*cvy = *cvy - tb->y;
	*cvx = *cvx / bl->coziness;
	*cvy = *cvy / bl->coziness;
}

void calc_repel_rule(boid_list *bl, int idx, float *rvx, float *rvy) {
	*rvx = 0;
	*rvy = 0;
	boid *tb = bl->boids[idx];
	for (int i=0; i<bl->num_boids; i++) {
		if (i == idx) continue;
		boid *b = bl->boids[i];
		float dist = sqrtf(((b->x-tb->x)*(b->x-tb->x))+((b->y-tb->y)*(b->y-tb->y)));
		if (dist < bl->repel_dist) {
			*rvx = *rvx-(b->x-tb->x);
			*rvy = *rvy-(b->y-tb->y);
		}
	}
}

void calc_vel_match_rule(boid_list *bl, int idx, float *mvx, float *mvy) {
	*mvx=0;
	*mvy=0;
	boid *tb = bl->boids[idx];
	for (int i=0; i<bl->num_boids; i++) {
		if (i == idx) continue;
		boid *b = bl->boids[i];
		*mvx = *mvx+b->vx;
		*mvy = *mvy+b->vy;
	}
	float cnt = (float)bl->num_boids;
	*mvx = *mvx / cnt;
	*mvy = *mvy / cnt;

	*mvx = (*mvx-tb->vx) / bl->conformity;
	*mvy = (*mvy-tb->vy) / bl->conformity;
}

void calc_attractor_rule(boid_list *bl, int idx, float *avx, float *avy) {
	*avx=bl->ax;
	*avy=bl->ay;
	boid *b = bl->boids[idx];
	*avx = *avx - b->x;
	*avy = *avy - b->y;
	*avx = *avx / bl->attraction;
	*avy = *avy / bl->attraction;
}

void update_boids(boid_list *bl) {
	float cvx = 0, cvy = 0;
	float rvx = 0, rvy = 0;
	float mvx = 0, mvy = 0;
	float avx = 0, avy = 0;
	for (int i=0; i<bl->num_boids; i++) {
		boid *b = bl->boids[i];
		b->lx = b->x;
		b->ly = b->y;
		calc_centroid_rule(bl, i, &cvx, &cvy);
		calc_repel_rule(bl, i, &rvx, &rvy);
		calc_vel_match_rule(bl, i, &mvx, &mvy);
		calc_attractor_rule(bl, i, &avx, &avy);
		b->vx = b->vx + cvx + rvx + mvx + avx;
		b->vy = b->vy + cvy + rvy + mvy + avy;
		if (b->x < bl->minx) {
			b->vx = bl->speed;
		} else if (b->x > bl->maxx) {
			b->vx = -bl->speed;
		}
		if (b->y < bl->miny) {
			b->vy = bl->speed;
		} else if (b->y > bl->maxy) {
			b->vy = -bl->speed;
		}
		if (fabsf(b->vx) > bl->speed) b->vx = (b->vx / fabsf(b->vx)) * bl->speed;
		if (fabsf(b->vy) > bl->speed) b->vy = (b->vy / fabsf(b->vy)) * bl->speed;
		b->x = b->x + b->vx;
		b->y = b->y + b->vy;
		//b->ang = atan2f((b->lx-b->x), (b->ly-b->y));
		b->ang = atan2f(b->vy, b->vx);
		b->frame++;
		if (b->frame > 5) b->frame = 0;
	}
}