#include "Dot.h"

Dot::Dot(int32_t u, int32_t v, uint8_t r, uint8_t g, uint8_t b)
	: x(u), y(v), color{r,g,b} {}

DotMovable::DotMovable(uint8_t m, int x, int y, glm::vec2 v) : mass(m), pos((float)x,(float)y), velo(v) {}

void DotMovable::force(glm::vec2 N, float dt) {
	this->velo += (N/((float)mass)) * dt;
}
