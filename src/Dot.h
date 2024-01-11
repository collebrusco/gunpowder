#ifndef DOT_H
#define DOT_H
#include <flgl.h>

// enum DotProperty {
// 	DP_NONE = 0x00,
// 	DP_DOWN = 0x01,
// 	DP_DOWN_SIDE = 0x02,
// 	DP_SIDE = 0x04,
// 	DP_UP_SIDE = 0x08,
// 	DP_UP = 0x10
// };

// inline DotProperty operator|(DotProperty const& a, DotProperty const& b) { return (DotProperty)((int)a | (int)b); }
// inline DotProperty operator&(DotProperty const& a, DotProperty const& b) { return (DotProperty)((int)a & (int)b); }
// inline DotProperty operator~(DotProperty const& a) { return (DotProperty)(~((int)a)); }

struct Dot {
	Dot(int32_t u, int32_t v, uint8_t r, uint8_t g, uint8_t b);
	int32_t x, y;
	uint8_t color[3];
};

struct DotMovable {
	DotMovable(uint8_t m, int x, int y, glm::vec2 v={0.,0.});
	uint8_t mass;
	glm::vec2 pos, velo;
	void force(glm::vec2 N, float dt);
};

#endif
