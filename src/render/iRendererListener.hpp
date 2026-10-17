#pragma once

class IRendererListener {
    public:
        virtual ~IRendererListener() = default;

        virtual void updateAspectRatio(const float aspectRatio) = 0;
};
