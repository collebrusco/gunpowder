#include "Driver.h"
#include <cmath>
#include <flgl/logger.h>
LOG_MODULE(driver);

GameDriver::GameDriver(uint32_t tr) : dt(_dt), launch_timer(_launch_timer),
                                    tick_rate(_tick_rate),
                                    _tick_rate(tr),
                                    _close(false),
                                    _dt(1.f/60.f),
                                    _ticks(0),
                                    _launch_timer(ftime::SECONDS),
                                    tick_timer(ftime::SECONDS),
                                    delta_timer(ftime::SECONDS)
{}

void GameDriver::exit() {
    user_destroy();
}

bool GameDriver::create() { 
    _launch_timer.start();
    user_create();
    return true;
}

void GameDriver::loop() {
    user_update(_dt);

    static const float t_pd = 1./((float)(this->tick_rate));
    if (tick_timer.read() > t_pd)
        user_tick(++_ticks, tick_timer.stop_reset_start());

    window.update();
    _dt = delta_timer.stop_reset_start();
    if (std::isinf(_dt) || std::isnan(_dt)){
        _dt = 1.f / 80.f; //TODO: hate this...
    }
}

void GameDriver::start() {
    tick_timer.start();
    while (!_close && !window.should_close()) {
       loop();
    }
    LOG_INF("driver out of loop, exiting...");
    exit();
}

void GameDriver::close() {
	_close = true;
}
