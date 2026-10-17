#pragma once

#include <chrono>
#include <thread>

struct TimeHandler {
    float frameTime = 0.f;
    float frameTimeAccumulator = 0.f;
    
    float currentFrame = 0.f;
    float lastFrame = 0.f;
    const float _maxFrametime = 60.f;

    int prevFrameAccumulator = 0;
    int frameAccumulator = 0;
    bool newFps = false;    

    TimeHandler(const int maxFPS): _maxFrametime(1.f / maxFPS) {}
    
    void waitFramerate(const float curTime) {
        currentFrame = curTime;
        frameTime = currentFrame - lastFrame;
        if (frameTime < _maxFrametime)
            std::this_thread::sleep_for(std::chrono::duration<float>(_maxFrametime - frameTime));
    }

    void update(const float curTime) {
        currentFrame = curTime;
        frameTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        ++frameAccumulator;
        frameTimeAccumulator += frameTime;
        newFps = false;
        if (frameTimeAccumulator >= 1.f) {
            if (frameAccumulator != prevFrameAccumulator) {
                prevFrameAccumulator = frameAccumulator;
                newFps = true;
            }
            frameAccumulator = 0;
            frameTimeAccumulator = 0.f;
        }
    }
};