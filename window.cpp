#include "window.h"
#include <glad/glad.h>

SDL_Window *main_window;
SDL_GLContext main_context;

bool init_window(const char *program_name, int w, int h)
{
	// Initialize SDL's Video subsystem
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("SDL_Init failed\n");
		return false;
	}

	// Create our window centered at 512x512 resolution
	main_window = SDL_CreateWindow(program_name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
	                              w, h, SDL_WINDOW_OPENGL);

	// Check that everything worked out okay
	if (!main_window) {
		printf("Unable to create window\n");
		check_sdl_error(__LINE__);
		return false;
	}

	// Create our opengl context and attach it to our window
	main_context = SDL_GL_CreateContext(main_window);

	// Set our OpenGL version.
	// SDL_GL_CONTEXT_CORE gives us only the newer version, deprecated functions are disabled
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	// 3.2 is part of the modern versions of OpenGL, but most video cards whould be able to run it
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	// Turn on double buffering with a 24bit Z buffer.
	// You may need to change this to 16 or 32 for your system
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	// This makes our buffer swap syncronized with the monitor's vertical refresh
	SDL_GL_SetSwapInterval(1);

	// Init GLAD
	if (!gladLoadGL()) {
		printf("glad failed to load\n");
		return -1;
	}

	return true;
}

void cleanup_window()
{
	SDL_GL_DeleteContext(main_context);
	SDL_DestroyWindow(main_window);
	SDL_Quit();
}

void swap_window() {
	SDL_GL_SwapWindow(main_window);
}

int key_map_index_for_key(int keycode, const int *key_map) {
	for (int i=0; i<NUM_KEYS; i++) {
		if (key_map[i] == keycode) {
			return i;
		}
	}
	return -1;
}

void get_input(bool *downs, bool *presses, const int *key_map) {
	SDL_Event e;
	for (int i=0; i<NUM_KEYS; i++) {
		presses[i] = false;
	}
	while (SDL_PollEvent(&e)) {
		//PrintEvent(&e);
		if (e.type == SDL_QUIT) {
			downs[KEY_QUIT] = true;
			presses[KEY_QUIT] = true;
		}
		if (e.type == SDL_KEYDOWN) {
			int idx = key_map_index_for_key(e.key.keysym.sym, key_map);
			if (idx >= 0 && idx < NUM_KEYS) {
				downs[idx] = true;
			}
		} else if (e.type == SDL_KEYUP) {
			int idx = key_map_index_for_key(e.key.keysym.sym, key_map);
			if (idx >= 0 && idx < NUM_KEYS) {
				presses[idx] = true;
				downs[idx] = false;
			}
		}
	}
}

void set_default_key_map(int *key_map) {
	key_map[KEY_QUIT] =  SDLK_ESCAPE;
	key_map[KEY_LEFT] =  SDLK_LEFT;
	key_map[KEY_RIGHT] = SDLK_RIGHT;
	key_map[KEY_UP] =    SDLK_UP;
	key_map[KEY_DOWN] =  SDLK_DOWN;
	key_map[KEY_JUMP] =  SDLK_w;
	key_map[KEY_FIRE] = SDLK_SPACE;
	key_map[KEY_SPECIAL] = SDLK_q;
	key_map[KEY_USE] = SDLK_LGUI;
	key_map[KEY_GRAB] = SDLK_LALT;
	key_map[KEY_SELECT] = SDLK_RETURN;
	key_map[KEY_RESET] = SDLK_r;
}

void set_wasd_key_map(int *key_map) {
	key_map[KEY_QUIT] =  SDLK_ESCAPE;
	key_map[KEY_LEFT] =  SDLK_a;
	key_map[KEY_RIGHT] = SDLK_d;
	key_map[KEY_UP] =    SDLK_w;
	key_map[KEY_DOWN] =  SDLK_s;
	key_map[KEY_JUMP] =  SDLK_UP;
	key_map[KEY_FIRE] = SDLK_RIGHT;
	key_map[KEY_SPECIAL] = SDLK_RALT;
	key_map[KEY_USE] = SDLK_DOWN;
	key_map[KEY_GRAB] = SDLK_LEFT;
	key_map[KEY_SELECT] = SDLK_RETURN;
	key_map[KEY_RESET] = SDLK_SPACE;
}

void check_sdl_error(int line = -1)
{
	const char *error = SDL_GetError();
	if (error != NULL) {
		printf("SDL error: %s\n", error);
		if (line != -1) {
			printf("Line: %d\n", line);
		}
		SDL_ClearError();
	}
}

void print_sdl_gl_attributes()
{
	int value = 0;
	SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &value);
	printf("SDL_GL_CONTEXT_MAJOR_VERSION: %d\n", value);
	SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &value);
	printf("SDL_GL_CONTEXT_MINOR_VERSION: %d\n", value);
}
