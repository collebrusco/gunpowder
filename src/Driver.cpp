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
                                    _launch_timer(SECONDS),
                                    tick_timer(SECONDS),
                                    delta_timer(SECONDS)
{
    t_pd = 1.f/(this->tick_rate);
}

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
    if (tick_timer.read() > t_pd) {
        user_tick(++_ticks, tick_timer.stop_reset_start());
        // LOG_DBG("tick %d tpd %f, tim %f", _ticks, t_pd, tick_timer.read());
    }

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
