#ifndef BOIDS_H
#define BOIDS_H

#if defined __cplusplus
extern "C" {
#endif

typedef struct boid {
	float x;
	float y;
	float lx;
	float ly;
	float vx;
	float vy;
	float ang;
	int frame;
} boid;

typedef struct boid_list {
	int num_boids;
	float coziness;
	float conformity;
	float attraction;
	float speed;
	float repel_dist;
	float minx;
	float miny;
	float maxx;
	float maxy;
	float ax; // attractor pos x
	float ay; // attractor pos y
	boid **boids;
} boid_list;

boid_list *create_boids(int num_boids, float coziness, float conformity, float attraction, float speed, float repel_dist, float minx, float miny, float maxx, float maxy);
void randomize_boids(boid_list *bl, float minx, float miny, float maxx, float maxy);
void update_boids(boid_list *bl);

#ifdef __cplusplus
}
#endif

#endif //BOIDS_H
