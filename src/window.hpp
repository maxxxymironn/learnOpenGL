#pragma once

struct GLFWwindow;

class Window {    
    GLFWwindow* _handle;
    int _width;
    int _height;
    bool _status;

    static void _keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void _framebufferSizeCallback(GLFWwindow* window, int width, int height);

public:
    Window();
    ~Window();

    bool getStatus() const { return _status; }
    
    bool shouldClose() const;
    void close();

    void endFrame();

    float getTime();

    int getWidth() const { return _width; }
    int getHeight() const { return _height; }
};