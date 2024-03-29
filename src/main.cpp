#include <flgl.h>
#include <flgl/tools.h>
#include <flgl/logger.h>
LOG_MODULE(main);

#include "pilot.h"

static DotRunner game;

int main() {
	gl.init();
	glconfig.set_shader_path("src/shaders/");
	window.create("tanks", 768, 768);

	game.create();
	game.start();
	
	return 0;
}
