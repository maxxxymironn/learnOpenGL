#pragma once

struct GLFWwindow;

class Window {    
    GLFWwindow* _handle;
    int _width;
    int _height;
    
    bool _status;
    bool _vSync;

    static void _keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void _framebufferSizeCallback(GLFWwindow* window, int width, int height);
    static void _windowFocusCallback(GLFWwindow* window, int focused);
    static void _cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
    static void _scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
public:
    Window(const bool vsync=true);
    ~Window();

    bool getStatus() const { return _status; }
    
    bool shouldClose() const;
    void close();

    void endFrame();

    float getTime() const;
    bool vSync() const { return _vSync; }

    int getWidth() const { return _width; }
    int getHeight() const { return _height; }

    void refreshTitle(const int fps);
};