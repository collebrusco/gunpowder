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
	Dot(uint32_t u, uint32_t v, uint8_t r, uint8_t g, uint8_t b, bool w);
	uint32_t x, y;
	uint8_t color[3];
	bool water{0};
};

struct DotMove {
	glm::ivec2 to;
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
	entID add_dot(uint32_t x, uint32_t y, Args... args) {
		if (x < 0 || x > this->x() || y < 0 || y > this->y()) {
			return 0xFFFFFFFFFFFFFFFF;
		}
		auto e = dots.newEntity();
		dots.addComp<Dot>(e, x, y, args...);
		lookup.set({x,y},e);
		return e;
	}

	void move_dot(entID dot, uint32_t x, uint32_t y);

	void erase_dots();
	void update_dots();
	void commit_dots();
	void paint_dots();

	class DotLookup {
		size_t w,h;
		std::unordered_map<glm::ivec2, entID> lookup;
	public:
		DotLookup(size_t x, size_t y);
		void set(glm::ivec2 pos, entID dot);
		void erase(int x, int y);
		bool empty(int x, int y);
		entID get(int x, int y);
		void clear();
	};
	DotLookup lookup;
};

#endif
