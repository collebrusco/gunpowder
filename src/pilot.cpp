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

DotRunner::DotRunner() : GameDriver(45) {}

static Dotfield df(256, 256);

static DotRenderer renderer = DotRenderer();

#include <stdlib.h>
#include "util/Stepper.h"
void DotRunner::user_create() {
	renderer.init();
	srand(656789);
	auto e = df.dots.newEntity();
	df.dots.addComp<Dot>(e, 0, 0, 0, 0, 0);
	df.dots.removeEntity(e);
	// for (int k = 0; k < 128; k++) {
	// 	for (int i = 0; i < 128; i++) {
	// 		int x = (128-64)+i;
	// 		int y = 200-k;
	// 		df.add_dot(x, y, 0xFC, 0xFC, 0xFC,DP_NONE|DP_DOWN|DP_DOWN_SIDE);
	// 	}
	// }
	// for (auto pos : Stepper(64, 64, 200, 166)) {
	// 	df.add_dot(pos.x, pos.y,0x7F -(rand()&0xF),0x7F -(rand()&0xF),0x7F -(rand()&0xF));
	// }
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
		brush_size = 33;

	static uint8_t r = 0xFC, g = 0xF8, b = 0xFF;
	static uint8_t sel = 0;
	if (window.keyboard[GLFW_KEY_H].pressed) {
		r = g = b = 0x2F; sel = 0;
	}
	else if (window.keyboard[GLFW_KEY_J].pressed) {
		r = 0xFE; g = 0xFC; b = 0xFF; sel = 1;
	}
	else if (window.keyboard[GLFW_KEY_K].pressed) {
		r = 0x12; g = 0x0F; b = 0xFF; sel = 2;
	}
	else if (window.keyboard[GLFW_KEY_L].pressed) {
		r = g = b = 0x3C; sel = 3;
	}

	if (mouse.left.pressed && window.keyboard[GLFW_KEY_V].down) {df.explode(df.mouse_pos().x,df.mouse_pos().y,30,50);return;}

	if (mouse.left.down && !window.keyboard[GLFW_KEY_V].down) {
		if (window.keyboard[GLFW_KEY_SPACE].down) {
			ivec2 mpos = df.mouse_pos();
			for (int i = 0; i < brush_size; i++) {
				for (int j = 0; j < brush_size; j++) {
					if ((mpos.x-(brush_size/2)+i) < 0 || (mpos.y-(brush_size/2)+j) < 0) continue;
					if ((mpos.x-(brush_size/2)+i) > df.x() || (mpos.y-(brush_size/2)+j) > df.y()) continue;
					if (!df.lookup.empty(mpos.x-(brush_size/2)+i,mpos.y-(brush_size/2)+j))
						df.kill_dot((mpos.x-(brush_size/2)+i), (mpos.y-(brush_size/2)+j));
				}
			}
		} else {
		ivec2 mpos = df.mouse_pos();
			for (int i = 0; i < brush_size; i++) {
				for (int j = 0; j < brush_size; j++) {
					if ((mpos.x-(brush_size/2)+i) < 0 || (mpos.y-(brush_size/2)+j) < 0) continue;
					if ((mpos.x-(brush_size/2)+i) > df.x() || (mpos.y-(brush_size/2)+j) > df.y()) continue;
					if (df.lookup.empty(mpos.x-(brush_size/2)+i,mpos.y-(brush_size/2)+j)) {
						auto e = df.add_dot(mpos.x-(brush_size/2)+i,mpos.y-(brush_size/2)+j,r -(rand()&0xF),g -(rand()&0xF),b -(rand()&0xF));
						if (sel) df.dots.addComp<DotMovable>(e,10, mpos.x-(brush_size/2)+i,mpos.y-(brush_size/2)+j, window.keyboard[GLFW_KEY_N].down ? vec2(-4.,-4.) : vec2(0.,0.));
						// df.dots.addComp<DotResist>(e, prop == DP_NONE ? 100 : 10);
						// df.add_dot_type(dtype, mpos.x-(brush_size/2)+i,mpos.y-(brush_size/2)+j);
					}
				}
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
static Stopwatch tm(ftime::MILLISECONDS);
static Stopwatch ttm(ftime::MILLISECONDS);
void DotRunner::user_tick(size_t ticks, float dt) {
	ttm.reset_start();
	tm.reset_start();

	df.apply_gravity(ticks, dt);

	df.update_dots(ticks, dt);
	// LOG_DBG("update: %fms",tm.stop_reset_start());

	df.erase_dots();
	// LOG_DBG("erase: %fms",tm.stop_reset_start());

	df.commit_dots();
	// LOG_DBG("commit: %fms",tm.stop_reset_start());

	df.paint_dots();
	// LOG_DBG("paint: %fms",tm.stop_reset_start());

	renderer.buffer_texture(df);

	// LOG_DBG("buffer: %fms",tm.stop_reset_start());

	// LOG_DBG("TOTAL: %fms\n========",ttm.stop_reset_start());

	TPS(dt);
}

void DotRunner::user_update(float dt) {
	if (window.keyboard[GLFW_KEY_ESCAPE].pressed) this->close();
	input();
	tm.reset_start();
	renderer.render();
	// LOG_DBG("render: %fms",tm.stop_reset_start());
}

void DotRunner::user_destroy() {
	gl.destroy();
}


