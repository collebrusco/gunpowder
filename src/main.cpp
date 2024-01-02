#include <flgl.h>
#include <flgl/tools.h>
#include <flgl/logger.h>
LOG_MODULE(main);

#include "pilot.h"

static Game game(window);

int main() {
	gl.init();
	glconfig.set_shader_path("src/shaders/");
	window.create("tanks", 1280, 720);
	
	game.create();
	game.start();
	
	return 0;
}
