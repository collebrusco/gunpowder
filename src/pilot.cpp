#include "pilot.h"
#include <flgl/tools.h>
#include <flgl/logger.h>
#include <flgl/glm.h>
#include <unordered_map>
#include "../lib/ecs/ECS.h"
#include "../lib/sw/Stopwatch.h"
#include "DotRenderer.h"
#include "Dotfield.h"
using namespace glm;
using std::unordered_map;
using ftime::Stopwatch;
LOG_MODULE(game);

Game::Game() : GameDriver(45) {}

static Dotfield df(256, 256);

static DotRenderer renderer = DotRenderer();

#include <stdlib.h>

void Game::user_create() {
	renderer.init();
	srand(656789);

	for (int k = 0; k < 30; k++) {
		for (int i = 0; i < 80; i++) {
			int x = (128-40)+i;
			int y = 128-k;
			df.add_dot(x, y, 0xFC, 0xFC, 0xFC);
		}
	}
	renderer.buffer_texture(df);
}

// min 2
static void input() {
	static auto const& mouse = window.mouse;
	static uint32_t brush_size = 5;
	if (window.keyboard[GLFW_KEY_1].pressed)
		brush_size = 1;
	else if (window.keyboard[GLFW_KEY_2].pressed)
		brush_size = 2;
	else if (window.keyboard[GLFW_KEY_3].pressed)
		brush_size = 3;
	else if (window.keyboard[GLFW_KEY_4].pressed)
		brush_size = 4;
	else if (window.keyboard[GLFW_KEY_5].pressed)
		brush_size = 5;
	else if (window.keyboard[GLFW_KEY_6].pressed)
		brush_size = 6;
	else if (window.keyboard[GLFW_KEY_7].pressed)
		brush_size = 7;
	else if (window.keyboard[GLFW_KEY_8].pressed)
		brush_size = 8;
	else if (window.keyboard[GLFW_KEY_9].pressed)
		brush_size = 9;

	if (mouse.left.down) {
		ivec2 mpos = df.mouse_pos();
		for (int i = 0; i < brush_size; i++) {
			for (int j = 0; j < brush_size; j++) {
				if ((mpos.x-(brush_size/2)+i) < 0 || (mpos.y-(brush_size/2)+j) < 0) continue;
				if ((mpos.x-(brush_size/2)+i) > df.x() || (mpos.y-(brush_size/2)+j) > df.y()) continue;
				if (df.lookup.empty(mpos.x-(brush_size/2)+i,mpos.y-(brush_size/2)+j))
					df.add_dot(mpos.x-(brush_size/2)+i,mpos.y-(brush_size/2)+j,0xFE,0xFC,0xFF);
			}
		}
	}
}

static void TPS(float dt) {	static size_t ix = 0; static float buff[20];
	buff[ix] = 1./dt; ix++;
	if (ix >= 20) {
		float sm = 0;
		for (int j = 0; j < 20; j++) {sm += buff[j];}
		ix = 0;
		LOG_INF("%1f TPS",sm/20.);
	}
}

void Game::user_tick(size_t ticks, float dt) {

	df.erase_dots();

	df.update_dots();

	df.commit_dots();

	df.paint_dots();

	renderer.buffer_texture(df);

	TPS(dt);
}

void Game::user_update(float dt) {
	if (window.keyboard[GLFW_KEY_ESCAPE].pressed) this->close();
	input();

	renderer.render();
}

void Game::user_destroy() {
	gl.destroy();
}


