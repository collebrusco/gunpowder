#include "pilot.h"
#include <flgl/tools.h>
#include <flgl/logger.h>
#include <glm.h>
using namespace glm;
LOG_MODULE(game);

Game::Game(Window& win) : GameDriver(win) {}

static OrthoCamera cam({0,0,1},{0,0,-1},{0,1,0},0.001,1000,10);

static float hmap(float x) {
	return 0.25*sin(x/3.);
}

struct Tank {
	float pos;
	vec2 size;
};

struct Controller {
	Tank* target;
	void attach(Tank& tank) {
		target = &tank;
	}
	void poll(float dt) {
		if (window.keyboard[GLFW_KEY_LEFT].down) target->pos -= (0.5 * dt);
		if (window.keyboard[GLFW_KEY_RIGHT].down) target->pos += (0.5 * dt);
	}
};

static Tank hero = {.pos = 0, .size = {0.5, 0.25}};
static Controller controller;

struct Renderer {
	Mesh<Vt_classic> quad;
	Shader terrain;
	Camera* camera;
	Renderer(Camera* c) {camera = c;}
	void init() {
		quad = DefaultMeshes::tile<Vt_classic>();
		terrain = Shader::from_source("passthrough_vert", "terrain");
	}
	void render() {
		gl.clear();
		terrain.bind();
		terrain.uMat4("uView", camera->view());
		terrain.uMat4("uProj", camera->proj());
		gl.draw_mesh(quad);
	}
};

static Renderer renderer(&cam);

void Game::userCreate() {
	renderer.init();
	controller.attach(hero);
}

void Game::userUpdate(float dt) {
	controller.poll(dt);
	cam.update();
	renderer.render();
}

void Game::userDestroy() {
	gl.destroy();
}


