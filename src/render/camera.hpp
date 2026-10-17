#pragma once

#include "glm/glm.hpp"
#include "glm/geometric.hpp"

enum class Direction {
    RIGHT,
    LEFT,
    FORWARD,
    BACK
};

class Camera {
    glm::vec3 _position;
    glm::vec3 _front;
    glm::vec3 _up;
    float _fov;
    float _speed;

public:
    Camera()
        : _position({ 0.f, 0.f, 5.f })
        , _front({ 0.f, 0.f, -1.f })
        , _up({ 0.f, 1.f, 0.f })
        , _fov(45.f)
        , _speed(0.f) {}

    glm::vec3 position() const { return _position; }
    
    glm::vec3 front() const { return _front; }
    
    glm::vec3 up() const { return _up; }
    
    float fov() const { return _fov; }

    void updateSpeed(const float speed) { _speed = speed; }
    
    void updatePosition(const Direction dir) {
        switch (dir) {
            case Direction::RIGHT:
                _position += glm::normalize(glm::cross(_front, _up)) * _speed; break;
            case Direction::LEFT:
                _position -= glm::normalize(glm::cross(_front, _up)) * _speed; break;
            case Direction::FORWARD:
                _position += _front * _speed; break;
            case Direction::BACK:
                _position -= _front * _speed; break;
        }
    }
    
    void updateFront(const float pitch, const float yaw) {
        _front = {
            std::cos(glm::radians(pitch)) * std::sin(glm::radians(yaw)),
            std::sin(glm::radians(pitch)),
            std::cos(glm::radians(pitch)) * std::cos(glm::radians(yaw))
        };
        _front = glm::normalize(_front);
    }
    
    void updateFov(const float newFov) { _fov = newFov; }
};