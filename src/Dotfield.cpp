#include "Dotfield.h"
#include <flgl/logger.h>
#include <flgl.h>
using namespace glm;
LOG_MODULE(Dotfield);

Dot::Dot(uint32_t u, uint32_t v, uint8_t r, uint8_t g, uint8_t b)
	: x(u), y(v), color{r,g,b} {}

uint32_t Dotfield::x() const {return _x;}
uint32_t Dotfield::y() const {return _y;}
uint32_t Dotfield::size() const {return _size;}
uint8_t* Dotfield::pixels() const {return _pixels;}
Dotfield::Dotfield(uint32_t x, uint32_t y) {
	_x = x; _y = y; _size = x*y;
	_pixels = new uint8_t[3*_size];
}
Dotfield::~Dotfield(){delete [] _pixels;}

ivec2 Dotfield::mouse_pos() const {
	int x = static_cast<int>((window.mouse.pos.x / window.width) * this->x());
	int y = static_cast<int>(((window.height - window.mouse.pos.y) / window.height) * this->y());
	x = max(0,x); y = max(0,y);
	return {x,y};
}

void Dotfield::color_texture(uint32_t i, uint8_t * clr) {
	if (i < 0 || i > _size) {
		LOG_ERR("accessing pixel buffer at %d, outside of %d size", i, _size); return;
	}
	i = 3*i;
	memcpy(&_pixels[i], clr, 3);
}

void Dotfield::update() {
	for (auto e : dots.view<Dot>()) {
		Dot& d = dots.getComp<Dot>(e);
		// if (((d.y * x()) + d.x) == -1) LOG_ERR("%d,%d = -1u!!!",d.x,d.y);
		color_texture((d.y * x()) + d.x, d.color);
	}
}

void Dotfield::clear() {
	for (auto e : dots.view<Dot>()) {
		Dot& d = dots.getComp<Dot>(e);
		uint8_t z[] = {0x00,0x00,0x00,0x00};
		// if (((d.y * x()) + d.x) == -1) LOG_ERR("%d,%d = -1c!!!",d.x,d.y);
		color_texture((d.y * x()) + d.x, z);
	}
}

void Dotfield::DotLookup::set(ivec2 pos, entID dot) {
	lookup.insert({pos, dot});
}
bool Dotfield::DotLookup::empty(int x, int y) {
	return lookup.end() == lookup.find({x, y});
}
entID Dotfield::DotLookup::get(int x, int y) {
	return lookup[{x,y}];
}
void Dotfield::DotLookup::clear() {
	lookup.clear();
}


