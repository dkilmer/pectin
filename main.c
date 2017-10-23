#include "window.h"
#include "example_common.h"
#include "example_wiggle.h"
#include "example_nine_slice.h"
#include "example_port_audio.h"
#include "example_pbox.h"
#include "example_throw.h"
#include "example_trifle.h"
#include "example_pingpong.h"
#include "level_editor.h"
#include "example_diffusion.h"


int main(int argc, char *argv[]) {
	//run_trifle();
	//run_wiggle();
	//run_nine_slice();
	//run_throw();
	//run_trifle();
	//run_pbox();
	//run_pingpong();
	//run_level_editor();
	run_diffusion();

	cleanup_window();

	return 0;
}


