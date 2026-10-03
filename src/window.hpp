#pragma once

struct GLFWwindow;

class Window {    
    GLFWwindow* _handle;
    int _width;
    int _height;
    bool _status;

    static void _setKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

public:
    Window();
    ~Window();

    bool getStatus() const { return _status; }
    
    bool shouldClose() const;
    void close();

    void endFrame();
};