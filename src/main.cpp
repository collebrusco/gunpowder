#include <flgl.h>
#include <flgl/tools.h>
#include <flgl/logger.h>
LOG_MODULE(main);

#include "pilot.h"

static DotRunner game;

int main() {
	LOG_DBG("in main");
	gl.init();
	LOG_DBG("gl init");
	glconfig.set_shader_path("src/shaders/");
	window.create("tanks", 768, 768);
	LOG_DBG("window created");

	game.create();
	game.start();
	LOG_DBG("game starteD");
	
	return 0;
}
