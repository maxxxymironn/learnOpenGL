#pragma once

class Renderer {
    unsigned int _vao, _vbo, _ebo;
    unsigned int _shader;

    unsigned int _texture;
    int _texWidth, _texHeight, _texChannels;
    unsigned char* _textureData;
    
public:
    Renderer();
    ~Renderer();

    bool initializeSuccess() { return _shader != 0; }

    void draw();

    void setWireframeMode();
    void setNormalMode();

    void updateColorCoeffUni(const float newValue);

    void updateTransformMatrixUni(const float degrees, const float moveX=0.f, const float moveY=0.f);
};
