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

void Dotfield::erase_dots() {
	for (auto e : dots.view<Dot>()) {
		Dot& d = dots.getComp<Dot>(e);
		uint8_t z[] = {0x00,0x00,0x00,0x00};
		color_texture((d.y * x()) + d.x, z);
	}
}

void Dotfield::update_dots() {
	for (auto e : this->dots.view<Dot>()) {
		auto& d = this->dots.getComp<Dot>(e);
		if (d.y > 0) {
			if (this->lookup.empty(d.x,d.y-1)){
				d.y--;
			} else {
				int32_t a = rand() & 0x00000002; a--;
				if (d.x-a > 0 && d.x - a < this->x() && this->lookup.empty(d.x-a,d.y-1)) {
					d.x-=a; d.y--;
				} else if (d.x+a > 0 && d.x+a < this->x() && this->lookup.empty(d.x+a,d.y-1)) {
					d.x+=a; d.y--;
				}
			}
		}
	}
}

void Dotfield::commit_dots() {
	this->lookup.clear();

	std::unordered_set<glm::ivec2> debugset;
	uint32_t debugct = 0;

	for (auto e : this->dots.view<Dot>()) {
		auto& d = this->dots.getComp<Dot>(e);
		if (debugset.find({d.x,d.y}) != debugset.end()) {
			LOG_ERR("overlapping dots! %d", ++debugct);
		}
		debugset.insert({d.x,d.y});
		this->lookup.set({d.x,d.y},e);
	}
}

void Dotfield::paint_dots() {
	for (auto e : dots.view<Dot>()) {
		Dot& d = dots.getComp<Dot>(e);
		color_texture((d.y * x()) + d.x, d.color);
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


