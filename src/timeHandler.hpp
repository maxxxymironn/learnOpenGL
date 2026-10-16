#pragma once

#include <chrono>
#include <thread>

struct TimeHandler {
    float deltaTime = 0.f;
    float deltaAccumulator = 0.f;
    
    float currentFrame = 0.f;
    float lastFrame = 0.f;
    const float _maxFrametime = 60.f;

    int prevFrameAccumulator = 0;
    int frameAccumulator = 0;
    bool newFps = false;    

    TimeHandler(const int maxFPS): _maxFrametime(1.f / maxFPS) {}
    
    void waitFramerate(const float curTime) {
        currentFrame = curTime;
        deltaTime = currentFrame - lastFrame;
        if (deltaTime < _maxFrametime)
            std::this_thread::sleep_for(std::chrono::duration<float>(_maxFrametime - deltaTime));
    }

    void update(const float curTime) {
        currentFrame = curTime;
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        ++frameAccumulator;
        deltaAccumulator += deltaTime;
        newFps = false;
        if (deltaAccumulator >= 1.f) {
            if (frameAccumulator != prevFrameAccumulator) {
                prevFrameAccumulator = frameAccumulator;
                newFps = true;
            }
            frameAccumulator = 0;
            deltaAccumulator = 0.f;
        }
    }
};