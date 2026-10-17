#include "window.hpp"

#include "src/core/logger.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>


void Window::_framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    Window* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (self) {
        self->_size = { width, height };
        float aspectRatio = static_cast<float>(width) / height;
        self->_rendererListener->updateAspectRatio(aspectRatio);
    }
    
    glViewport(0, 0, width, height);
}

void Window::_keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    Window* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (self) {
        switch (key) {
            case GLFW_KEY_ESCAPE: glfwSetWindowShouldClose(window, GLFW_TRUE); break;
            case GLFW_KEY_M: self->_inputListener->updateKeyStatus(Key::POLYGON_MODE, action); break;
            case GLFW_KEY_D: self->_inputListener->updateKeyStatus(Key::RIGHT, action); break;
            case GLFW_KEY_RIGHT: self->_inputListener->updateKeyStatus(Key::RIGHT, action); break;
            case GLFW_KEY_A: self->_inputListener->updateKeyStatus(Key::LEFT, action); break;
            case GLFW_KEY_LEFT: self->_inputListener->updateKeyStatus(Key::LEFT, action); break;
            case GLFW_KEY_W: self->_inputListener->updateKeyStatus(Key::UP, action); break;
            case GLFW_KEY_UP: self->_inputListener->updateKeyStatus(Key::UP, action); break;
            case GLFW_KEY_S: self->_inputListener->updateKeyStatus(Key::DOWN, action); break;
            case GLFW_KEY_DOWN: self->_inputListener->updateKeyStatus(Key::DOWN, action); break;
            case GLFW_KEY_LEFT_SHIFT: self->_inputListener->updateKeyStatus(Key::SHIFT, action); break;
            default: break;
        }
    }
}

void Window::_windowFocusCallback(GLFWwindow* window, int focused) {
    Window* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (self) {
        if (focused) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            
            vec2d cursorPos;
            glfwGetCursorPos(window, &cursorPos.x, &cursorPos.y);
            self->_inputListener->updateMouseStatus(Mouse::X_POS, cursorPos.x);
            self->_inputListener->updateMouseStatus(Mouse::Y_POS, cursorPos.y);
            
            glfwSetCursorPosCallback(window, Window::_cursorPosCallback);
        }
        else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            glfwSetCursorPosCallback(window, nullptr);
        }
    }
}

void Window::_cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    Window* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (self) {
        self->_inputListener->updateMouseStatus(Mouse::X_POS, xpos);
        self->_inputListener->updateMouseStatus(Mouse::Y_POS, ypos);
    }
}

void Window::_scrollCallback(GLFWwindow *window, double xoffset, double yoffset) {
    if (yoffset < 0.01f && yoffset > -0.01f)
        return;

    Window* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (self) {
        self->_inputListener->updateMouseScroll(yoffset);
    }
}

Window::Window(const int x, const int y, const bool vsync)
    : _handle(nullptr)
    , _size(x, y)
    , _inputListener(nullptr)
    , _rendererListener(nullptr)
{
    if (!glfwInit()) {
        Logger::getInstance().printErr("Initialize glfw failed");
    } 
    else {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        _handle = glfwCreateWindow(_size.x, _size.y, "OpenGL", nullptr, nullptr);
        if (!_handle) {
            Logger::getInstance().printErr("Create window failed");
        }
        else {
            glfwMakeContextCurrent(_handle);
        
            if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
                Logger::getInstance().printErr("Initialize glad failed");
                _handle = nullptr;
            }
        }
    }

    if (_handle) {
        glfwSetWindowSizeLimits(_handle, 150, 150, GLFW_DONT_CARE, GLFW_DONT_CARE);
        glfwSetWindowUserPointer(_handle, this);
        glfwGetFramebufferSize(_handle, &_size.x, &_size.y);
        
        glfwSwapInterval(vsync);
        glViewport(0, 0, _size.x, _size.y);

        glfwSetKeyCallback(_handle, _keyCallback);
        glfwSetFramebufferSizeCallback(_handle, _framebufferSizeCallback);
        glfwSetWindowFocusCallback(_handle, _windowFocusCallback);
        glfwSetCursorPosCallback(_handle, _cursorPosCallback);
        glfwSetScrollCallback(_handle, _scrollCallback);

        glfwSetInputMode(_handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
}

Window::~Window() { glfwTerminate(); }

bool Window::shouldClose() const { return glfwWindowShouldClose(_handle); }

void Window::endFrame() {
    glfwPollEvents();
    glfwSwapBuffers(_handle);
}

float Window::getTime() const { return static_cast<float>(glfwGetTime()); }

void Window::refreshTitle(const int fps) {
    std::string newTitle("OpenGL | FPS: ");
    newTitle += std::to_string(fps);
    glfwSetWindowTitle(_handle, newTitle.c_str());
}