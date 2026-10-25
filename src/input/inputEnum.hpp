#pragma once

enum class Key: unsigned char {
    POLYGON_MODE,
    RIGHT,
    LEFT,
    UP,
    DOWN,
    SHIFT,
    SPACE,
    CTRL,
    TOGGLE_FLASHLIGHT,
    TOGGLE_SHADERS,
    COUNT
};

enum class Mouse: unsigned char {
    X_POS,
    Y_POS,
    COUNT
};