#include "pilot.h"
#include <flgl/tools.h>
#include <flgl/logger.h>
#include <glm.h>
#include "../lib/ecs/ECS.h"
#include "../lib/sw/Stopwatch.h"
#include <unordered_map>
using namespace glm;
using std::unordered_map;
using ftime::Stopwatch;
LOG_MODULE(game);

Game::Game() : GameDriver(20) {}

namespace std { template <> struct hash<ivec2> {
std::size_t operator()(const ivec2& obj) const {
	size_t a = std::hash<int>().operator()(obj.y);
	a = (a << (sizeof(size_t)*4)) | (a >> (sizeof(size_t)*4));
	return (std::hash<int>().operator()(obj.x) ^ (a));
}};}

struct Dot {
	Dot(uint32_t u, uint32_t v, uint8_t r, uint8_t g, uint8_t b)
	: x(u), y(v), color{r,g,b} {}
	uint32_t x, y;
	uint8_t color[3];
};

class DotField {
	uint8_t* _pixels;
	uint32_t _x,_y, _size;
public:
	ECS dots;
	uint32_t x() {return _x;}
	uint32_t y() {return _y;}
	uint32_t size() {return _size;}
	uint8_t* pixels() {return _pixels;}
	DotField(uint32_t x, uint32_t y) {
		_x = x; _y = y;_size = x*y;
		_pixels = new uint8_t[3*_size];
	}
	~DotField(){delete [] _pixels;}
	ivec2 mouse_pos() {
		int x = static_cast<int>((window.mouse.pos.x / window.width) * this->x());
		int y = static_cast<int>(((window.height - window.mouse.pos.y) / window.height) * this->y());
		x = max(0,x); y = max(0,y);
		return {x,y};
	}
	void color_texture(uint32_t i, uint8_t * clr) {
		if (i < 0 || i > _size) {
			LOG_ERR("accessing pixel buffer at %d, outside of %d size", i, _size); return;
		}
		i = 3*i;
		memcpy(&_pixels[i], clr, 3);
	}
	template<typename...Args>
	void add_dot(uint32_t x, uint32_t y, Args... args) {
		if (x < 0 || x > this->x() || y < 0 || y > this->y()) {
			LOG_INF("add dot at (%d,%d) not allowed", x, y); return;
		}
		auto e = dots.newEntity();
		dots.addComp<Dot>(e, x, y, args...);
		lookup.set({x,y},e);
	}
	void update() {
		for (auto e : dots.view<Dot>()) {
			Dot& d = dots.getComp<Dot>(e);
			if (((d.y * x()) + d.x) == -1) LOG_ERR("%d,%d = -1u!!!",d.x,d.y);
			color_texture((d.y * x()) + d.x, d.color);
		}
	}

	void clear() {
		for (auto e : dots.view<Dot>()) {
			Dot& d = dots.getComp<Dot>(e);
			uint8_t z[] = {0x00,0x00,0x00,0x00};
			if (((d.y * x()) + d.x) == -1) LOG_ERR("%d,%d = -1c!!!",d.x,d.y);
			color_texture((d.y * x()) + d.x, z);
		}
	}

	class DotLookup {
		unordered_map<ivec2, entID> lookup;
	public:
		void set(ivec2 pos, entID dot) {
			lookup.insert({pos, dot});
		}
		bool empty(int x, int y) {
			return lookup.end() == lookup.find({x, y});
		}
		entID get(int x, int y) {
			return lookup[{x,y}];
		}
		void clear() {
			lookup.clear();
		}
	};
	DotLookup lookup;
};

static DotField df(256, 256);

struct Renderer {
	Mesh<Vt_classic> quad;
	Shader df_shad;
	Texture df_tex;
	Stopwatch timer;
	Renderer() : timer(ftime::MILLISECONDS) {}
	void init() {
		quad = DefaultMeshes::tile<Vt_classic>();
		df_shad = Shader::from_source("passthrough_vert", "tex");
		df_tex.create();
		df_tex.bind();
		df_tex.pixelate();
    	df_tex.alloc(GL_TEXTURE_2D, 0, GL_RGB32F,
                  	df.x(), df.y(), GL_RGB, GL_UNSIGNED_BYTE, df.pixels());
    	df_tex.unbind();
	}
	void buffer_texture() {
		timer.reset_start();
		df_tex.bind();
    	df_tex.alloc(GL_TEXTURE_2D, 0, GL_RGB32F,
                  	 df.x(), df.y(), GL_RGB, GL_UNSIGNED_BYTE, df.pixels());
		double stop = timer.stop_reset();
		df_tex.unbind();
		// LOG_DBG("updating took %3f ms", stop);
	}
	void render() {
		gl.clear();
		df_tex.bind();

		df_shad.bind();
		timer.reset_start();
		gl.draw_mesh(quad);
		double stop = timer.stop_reset();
		df_tex.unbind();
		df_shad.unbind();
		// LOG_DBG("drawing took %3f ms", stop);
	}
};

static Renderer renderer = Renderer();

#include <stdlib.h>

void Game::user_create() {
	renderer.init();
	srand(656789);

	for (int k = 0; k < 30; k++) {
		for (int i = 0; i < 80; i++) {
			int x = (128-40)+i;
			int y = 128-k;
			df.add_dot(x, y, 0xFC, 0xFC, 0xFC);
			// auto e = df.dots.newEntity();
			// df.dots.addComp<Dot>(e, x, y, 0xFC, 0xFC, 0xFC);
			// df.lookup.set({x,y},e);
		}
	}


	// for (int i = 0; i < df.size(); i++) {
	// 	df.color_texture(i, i&0x20 ? 0xFF : 0x00, 0x00, 0x00);
	// }
}

static void input() {
	auto const& mouse = window.mouse;
	if (mouse.left.down) {
		ivec2 mpos = df.mouse_pos();
		for (int i = 0; i < 10; i++) {
			for (int j = 0; j < 10; j++) {
				if ((mpos.x-5+i) < 0 || (mpos.y-5+j) < 0) continue;
				if ((mpos.x-5+i) > df.x() || (mpos.y-5+j) > df.y()) continue;
				if (df.lookup.empty(mpos.x-5+i,mpos.y-5+j))
					df.add_dot(mpos.x-5+i,mpos.y-5+j,0xFE,0xFC,0xFF);
			}
		}
	}
}

void Game::user_tick(size_t ticks, float dt) {

	df.clear();

	for (auto e : df.dots.view<Dot>()) {
		auto& d = df.dots.getComp<Dot>(e);
		if (d.y > 0) {
			if (df.lookup.empty(d.x,d.y-1)){
				d.y--;
			} else {
				int32_t a = rand() & 0x00000002; a--;
				if (d.x-a > 0 && d.x - a < df.x() && df.lookup.empty(d.x-a,d.y-1)) {
					d.x-=a; d.y--;
				} else if (d.x+a > 0 && d.x+a < df.x() && df.lookup.empty(d.x+a,d.y-1)) {
					d.x+=a; d.y--;
				}
			}
		}
	}

	df.lookup.clear();

	for (auto e : df.dots.view<Dot>()) {
		auto& d = df.dots.getComp<Dot>(e);
		df.lookup.set({d.x,d.y},e);
	}


	df.update();

	renderer.buffer_texture();

	static size_t ix = 0;
	static float buff[20];
	buff[ix] = 1./dt;
	ix++;
	if (ix >= 20) {
		float sm = 0;
		for (int j = 0; j < 20; j++) {sm += buff[j];}
		ix = 0;
		LOG_INF("%3f TPS",sm/20.);
	}
}

void Game::user_update(float dt) {
	if (window.keyboard[GLFW_KEY_ESCAPE].pressed) this->close();
	input();

	renderer.render();
	// LOG_INF("%2f FPS", 1./dt);
}

void Game::user_destroy() {
	gl.destroy();
}


