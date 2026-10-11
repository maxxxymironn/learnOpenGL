#pragma once

enum class Direction {
    RIGHT,
    LEFT,
    FORWARD,
    BACK
};

class Renderer {
    unsigned int _vao, _vbo, _ebo;
    unsigned int _shader;

    unsigned int _texture;
    int _texWidth, _texHeight, _texChannels;
    unsigned char* _textureData;

    bool _isWireframeMode = false;

public:
    Renderer();
    ~Renderer();

    bool initializeSuccess() { return _shader != 0; }

    void draw();

    void setWireframeMode();
    void setNormalMode();

    void updateColorCoeffUni(const float newValue);

    void updateTransformMatrixUni(float degrees, float moveX, float moveY, float moveZ, float aspect);

    void drawManyCubes(const float aspect, const float rotateDegrees);

    bool isWireframeMode() const { return _isWireframeMode; }

    void moveCameraPos(const Direction dir, const float speed);

    void updateDeltaTime(const float deltaTime);

    void moveCameraFront(const float pitch, const float yaw);

    void changeCameraFov(const float fov);
};
