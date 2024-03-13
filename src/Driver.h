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
    void loop();
    GameDriver(uint32_t tick_rate);

    float const& dt;
    Stopwatch const& launch_timer;
    uint32_t const& tick_rate;

private:
    float t_pd;
    uint32_t _tick_rate;
	bool _close;
	float _dt;
    size_t _ticks;
	Stopwatch _launch_timer;
    Stopwatch tick_timer;
    Stopwatch delta_timer;
    virtual void user_create() = 0;
    virtual void user_tick(size_t tick, float dt) = 0;
    virtual void user_update(float dt) = 0;
    virtual void user_destroy() = 0;
    void exit();
};


#endif
