#pragma once

class Renderer {
    unsigned int _vao, _vbo, _ebo;
    unsigned int _shader;
    
public:
    Renderer();
    ~Renderer();

    bool initializeSuccess() { return _shader != 0; }

    void draw();
};
