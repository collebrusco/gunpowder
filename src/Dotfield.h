#ifndef DOT_FIELD_H
#define DOT_FIELD_H

#include <flgl/glm.h>
#include <unordered_map>
#include "../lib/ecs/ECS.h"

namespace std { template <> struct hash<glm::ivec2> {
std::size_t operator()(const glm::ivec2& obj) const {
	size_t a = std::hash<int>().operator()(obj.y);
	a = (a << (sizeof(size_t)*4)) | (a >> (sizeof(size_t)*4));
	return (std::hash<int>().operator()(obj.x) ^ (a));
}};}

struct Dot {
	Dot(uint32_t u, uint32_t v, uint8_t r, uint8_t g, uint8_t b);
	uint32_t x, y;
	uint8_t color[3];
};

class Dotfield {
	uint8_t* _pixels;
	uint32_t _x,_y, _size;
public:
	ECS dots;
	uint32_t x() const;
	uint32_t y() const;
	uint32_t size() const;
	uint8_t* pixels() const;
	Dotfield(uint32_t x, uint32_t y);
	~Dotfield();

	glm::ivec2 mouse_pos() const;
	void color_texture(uint32_t i, uint8_t * clr);
	template<typename...Args>
	void add_dot(uint32_t x, uint32_t y, Args... args) {
		if (x < 0 || x > this->x() || y < 0 || y > this->y()) {
			return;
		}
		auto e = dots.newEntity();
		dots.addComp<Dot>(e, x, y, args...);
		lookup.set({x,y},e);
	}
	void update();
	void clear();

	class DotLookup {
		std::unordered_map<glm::ivec2, entID> lookup;
	public:
		void set(glm::ivec2 pos, entID dot);
		bool empty(int x, int y);
		entID get(int x, int y);
		void clear();
	};
	DotLookup lookup;
};

#endif
