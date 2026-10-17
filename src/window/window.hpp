#pragma once
#include "src/core/vector.hpp"
#include "src/input/iInputListener.hpp"
#include "src/render/iRendererListener.hpp"

struct GLFWwindow;

class Window {    
    GLFWwindow* _handle;
    vec2i _size;

    IInputListener* _inputListener;
    IRendererListener* _rendererListener;

    static void _keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void _framebufferSizeCallback(GLFWwindow* window, int width, int height);
    static void _windowFocusCallback(GLFWwindow* window, int focused);
    static void _cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
    static void _scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
public:
    Window(const int x=800, const int y=800, const bool vsync=true);
    ~Window();

    void setInputListener(IInputListener* listener) { _inputListener = listener; }
    void setRendererListener(IRendererListener* listener) { _rendererListener = listener; }

    float getTime() const;
    bool getStatus() const { return _handle; }
    vec2i getSize() const { return _size; }
    
    bool shouldClose() const;

    void endFrame();

    void refreshTitle(const int fps);
};