#pragma once
#include "camera.hpp"
#include "iRendererListener.hpp"

enum class PolygonMode: unsigned char {
    FILL,
    LINE
};

class Renderer: public IRendererListener {
    unsigned int _vao, _vbo, _ebo;
    unsigned int _shader;
    float _aspectRatio;
    
    PolygonMode _polygonMode;

    Camera _camera;

public:
    Renderer();
    ~Renderer();
    
    bool getStatus() const { return _shader; }
    
    Camera& camera() { return _camera; }

    PolygonMode getPolygonMode() const { return _polygonMode; }
    void setPolygonMode(const PolygonMode newMode);

    void updateAspectRatio(const float aspectRatio) override { _aspectRatio = aspectRatio; }
    
    void draw();
};
