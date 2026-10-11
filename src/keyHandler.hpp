#pragma once

#include <array>

enum class Key {
    POLYGON_MODE,
    RIGHT,
    LEFT,
    UP,
    DOWN,
    SHIFT,
    COUNT
};

struct KeyHandler {
    inline static constexpr std::size_t keyCount = static_cast<std::size_t>(Key::COUNT);
    
    inline static float pitch = 0.f;
    inline static float yaw = -90.f;
    inline static bool isCursorMoved = false;
    
    inline static bool isScrolled = false;
    inline static float scrollOffset = 0.f;
    
    inline static std::array<bool, keyCount> currKeys;
    inline static std::array<bool, keyCount> prevKeys;

    static void updateKeyStatus(const Key key, const bool isPressed) {
        currKeys[static_cast<int>(key)] = isPressed;
    }

    static void updateBuffers() { prevKeys = currKeys; }

    static bool isKeyPressed(const Key key) {
        return currKeys[static_cast<int>(key)] && !prevKeys[static_cast<int>(key)];
    }

    static bool isKeyDown(const Key key) {
        return currKeys[static_cast<int>(key)] && prevKeys[static_cast<int>(key)];
    }

    static bool isKeyReleased(const Key key) {
        return !currKeys[static_cast<int>(key)] && prevKeys[static_cast<int>(key)];
    }
};
