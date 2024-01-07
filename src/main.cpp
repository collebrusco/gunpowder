#include <flgl.h>
#include <flgl/tools.h>
#include <flgl/logger.h>
LOG_MODULE(main);

#include "pilot.h"
	#include "util/Stepper.h"

static Game game;

int main() {
	// gl.init();
	// glconfig.set_shader_path("src/shaders/");
	// window.create("tanks", 768, 768);

	// std::cout << "steppin 0 0 4 2\n";
	// for (auto vec : Stepper(0, 0, 4, 2)) {
	// 	std::cout << vec.x << ", " << vec.y << "\n";
	// }

	std::cout << "steppin 0 0 4 2\n";
	for (auto vec : Stepper(10, 0, -10, -1)) {
		std::cout << vec.x << ", " << vec.y << "\n";
		std::cin.get();
	}

	// game.create();
	// game.start();
	
	return 0;
}
