#include "Dot.h"

Dot::Dot(uint32_t u, uint32_t v, uint8_t r, uint8_t g, uint8_t b, DotProperty p)
	: x(u), y(v), color{r,g,b}, props(p) {}

DotResist::DotResist(uint8_t a) : resist(a) {}
