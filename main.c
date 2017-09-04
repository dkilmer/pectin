#include "window.h"
#include "example_common.h"
#include "example_wiggle.h"
#include "example_nine_slice.h"
#include "example_port_audio.h"
#include "example_box.h"
#include "example_throw.h"
#include "example_trifle.h"


int main(int argc, char *argv[]) {
	if (!init_window("test sdl_ogl", SCREEN_W, SCREEN_H)) {
		return -1;
	}
	print_sdl_gl_attributes();

	//run_trifle();
	//run_wiggle();
	//run_nine_slice();
	//run_throw();
	//run_trifle();
	run_box();

	cleanup_window();

	return 0;
}


