#include "example_common.h"
#include "example_throw.h"
#include "window.h"
#include "render_util.h"
#include "game_util.h"
#include "physics.h"
#include <chipmunk/chipmunk.h>

int level_throw[] = {
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
};

int static_cnt = 7;
srect statics[] = {
	{1.0,13.0,6.0,14.0},
	{1.0,9.0,7.0,10.0},
	{1.0,4.0,11.0,5.0},
	//{14.0,4.0,20.0,5.0},
	{0.0,0.0,1.0,20.0},
	{31.0,0.0,32.0,20.0},
	{1.0,19.0,31.0,20.0},
	{1.0,0.0,31.0,1.0}
};

/*
int project_trajectory(phys_def *phys, line *proj, float x, float y, float vx, float vy) {
	dobj o = {x, y, vx, vy, 0.499f, 0.499f, 0, 0, false, false, false, true, false, false};
	int ppcnt = 0;
	float lastx = x;
	float lasty = y;
	for (int i=0; i<20; i++) {
		update_np_dobj(phys, &o, (float) 0.05f, &handle_horz_collision, &handle_vert_collision);
		proj[i].p1.x = lastx;
		proj[i].p1.y = lasty;
		proj[i].p1.z = 0.5f;
		proj[i].p1.r = 1.0f;
		proj[i].p1.g = 1.0f;
		proj[i].p1.b = 1.0f;

		proj[i].p2.x = o.x;
		proj[i].p2.y = o.y;
		proj[i].p2.z = 0.5f;
		proj[i].p2.r = 1.0f;
		proj[i].p2.g = 1.0f;
		proj[i].p2.b = 1.0f;

		if (!o.in_air) break;
		lastx = o.x;
		lasty = o.y;
		ppcnt++;
	}
	return ppcnt;
}
*/

static cpBody *add_kbar(cpSpace *space, cpVect a, cpVect b, int group)
{
	cpVect center = cpvmult(cpvadd(a, b), 1.0f/2.0f);
	cpFloat length = cpvlength(cpvsub(b, a));

	cpBody *body = cpSpaceAddBody(space, cpBodyNewKinematic());
	cpBodySetPosition(body, center);
	cpBodySetAngle(body, 0);

	//cpShape *shape = cpSpaceAddShape(space, cpSegmentShapeNew(body, a, b, 0.1f));
	cpVect av = cpvsub(a, center);
	cpVect bv = cpvsub(b, center);
	cpShape *shape = cpSpaceAddShape(space, cpSegmentShapeNew(body, av, bv, 0.0f));
	cpShapeSetFilter(shape, cpShapeFilterNew(group, CP_ALL_CATEGORIES, CP_ALL_CATEGORIES));

	return body;
}

static cpBody *add_bar(cpSpace *space, cpVect a, cpVect b, int group)
{
	cpVect center = cpvmult(cpvadd(a, b), 1.0f/2.0f);
	cpFloat length = cpvlength(cpvsub(b, a));
	cpFloat mass = length;

	cpBody *body = cpSpaceAddBody(space, cpBodyNew(mass, mass*length*length/1.2f));
	cpBodySetPosition(body, center);
	cpBodySetAngle(body, 0);

	cpVect av = cpvsub(a, center);
	cpVect bv = cpvsub(b, center);
	cpShape *shape = cpSpaceAddShape(space, cpSegmentShapeNew(body, av, bv, 0.0f));
	cpShapeSetFilter(shape, cpShapeFilterNew(group, CP_ALL_CATEGORIES, CP_ALL_CATEGORIES));

	return body;
}

void line_for(cpVect pos, cpFloat ang, cpFloat r, line *l) {
	// position as if about the origin
	cpFloat x = (r * sin(ang));
	cpFloat y = (r * cos(ang));
	l->p1.x = (float)(x + pos.x);
	l->p1.y = (float)(y + pos.y);
	l->p2.x = (float)((x * -1.0) + pos.x);
	l->p2.y = (float)((y * -1.0) + pos.y);
}


void run_throw() {
	screen_w = 1024;
	screen_h = 640;
	if (!init_window("test sdl_ogl", screen_w, screen_h)) return;
	print_sdl_gl_attributes();
	cpSpace *space = cpSpaceNew();
	cpSpaceSetIterations(space, 10);
	cpSpaceSetGravity(space, cpv(0, -30));
	cpSpaceSetSleepTimeThreshold(space, 0.5f);

	cpBody *staticBody = cpSpaceGetStaticBody(space);

	for (int i=0; i<static_cnt; i++) {
		cpVect verts[] = {
			{statics[i].x1,statics[i].y1},
			{statics[i].x1,statics[i].y2},
			{statics[i].x2,statics[i].y2},
			{statics[i].x2,statics[i].y1}
		};
		cpSpaceAddShape(space, cpPolyShapeNewRaw(staticBody, 4, verts, 0.0f));
	}

	int num_bodies = 6;
	cpBody *last_body = staticBody;
	cpVect start_pos = cpv(20, 1);
	cpBody **plant = (cpBody **)malloc(sizeof(cpBody *) * num_bodies);
	for (int i=0; i<num_bodies; i++) {
		cpFloat cur_y = start_pos.y + ((cpFloat)i);
		plant[i] = add_bar(space, cpv(start_pos.x, cur_y), cpv(start_pos.x, cur_y+1), 1);
		cpSpaceAddConstraint(space, cpPivotJointNew(plant[i],  last_body, cpv(start_pos.x, cur_y)));
		cpSpaceAddConstraint(space, cpDampedRotarySpringNew(plant[i], last_body, 0, 2000.0f, 10.0f));
		last_body = plant[i];
	}

	level_w = 32;
	level_h = 20;
	level_layers = 1;
	collision_layer = 0;
	level = level_throw;
	font_kern = 1;
	// setup physics - use the defaults
	phys_def phys;
	init_default_phys(&phys);
	// check physics - last second the player can terminate the jump
	float tt = phys.jump_time - ((2 * (phys.max_jump_h - phys.min_jump_h)) / (phys.v_jump + phys.v_term_jump) );
	printf("g is %f\n", phys.g);
	printf("v_jump is %f\n", phys.v_jump);
	printf("v_term_jump is %f\n", phys.v_term_jump);
	printf("abort is %f\n", tt);

	// variables for timing used in physics
	timespec t1, t2;
	bool loop = true;

	// keyboard event variables
	bool kdown[NUM_KEYS];
	bool kpress[NUM_KEYS];
	for (int i=0; i<NUM_KEYS; i++) {
		kdown[i] = false;
		kpress[i] = false;
	}
	int key_map[NUM_KEYS];
	set_default_key_map(key_map);
	mouse_input mouse;
	float wmx = 0.0f;
	float wmy = 0.0f;

	// initialize the screen view
	screen_def sd;
	init_screen(&sd, screen_w, screen_h, 32, true);
	//glViewport(-32, -32, 512, 320);
	print_screen_def(&sd);

	const char *config_filename = "/Users/dmk/code/pectin/config/throw_render_defs.cfg";
	// set up rendering for the player character
	render_def *ball_def = load_render_def(config_filename, "ball", (GLfloat *)&sd.vp_mat, NULL, NULL);
	// set up rendering for blocks
	render_def *block_def = load_render_def(config_filename, "blocks", (GLfloat *)&sd.vp_mat, NULL, NULL);
	// set up rendering for lines
	render_def *line_def = load_render_def(config_filename, "lines", (GLfloat *)&sd.vp_mat, NULL, NULL);
	// set up rendering for font
	render_def *font_def = load_render_def(config_filename, "font", (GLfloat *)&sd.vp_mat, NULL, NULL);

	// create the player, using the dobj structure (for dynamic objects that have physics and collide)
	dobj ball = {16, 8, 0, 0, 0.499f, 0.499f, 2, 0, false, false, false, true, false, false};

	cpFloat mass = 1.0f;
	cpFloat r = 0.5f;
	cpBody *body = cpSpaceAddBody(space, cpBodyNew(mass, cpMomentForCircle(mass, 0.0f, r, cpvzero)));
	cpBodySetPosition(body, cpv(16.5f, 8.5f));
	cpSpaceAddShape(space, cpCircleShapeNew(body, r, cpvzero));

	line ln = {{16, 8, 0, 0.0f, 0.5f, 0.0f}, {8, 12, 0, 0.0f, 0.5f, 0.0f}};

	//const char *txt = "This is an [example] of some {text}. Who knows if it has mojo?";
	const char *txt = "This is some text that will be on the screen";
	float txt_x = 1.0f;
	float txt_y = 18.0f;
	int txt_len = (int)strlen(txt);

	printf("sizeof(line) is %d\n", (int)sizeof(line));
	line *project = (line *)malloc(20 * sizeof(line));

	sprite *s = (sprite *)malloc(sizeof(sprite));
	tile_range lr = {0, 32, 0, 20};
	tile_range tr;
	get_tile_range(&sd, &tr, &lr);
	bool cam_moved = false;
	init_render_environment(false);
	for (int y=tr.b; y<tr.t; y++) {
		for (int x=tr.l; x<tr.r; x++) {
			int tidx = thing_at(x, y, 0);
			if (tidx == 0) continue;
			int ridx = (tidx - 1) / block_def->cols;
			int cidx = (tidx - 1) % block_def->cols;
			sprite_for((float)x, (float)y, 0, ridx, cidx, s, block_def);
			render_sprite(block_def, s);
		}
	}

	int advance = 0;
	float tx = txt_x;
	for (int i=0; i<txt_len; i++) {
		int ch = txt[i] - 32;
		int trow = ch / 24;
		int tcol = ch % 24;
		int back = font_back[ch];
		float kern = (((float)font_kern / 8.0f) * font_def->xscale);
		tx += (((float)advance / 8.0f) * font_def->xscale);
		tx += kern;
		if (back < 0) tx += (((float)back / 8.0f) * font_def->xscale);
		sprite_for(tx, txt_y, 0, trow, tcol, s, font_def);
		render_sprite(font_def, s);
		advance = font_space[ch];
	}
	clock_gettime(CLOCK_REALTIME, &t1);
	int mult = 2;
	int frame = 0;
	int proj_cnt = 0;
	while (loop) {
		get_input(kdown, kpress, key_map, &mouse);
		if (kpress[KEY_QUIT]) {
			loop = false;
		}
		cpVect cur_vel = cpBodyGetVelocity(body);
		if (kdown[KEY_LEFT]) {
			cpBodySetVelocity(body, cpv(-5.0, cur_vel.y));
		} else if (kdown[KEY_RIGHT]) {
			cpBodySetVelocity(body, cpv(5.0, cur_vel.y));
		} else if (kdown[KEY_FIRE]) {
			cpBodySetVelocity(body, cpv(cur_vel.x, 10.0));
		}
		clock_gettime(CLOCK_REALTIME, &t2);
		double dt = time_diff_d(t1, t2);
		cpSpaceStep(space, dt);
		//update_dobj(&phys, &ball, (float) dt, kdown[KEY_LEFT], kdown[KEY_RIGHT], (kdown[KEY_FIRE] || kdown[KEY_UP]),
		//            &handle_horz_collision, &handle_vert_collision);
		clock_gettime(CLOCK_REALTIME, &t1);

		render_advance(ball_def);
		cpVect ball_pos = cpBodyGetPosition(body);
		ball.x = (float)(ball_pos.x - 0.5);
		ball.y = (float)(ball_pos.y - 0.5);
		sprite_for(ball.x, ball.y, 1, ball.row, (ball.col / mult), s, ball_def);
		render_sprite(ball_def, s);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		render_advance(line_def);
		ln.p2.x = (float)start_pos.x;
		ln.p2.y = (float)start_pos.y;
		for (int i=0; i<num_bodies; i++) {
			cpVect lpos = cpBodyGetPosition(plant[i]);
			ln.p1.x = ln.p2.x;
			ln.p1.y = ln.p2.y;
			ln.p2.x = (float)lpos.x;
			ln.p2.y = (float)lpos.y;
			render_line(line_def, &ln);
		}
		render_buffer(line_def);

		render_buffer(block_def);
		render_buffer(ball_def);
		render_buffer(font_def);
		swap_window();
		frame += 1;
		if (frame == 60) frame = 0;
	}
	free(s);
	free_render_def(block_def);
	free_render_def(ball_def);
	free_render_def(line_def);
}


