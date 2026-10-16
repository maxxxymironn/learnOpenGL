#pragma once
#include "vector.hpp"
#include "inputEnum.hpp"

#include <functional>

struct GLFWwindow;

class Window {    
    GLFWwindow* _handle;
    vec2i _size;
    bool _status;

    std::function<void(const Key key, const bool isPressed)> _updateKeyStatus;
    std::function<void(const Mouse key, const float value)> _updateMouseStatus;
    std::function<void(const float value)> _updateMouseScroll;

    static void _keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void _framebufferSizeCallback(GLFWwindow* window, int width, int height);
    static void _windowFocusCallback(GLFWwindow* window, int focused);
    static void _cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
    static void _scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
public:
    Window(const int x=800, const int y=800, const bool vsync=true);
    ~Window();

    void setFunUpdateKeyStatus(std::function<void(const Key, const bool)> fun) { _updateKeyStatus = fun; }
    void setFunUpdateMouseStatus(std::function<void(const Mouse, const float)> fun) { _updateMouseStatus = fun; }
    void setFunUpdateMouseScroll(std::function<void(const float)> fun) { _updateMouseScroll = fun; }

    float getTime() const;
    bool getStatus() const { return _handle; }
    vec2i getSize() const { return _size; }
    
    bool shouldClose() const;

    void endFrame();

    void refreshTitle(const int fps);
};