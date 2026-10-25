#pragma once
#include "camera.hpp"
#include "iRendererListener.hpp"
#include "shader.hpp"
#include "model/model.hpp"

enum class PolygonMode: unsigned char {
    FILL,
    LINE
};

enum class ShaderMode: unsigned char {
    MAIN,
    SIMPLE,
};

class Renderer: public IRendererListener {
    Shader _mainShader, _simpleShader;
    Camera _camera;
    Model _model;

    unsigned int _vao, _vbo, _ebo;
    float _aspectRatio;
    PolygonMode _polygonMode;
    bool _isShaderMain;

public:
    Renderer();
    
    bool getStatus() const { return _mainShader.id && _simpleShader.id;}
    
    Camera& camera() { return _camera; }

    PolygonMode getPolygonMode() const { return _polygonMode; }
    void setPolygonMode(const PolygonMode newMode);

    void updateAspectRatio(const float aspectRatio) override { _aspectRatio = aspectRatio; }
    void updateInnerFlashBorder();

    void toggleShaderMode() { _isShaderMain = !_isShaderMain; }
    
    void draw();
};
