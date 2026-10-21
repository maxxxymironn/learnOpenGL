#include "timeHandler.hpp"

#include <chrono>
#include <thread>

void TimeHandler::waitFramerate(const float curTime) {
    _currentFrame = curTime;
    _frametime = _currentFrame - _lastFrame;
    if (_frametime < _maxFrametime)
        std::this_thread::sleep_for(std::chrono::duration<float>(_maxFrametime - _frametime));
}

void TimeHandler::update(const float curTime) {
    _currentFrame = curTime;
    _frametime = _currentFrame - _lastFrame;
    _lastFrame = _currentFrame;

    ++_frameAccumulator;
    _frameTimeAccumulator += _frametime;
    _newFps = false;
    if (_frameTimeAccumulator >= 1.f) {
        if (_frameAccumulator != _prevFrameAccumulator) {
            _prevFrameAccumulator = _frameAccumulator;
            _newFps = true;
        }
        _frameAccumulator = 0;
        _frameTimeAccumulator = 0.f;
    }
}