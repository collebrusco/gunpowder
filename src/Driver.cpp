#include "Driver.h"
#include <cmath>
#include <flgl/logger.h>
LOG_MODULE(driver);

GameDriver::GameDriver(Window& win) : window(win),
                                    dt(_dt), launch_timer(_launch_timer),
                                    _close(false),
                                    _dt(1.f/60.f),
                                    _launch_timer(ftime::SECONDS),
                                    delta_timer(ftime::SECONDS)
{}

void GameDriver::exit() {
    userDestroy();
}

bool GameDriver::create() { 
    _launch_timer.start();
    userCreate();
    return true;
}

void GameDriver::start() {
    while (!_close && !window.should_close()) {
        userUpdate(_dt);
        window.update();
        _dt = delta_timer.stop_reset_start();
        if (std::isinf(_dt) || std::isnan(_dt)){
            _dt = 1.f / 80.f; //TODO: hate this...
        }
    }
    LOG_INF("driver out of loop, exiting...");
    exit();
}

void GameDriver::close() {
	_close = true;
}
