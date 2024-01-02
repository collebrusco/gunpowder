#ifndef DRIVER_H
#define DRIVER_H

#include <flgl.h>
#include "../lib/sw/Stopwatch.h"

class GameDriver {
protected:
	void close();
public:
    bool create();
    void start();
    GameDriver(Window& win);

    Window& window;

    float const& dt;
    ftime::Stopwatch const& launch_timer;

private:
	bool _close;
	float _dt;
	ftime::Stopwatch _launch_timer;
    ftime::Stopwatch delta_timer;
    virtual void userCreate() = 0;
    virtual void userUpdate(float dt) = 0;
    virtual void userDestroy() = 0;
    void exit();
};


#endif
