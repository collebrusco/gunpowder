#ifndef DOT_FIELD_H
#define DOT_FIELD_H

#include <flgl/glm.h>
#include <unordered_map>
#include "../lib/ecs/ECS.h"
#include <stdlib.h>
#include "Dot.h"
#include "util/Stepper.h"


struct DotMove {
	glm::ivec2 to;
};

class Dotfield {
	uint8_t* _pixels;
	uint32_t _x,_y, _size;
	void erase_dot(entID e);
	void color_texture(uint32_t i, uint8_t * clr);
public:
	ECS dots;
	uint32_t x() const;
	uint32_t y() const;
	uint32_t size() const;
	uint8_t* pixels() const;
	Dotfield(uint32_t x, uint32_t y);
	~Dotfield();

	bool bounds(int x, int y) const;

	glm::ivec2 mouse_pos() const;
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

	void exptr(int, int, int, int, uint32_t, uint8_t);
	void explode(int x, int y, int rad, uint8_t pow);

	void kill_dot(entID dot);
	void kill_dot(int32_t x, int32_t y);

	void q_move(entID dot, int32_t x, int32_t y);

	void apply_gravity(size_t ticks, float dt);
	// air? other forcers?
	void update_dots(size_t ticks, float dt);

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
