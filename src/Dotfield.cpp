#include "Dotfield.h"
#include <flgl/logger.h>
#include <flgl.h>
using namespace glm;
LOG_MODULE(Dotfield);

Dot::Dot(uint32_t u, uint32_t v, uint8_t r, uint8_t g, uint8_t b)
	: x(u), y(v), color{r,g,b}, water(false) {}
Dot::Dot(uint32_t u, uint32_t v, uint8_t r, uint8_t g, uint8_t b, bool w)
	: x(u), y(v), color{r,g,b}, water(w) {}


uint32_t Dotfield::x() const {return _x;}
uint32_t Dotfield::y() const {return _y;}
uint32_t Dotfield::size() const {return _size;}
uint8_t* Dotfield::pixels() const {return _pixels;}
Dotfield::Dotfield(uint32_t x, uint32_t y) : lookup(x,y) {
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
	// if (i < 0 || i > _size) {
	// 	LOG_ERR("accessing pixel buffer at %d, outside of %d size", i, _size); return;
	// }
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
				} else if (d.water && d.x+a > 0 && d.x+a < this->x() && this->lookup.empty(d.x+a,d.y)) {
					// d.x+=a; d.y--;
					this->move_dot(e, d.x+a, d.y);
				}
			}
		}
	}
}

void Dotfield::commit_dots() {
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
	// std::unordered_set<ivec2> debug;
	// size_t debugct = 0;
	// size_t totalct = 0;
	for (auto e : dots.view<Dot>()) {
		// totalct++;
		Dot& d = dots.getComp<Dot>(e);
		color_texture((d.y * x()) + d.x, d.color);
		// { //DEBUG
		// 	if (debug.find({d.x,d.y}) != debug.end()) {
		// 	}
		// 	debug.insert({d.x,d.y});
		// }
	}
	// { //deBUG
	// 	static size_t ab = 0;
	// 	if (totalct != debug.size()) {
	// 		LOG_ERR("dot mismatch! %d total, %d dist", totalct, debug.size());
	// 	}
	// }
	// debug.clear();
}

Dotfield::DotLookup::DotLookup(size_t x, size_t y) : w(x), h(y) {
	lookup = new entID[x*y];
	memset(lookup, 0xFF, x*y*sizeof(entID));
}

Dotfield::DotLookup::~DotLookup() {
	delete [] lookup;
}

static size_t p2i(int x, int y) {
	return (y*256)+x;
}

void Dotfield::DotLookup::set(ivec2 pos, entID dot) {
	// lookup.insert({pos, dot});
	lookup[p2i(pos.x,pos.y)] = dot;
}
void Dotfield::DotLookup::erase(int x, int y) {
	// lookup.erase({x, y});
	lookup[p2i(x,y)] = 0xFFFFFFFFFFFFFFFF;
}
bool Dotfield::DotLookup::empty(int x, int y) {
	// return lookup.end() == lookup.find({x, y});
	return lookup[p2i(x,y)] == 0xFFFFFFFFFFFFFFFF;
}
entID Dotfield::DotLookup::get(int x, int y) {
	// return lookup[{x,y}];
	return lookup[p2i(x,y)];
}
void Dotfield::DotLookup::clear() {
	// lookup.clear();
	memset(lookup, 0xFF, w*h*sizeof(entID));
}


