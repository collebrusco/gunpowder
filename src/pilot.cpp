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

Game::Game(Window& win) : GameDriver(win) {}

namespace std { template <> struct hash<ivec2> {
std::size_t operator()(const ivec2& obj) const {
	size_t a = std::hash<int>().operator()(obj.y);
	a = (a << (sizeof(size_t)*4)) | (a >> (sizeof(size_t)*4));
	return (std::hash<int>().operator()(obj.x) ^ (a));
}};}

struct Dot {

};

class DotField {
	uint8_t* pixels;
	uint32_t _x,_y, _size;
	Texture tex;
public:
	uint32_t x() {return _x;}
	uint32_t y() {return _y;}
	uint32_t size() {return _size;}
	Texture const& texture() {return tex;}
	DotField(uint32_t x, uint32_t y) {
		_x = x; _y = y;_size = x*y;
		pixels = new uint8_t[3*_size];
	}
	void tinit() {
		tex.create();
		tex.bind();
		tex.pixelate();
    	tex.alloc(GL_TEXTURE_2D, 0, GL_RGB32F,
                  _x, _y, GL_RGB, GL_UNSIGNED_BYTE, pixels);
    	tex.unbind();
	}
	~DotField(){delete [] pixels;}
	void color_texture(uint32_t i, uint8_t r, uint8_t g, uint8_t b) {
		i = 3*i;
		pixels[i  ] = r;
		pixels[i+1] = g;
		pixels[i+2] = b;
	}
	void update_texture() {
		tex.bind();
    	tex.alloc(GL_TEXTURE_2D, 0, GL_RGB32F,
                  _x, _y, GL_RGB, GL_UNSIGNED_BYTE, pixels);
	}

	class DotLookup {
		unordered_map<ivec2, Dot*> lookup;
	public:
		void set(ivec2 pos, Dot * dot) {
			lookup.insert({pos, dot});
		}
		bool empty(int x, int y) {
			return lookup.end() == lookup.find({x, y});
		}
		Dot& get(int x, int y) {
			return *lookup[{x,y}];
		}
	};
};

static DotField df(1280, 720);

struct Renderer {
	Mesh<Vt_classic> quad;
	Shader df_shad;
	Stopwatch timer;
	Renderer() : timer(ftime::MILLISECONDS) {}
	void init() {
		quad = DefaultMeshes::tile<Vt_classic>();
		df_shad = Shader::from_source("passthrough_vert", "tex");
	}
	void render() {
		gl.clear();
		df.texture().bind();

		timer.reset_start();
		df.update_texture();
		double stop = timer.stop_reset();
		LOG_DBG("updating took %3f ms", stop);

		df_shad.bind();
		timer.reset_start();
		gl.draw_mesh(quad);
		stop = timer.stop_reset();
		LOG_DBG("drawing took %3f ms", stop);
	}
};

static Renderer renderer = Renderer();

void Game::userCreate() {
	renderer.init();
	df.tinit();
	for (int i = 0; i < df.size(); i++) {
		df.color_texture(i, i&0x20 ? 0xFF : 0x00, 0x00, 0x00);
	}
}

void Game::userUpdate(float dt) {
	renderer.render();
	LOG_INF("%2f FPS", 1./dt);
}

void Game::userDestroy() {
	gl.destroy();
}


