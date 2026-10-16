#pragma once

#include "vector.hpp"
#include "inputEnum.hpp"

#include <array>

class InputHandler {
    std::array<bool, static_cast<int>(Key::COUNT)> currKeys{false};
    std::array<bool, static_cast<int>(Key::COUNT)> prevKeys{false};

    std::array<float, static_cast<int>(Mouse::COUNT)> currMouse{0.f};
    std::array<float, static_cast<int>(Mouse::COUNT)> prevMouse{0.f};

    float scrollOffset = 0.f;
    bool isScrolled = false;

public:
    void updateKeyStatus(const Key key, const bool isPressed) {
        currKeys[static_cast<int>(key)] = isPressed;
    }

    void updateMouseStatus(const Mouse var, const float value) {
        currMouse[static_cast<int>(var)] = value;
    }

    void updateMouseScroll(const float value) {
        scrollOffset = value;
        isScrolled = true;
    }

    void refreshBuffers() {
        prevKeys = currKeys;
        prevMouse = currMouse;
    }

    bool isKeyPressed(const Key key) {
        int i = static_cast<int>(key);
        return currKeys[i] && !prevKeys[i];
    }

    bool isKeyDown(const Key key) {
        int i = static_cast<int>(key);
        return currKeys[i] && prevKeys[i];
    }

    bool isKeyReleased(const Key key) {
        int i = static_cast<int>(key);
        return !currKeys[i] && prevKeys[i];
    }

    bool isMouseMoved() { return !(currMouse[0] == prevMouse[0] && currMouse[1] == prevMouse[1]); }

    bool isMouseScrolled() { return isScrolled; }

    vec2f getMousePosOffset() { return vec2f(prevMouse[0] - currMouse[0], prevMouse[1] - currMouse[1]); }

    float getScrollOffset() {
        isScrolled = false;
        return scrollOffset;
    }
};
