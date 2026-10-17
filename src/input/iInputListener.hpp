#pragma once
#include "inputEnum.hpp"

class IInputListener {
    public:
        virtual ~IInputListener() = default;

        virtual void updateKeyStatus(const Key key, const bool isPressed) = 0;
        virtual void updateMouseStatus(const Mouse key, const float value) = 0;
        virtual void updateMouseScroll(const float value) = 0;
};
