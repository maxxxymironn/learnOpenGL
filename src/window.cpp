#include "window.hpp"

#include "logger.hpp"
#include "keyHandler.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace {
    float sensitivity = 0.05f;
    float prevCursorPosX = 0.f;
    float prevCursorPosY = 0.f;
}

void Window::_keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    Window* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (self) {
        if (key == GLFW_KEY_ESCAPE) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
            return;
        }

        switch (key) {
            case GLFW_KEY_M: KeyHandler::updateKeyStatus(Key::POLYGON_MODE, action); break;
            case GLFW_KEY_D: KeyHandler::updateKeyStatus(Key::RIGHT, action); break;
            case GLFW_KEY_RIGHT: KeyHandler::updateKeyStatus(Key::RIGHT, action); break;
            case GLFW_KEY_A: KeyHandler::updateKeyStatus(Key::LEFT, action); break;
            case GLFW_KEY_LEFT: KeyHandler::updateKeyStatus(Key::LEFT, action); break;
            case GLFW_KEY_W: KeyHandler::updateKeyStatus(Key::UP, action); break;
            case GLFW_KEY_UP: KeyHandler::updateKeyStatus(Key::UP, action); break;
            case GLFW_KEY_S: KeyHandler::updateKeyStatus(Key::DOWN, action); break;
            case GLFW_KEY_DOWN: KeyHandler::updateKeyStatus(Key::DOWN, action); break;
            case GLFW_KEY_LEFT_SHIFT: KeyHandler::updateKeyStatus(Key::SHIFT, action); break;
            default: break;
        }
    }
}

void Window::_framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    Window* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (self) {
        self->_width = width;
        self->_height = height;
    }
    
    glViewport(0, 0, width, height);
}

void Window::_windowFocusCallback(GLFWwindow* window, int focused) {
    if (focused) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        
        double x, y;
        glfwGetCursorPos(window, &x, &y);
        prevCursorPosX = x;
        prevCursorPosY = y;
        
        glfwSetCursorPosCallback(window, Window::_cursorPosCallback);
    }
    else {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        glfwSetCursorPosCallback(window, nullptr);
    }
}

void Window::_cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    float xOffset = (xpos - prevCursorPosX) * sensitivity;
    float yOffset = (prevCursorPosY - ypos) * sensitivity; 
    
    KeyHandler::yaw += xOffset;
    KeyHandler::pitch += yOffset;
    
    if (KeyHandler::pitch > 89.f)
        KeyHandler::pitch = 89.f;
    else if (KeyHandler::pitch < -89.f)
        KeyHandler::pitch = -89.f;

    KeyHandler::isCursorMoved = true;
    
    prevCursorPosX = xpos;
    prevCursorPosY = ypos;
}

void Window::_scrollCallback(GLFWwindow *window, double xoffset, double yoffset) {
    if (yoffset < 0.01f && yoffset > -0.01f)
        return;
    
    KeyHandler::isScrolled = true;
    KeyHandler::scrollOffset = yoffset;
}

Window::Window(): _handle(nullptr), _width(800), _height(800), _status(0), _vSync(true) {
    if (!glfwInit()) {
        Logger::getInstance().printErr("Initialize glfw failed");
        _status = 1;
    }
    else {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        _handle = glfwCreateWindow(_width, _height, "OpenGL", nullptr, nullptr);
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
        glfwSetWindowSizeLimits(_handle, 150, 150, GLFW_DONT_CARE, GLFW_DONT_CARE);
        glfwSetWindowUserPointer(_handle, this);
        glfwGetFramebufferSize(_handle, &_width, &_height);

        glfwSwapInterval(_vSync);
        glViewport(0, 0, _width, _height);
        prevCursorPosX = static_cast<float>(_width) / 2;
        prevCursorPosY = static_cast<float>(_height) / 2;
        glfwSetCursorPos(_handle, prevCursorPosX, prevCursorPosY);

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

void Window::close() { glfwSetWindowShouldClose(_handle, GLFW_TRUE); }

void Window::endFrame() {
    glfwPollEvents();
    glfwSwapBuffers(_handle);
}

float Window::getTime() { return static_cast<float>(glfwGetTime()); }