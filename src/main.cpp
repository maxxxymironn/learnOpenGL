#include "window.hpp"
#include "renderer.hpp"
#include "states.hpp"

#include <cmath>

void checkInput(Renderer& renderer);

int main() {
    Window window;
    if (window.getStatus() == 1)
        return 1;

    Renderer renderer;
    if (!renderer.initializeSuccess())
        return 1;

    while(!window.shouldClose()) {
        float time = window.getTime();
        renderer.updateColorCoeffUni(std::sin(time) * 0.5f + 0.5f);
        renderer.updateTransformMatrixUni(time * 50.f, 0.5f, -0.5f);
        
        renderer.draw();
        window.endFrame();

        if (States::updated)
            checkInput(renderer);
    }

    return 0;
}

void checkInput(Renderer& renderer) {
    if (States::polygonModeUpdated) {
        if (States::polygonMode)
            renderer.setWireframeMode();
        else
            renderer.setNormalMode();

        States::polygonModeUpdated = false;
    }

    States::updated = false;
}
