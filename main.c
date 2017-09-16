#include "window.h"
#include "example_common.h"
#include "example_wiggle.h"
#include "example_nine_slice.h"
#include "example_port_audio.h"
#include "example_box.h"
#include "example_throw.h"
#include "example_trifle.h"
#include "example_pingpong.h"
#include "level_editor.h"


int main(int argc, char *argv[]) {
	//run_trifle();
	//run_wiggle();
	//run_nine_slice();
	//run_throw();
	//run_trifle();
	run_box();
	//run_pingpong();
	//run_level_editor();

	cleanup_window();

	return 0;
}


