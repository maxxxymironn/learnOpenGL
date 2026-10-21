#pragma once

class TimeHandler {
    float _frametime = 0.f;
    float _frameTimeAccumulator = 0.f;
    
    float _currentFrame = 0.f;
    float _lastFrame = 0.f;
    const float _maxFrametime = 60.f;

    int _prevFrameAccumulator = 0;
    int _frameAccumulator = 0;
    bool _newFps = false;    

public:
    TimeHandler(const int maxFPS): _maxFrametime(1.f / maxFPS) {}
    
    void waitFramerate(const float curTime);

    void update(const float curTime);

    bool isNewFps() const { return _newFps; }

    int getNewFps() const { return _prevFrameAccumulator; }

    float getFrametime() const { return _frametime; }
};