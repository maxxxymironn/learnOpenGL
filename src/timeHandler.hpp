#pragma once

#include "logger.hpp"

#include <chrono>
#include <thread>
#include <string>

struct TimeHandler {
    const float TARGET_FRAMERATE = 1.f / 30.f;
    
    float deltaTime = 0.f;
    float currentFrame = 0.f;
    float lastFrame = 0.f;

    float deltaAccumulator = 0.f;
    int frameAccumulator = 0;

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
            std::string toPrint("fps=");
            toPrint += std::to_string(frameAccumulator);
            
            Logger::getInstance().print(toPrint.c_str());
            frameAccumulator = 0;
            deltaAccumulator = 0.f;
        }
    }
};