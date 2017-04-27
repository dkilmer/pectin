#include <glad/glad.h>
#include "window.h"

void run()
{
	bool loop = true;
	bool kdown[NUM_KEYS];
	bool kpress[NUM_KEYS];
	int key_map[NUM_KEYS];
	set_default_key_map(key_map);

	while (loop) {
		get_input(kdown, kpress, key_map);

		if (kpress[KEY_QUIT]) {
			loop = false;
		}
		if (kdown[KEY_FIRE]) {
			glClearColor(1.0, 0.0, 0.0, 1.0);
		} else {
			glClearColor(0.0, 0.0, 0.0, 1.0);
		}
		glClear(GL_COLOR_BUFFER_BIT);
		swap_window();
	}
}

int main(int argc, char *argv[])
{
	if (!init_window("test sdl_ogl", 800, 600)) {
		return -1;
	}
	print_sdl_gl_attributes();

	run();

	cleanup_window();

	return 0;
}


