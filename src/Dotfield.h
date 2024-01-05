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

enum DotProperty {
	DP_NONE = 0x00,
	DP_DOWN = 0x01,
	DP_DOWN_SIDE = 0x02,
	DP_SIDE = 0x04,
	DP_UP_SIDE = 0x08,
	DP_UP = 0x10
};

inline DotProperty operator|(DotProperty const& a, DotProperty const& b) {
	return (DotProperty)((int)a | (int)b);
}
inline DotProperty operator&(DotProperty const& a, DotProperty const& b) {
	return (DotProperty)((int)a & (int)b);
}
inline DotProperty operator~(DotProperty const& a) {
	return (DotProperty)(~((int)a));
}

struct Dot {
	Dot(uint32_t u, uint32_t v, uint8_t r, uint8_t g, uint8_t b, DotProperty p=DP_NONE);
	uint32_t x, y;
	uint8_t color[3];
	DotProperty props;
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

	void update_dots();
	void commit_dots();
	void erase_dots();
	void paint_dots();

	class DotLookup {
		size_t w,h;
		// std::unordered_map<glm::ivec2, entID> lookup;
		entID * lookup;
		size_t p2i(int x, int y);
	public:
		DotLookup(size_t x, size_t y);
		~DotLookup();
		DotLookup(DotLookup const&) = delete;
		DotLookup& operator=(DotLookup const&) = delete;
		void set(glm::ivec2 pos, entID dot);
		void erase(int x, int y);
		bool empty(int x, int y);
		entID get(int x, int y);
		void clear();
	};
	DotLookup lookup;
};

#endif
