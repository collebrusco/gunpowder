#ifndef PILOT_H
#define PILOT_H
#include "Driver.h"

class Game : public GameDriver {
public:
	Game(Window& win);
	void userCreate() override final;
	void userUpdate(float dt) override final;
	void userDestroy() override final;
};

#endif
