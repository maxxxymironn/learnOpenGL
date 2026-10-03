#include "window.hpp"
#include "renderer.hpp"
#include "states.hpp"

void checkInput(Renderer& renderer);

int main() {
    Window window;
    if (window.getStatus() == 1)
        return 1;

    Renderer renderer;
    if (!renderer.initializeSuccess())
        return 1;

    while(!window.shouldClose()) {
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
