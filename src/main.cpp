#include "window.hpp"
#include "renderer.hpp"

int main() {
    Window window;
    if (window.getStatus() == 1)
        return 1;
    
    Renderer renderer;
    if (!renderer.initializeSuccess())
        window.close();

    while(!window.shouldClose()) {
        renderer.draw();
        window.endFrame();
    }
    
    return 0;
}