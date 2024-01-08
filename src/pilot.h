#ifndef PILOT_H
#define PILOT_H
#include "Driver.h"

class DotRunner : public GameDriver {
public:
	DotRunner();
	void user_create() override final;
	void user_update(float dt) override final;
	void user_tick(size_t ticks, float dt) override final;
	void user_destroy() override final;
};

#endif
