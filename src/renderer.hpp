#pragma once

class Renderer {
    unsigned int _vao, _vbo, _ebo;
    unsigned int _shader;
    int _uniformLocation;
    
public:
    Renderer();
    ~Renderer();

    bool initializeSuccess() { return _shader != 0; }

    void draw();

    void setWireframeMode();
    void setNormalMode();

    void updateUniform(const float newValue);
};
