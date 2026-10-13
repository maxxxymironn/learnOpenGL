#pragma once

#include <chrono>
#include <thread>

struct TimeHandler {
    const float TARGET_FRAMERATE = 1.f / 30.f;
    
    float deltaTime = 0.f;
    float currentFrame = 0.f;
    float lastFrame = 0.f;

    float deltaAccumulator = 0.f;

    int prevFrameAccumulator = 0;
    int frameAccumulator = 0;
    bool newFps = false;

    void waitFramerate(const float curTime) {
        currentFrame = curTime;
        deltaTime = currentFrame - lastFrame;
        if (deltaTime < TARGET_FRAMERATE)
            std::this_thread::sleep_for(std::chrono::duration<float>(TARGET_FRAMERATE - deltaTime));
    }

    void update(const float curTime) {
        currentFrame = curTime;
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        ++frameAccumulator;
        deltaAccumulator += deltaTime;
        if (deltaAccumulator >= 1.f) {
            newFps = false;
            if (frameAccumulator != prevFrameAccumulator) {
                prevFrameAccumulator = frameAccumulator;
                newFps = true;
            }
            frameAccumulator = 0;
            deltaAccumulator = 0.f;
        }
    }
};