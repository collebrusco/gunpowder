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

void Dotfield::move_dot(entID dot, uint32_t x, uint32_t y) {
	DotMove& dm = dots.addComp<DotMove>(dot);
	// dm->from = {dots.getComp<Dot>(dot).x, dots.getComp<Dot>(dot).y};
	dm.to = {x,y};
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
				// d.y--;
				this->move_dot(e, d.x, d.y-1);
			} else {
				int32_t a = rand() & 0x00000002; a--;
				if (d.x-a > 0 && d.x - a < this->x() && this->lookup.empty(d.x-a,d.y-1)) {
					// d.x-=a; d.y--;
				this->move_dot(e, d.x-a, d.y-1);
				} else if (d.x+a > 0 && d.x+a < this->x() && this->lookup.empty(d.x+a,d.y-1)) {
					// d.x+=a; d.y--;
				this->move_dot(e, d.x+a, d.y-1);
				}
			}
		}
	}
}

void Dotfield::commit_dots() {
	// this->lookup.clear();

	// for (auto e : dots.view<Dot>()) {
	// 	auto& d = dots.getComp<Dot>(e);
	// 	if (!dots.tryGetComp<DotMove>(e)) {
	// 		lookup.set({d.x,d.y}, e);
	// 	}
	// }
	for (auto e : dots.view<DotMove>()) {
		auto& d = dots.getComp<Dot>(e);
		auto& dm = dots.getComp<DotMove>(e);
		if (lookup.empty(dm.to.x, dm.to.y)) {
			lookup.erase(d.x, d.y);
			d.x = dm.to.x; d.y = dm.to.y;
			lookup.set({d.x,d.y}, e);
		}
		dots.removeComp<DotMove>(e);
	}

}
#include <unordered_set>
void Dotfield::paint_dots() {
	std::unordered_set<ivec2> debug;
	size_t debugct = 0;
	size_t totalct = 0;
	for (auto e : dots.view<Dot>()) {
		totalct++;
		Dot& d = dots.getComp<Dot>(e);
		color_texture((d.y * x()) + d.x, d.color);
		{ //DEBUG
			if (debug.find({d.x,d.y}) != debug.end()) {
				// LOG_ERR("overlapping dot! fuck! %d", ++debugct);
			}
			debug.insert({d.x,d.y});
		}
	}
	{ //deBUG
		static size_t ab = 0;
		if (totalct != debug.size()) {
			LOG_ERR("dot mismatch! %d total, %d dist", totalct, debug.size());
		}
	}
	debug.clear();
}

void Dotfield::DotLookup::set(ivec2 pos, entID dot) {
	lookup.insert({pos, dot});
}
void Dotfield::DotLookup::erase(int x, int y) {
	lookup.erase({x, y});
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


