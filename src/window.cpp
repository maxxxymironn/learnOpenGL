#include "window.hpp"

#include "logger.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

void Window::_setKeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    Window* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (self) {
        if (key == GLFW_KEY_ESCAPE) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
    }
}

Window::Window(): _handle(nullptr), _status(0) {
    if (!glfwInit()) {
        Logger::getInstance().printErr("Initialize glfw failed");
        _status = 1;
    }
    else {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        
        _handle = glfwCreateWindow(800, 800, "OpenGL", nullptr, nullptr);
        if (!_handle) {
            Logger::getInstance().printErr("Create window failed");
            _status = 1;
        }
        else {
            glfwMakeContextCurrent(_handle);
        
            if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
                Logger::getInstance().printErr("Initialize glad failed");
                _status = 1;
            }
        }
    }

    if (_status == 0) {
        glfwSetWindowUserPointer(_handle, this);
        glfwGetFramebufferSize(_handle, &_width, &_height);
        
        glfwSwapInterval(GLFW_TRUE);
        
        glViewport(0, 0, _width, _height);

        glfwSetKeyCallback(_handle, _setKeyCallback);
    }
}

Window::~Window() { glfwTerminate(); }

bool Window::shouldClose() const { return glfwWindowShouldClose(_handle); }

void Window::close() { glfwSetWindowShouldClose(_handle, GLFW_TRUE); }

void Window::endFrame() {
    glfwPollEvents();
    glfwSwapBuffers(_handle);
}