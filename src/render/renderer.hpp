#pragma once
#include "camera.hpp"
#include "iRendererListener.hpp"
#include "shader.hpp"

enum class PolygonMode: unsigned char {
    FILL,
    LINE
};

class Renderer: public IRendererListener {
    Shader _shader1, _shader2;
    Camera _camera;
    
    unsigned int _vao, _vao2, _vbo, _ebo;
    float _aspectRatio;
    PolygonMode _polygonMode;

public:
    Renderer();
    ~Renderer();
    
    bool getStatus() const { return _shader1.id && _shader2.id; }
    
    Camera& camera() { return _camera; }

    PolygonMode getPolygonMode() const { return _polygonMode; }
    void setPolygonMode(const PolygonMode newMode);

    void updateAspectRatio(const float aspectRatio) override { _aspectRatio = aspectRatio; }
    
    void draw();
};
