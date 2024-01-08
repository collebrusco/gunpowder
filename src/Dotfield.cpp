#include "Dotfield.h"
#include <flgl/logger.h>
#include <flgl.h>
using namespace glm;
LOG_MODULE(Dotfield);

uint32_t Dotfield::x() const {return _x;}
uint32_t Dotfield::y() const {return _y;}
uint32_t Dotfield::size() const {return _size;}
uint8_t* Dotfield::pixels() const {return _pixels;}
Dotfield::Dotfield(uint32_t x, uint32_t y) : lookup(x,y) {
	_x = x; _y = y; _size = x*y;
	_pixels = new uint8_t[3*_size];
}
Dotfield::~Dotfield(){delete [] _pixels;}

bool Dotfield::bounds(int x, int y) const {
	return (x > 0) && (x < this->x()) &&
		   (y > 0) && (y < this->y());
}

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

void Dotfield::erase_dot(entID e) {
	Dot& d = dots.getComp<Dot>(e);
	static uint8_t z[] = {0x00,0x00,0x00,0x00};
	color_texture((d.y * x()) + d.x, z);
}

void Dotfield::exptr(int x1, int y1, int x2, int y2, uint32_t r, uint8_t p) {
	for (auto vec : Stepper(x1,y1,x2,y2)) {
		// LOG_DBG("%d,%d",vec.x,vec.y);
		if (!bounds(vec.x, vec.y)) continue;
		int l2 = (vec.x - x1)*(vec.x - x1) + (vec.y - y1)*(vec.y - y1);
		if (l2>((int)r)*((int)r)) return;
		if (!lookup.empty(vec.x,vec.y)) {
			auto e = lookup.get(vec.x, vec.y);
			// auto r = dots.getComp<DotResist>(e).resist;
			// if (r<=p) {
			if (dots.tryGetComp<DotMovable>(e)) {
				kill_dot(e);
			} else return;
		}
	}
}

void Dotfield::explode(int x, int y, int rad, uint8_t pow) {
	int i = x-(rad), j = y-(rad);
	for (;j<y+(rad);j++)
		exptr(x,y,i,j,rad,pow);
	for (;i<x+(rad);i++)
		exptr(x,y,i,j,rad,pow);
	for (;j>=y-(rad);j--)
		exptr(x,y,i,j,rad,pow);
	for (;i>=x-(rad);i--)
		exptr(x,y,i,j,rad,pow);
}

void Dotfield::kill_dot(entID dot) {
	erase_dot(dot);
	auto& d = this->dots.getComp<Dot>(dot);
	this->lookup.erase(d.x, d.y);
	this->dots.removeEntity(dot);
}

void Dotfield::kill_dot(int32_t x, int32_t y) {
	if (this->lookup.empty(x,y)) return; 
	auto e = this->lookup.get(x,y);
	erase_dot(e);
	this->dots.removeEntity(e);
	this->lookup.erase(x, y);
}

void Dotfield::move_dot(entID dot, int32_t x, int32_t y) {
	DotMove& dm = dots.addComp<DotMove>(dot);
	// dm->from = {dots.getComp<Dot>(dot).x, dots.getComp<Dot>(dot).y};
	dm.to = {x,y};
}

void Dotfield::erase_dots() {
	for (auto e : dots.view<DotMove>()) {
		erase_dot(e);
	}
}

void Dotfield::update_dots(size_t ticks, float dt) {
	for (auto e : dots.view<DotMovable>()) {
		auto& dm = dots.getComp<DotMovable>(e);
		dm.force({0.,-((float)dm.mass)}, dt); // gravity
		// air? other updates? if I update dots here that affect others, must finish loop before doing the rest
		auto& d = dots.getComp<Dot>(e);
		dm.pos += dm.velo;
		LOG_DBG("%f,%f", dm.pos.x,dm.pos.y);
		int32_t ix = (int32_t)dm.pos.x;
		int32_t iy = (int32_t)dm.pos.y;
		LOG_DBG("%d,%d", ix, iy);
		if (bounds(ix,iy) && ((ix != d.x) || (iy != d.y))) {
			this->move_dot(e, ix, iy);
		}
	}
}

void Dotfield::commit_dots() {
	for (auto e : dots.view<DotMove>()) {
		auto& d = dots.getComp<Dot>(e);
		auto& dm = dots.getComp<DotMove>(e);
		{
			if (lookup.empty(dm.to.x, dm.to.y)) {
				lookup.erase(d.x, d.y);
				d.x = dm.to.x; d.y = dm.to.y;
				lookup.set({d.x,d.y}, e);
			}
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
		color_texture(((uint32_t)d.y * x()) + (uint32_t)d.x, d.color);
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

size_t Dotfield::DotLookup::p2i(int x, int y) {
	return (y*w)+x;
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


