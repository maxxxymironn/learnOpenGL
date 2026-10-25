#include "src/core/timeHandler.hpp"
#include "src/window/window.hpp"
#include "src/render/renderer.hpp"
#include "src/input/inputHandler.hpp"

namespace {
    constexpr int WIDTH = 800;
    constexpr int HEIGHT = 800;
    constexpr float MOUSE_SENSITIVITY = 1.f;
    
    constexpr int MAX_FPS = 180;     // if VSYNC=false
    constexpr bool VSYNC = true;
}

void checkInput(InputHandler& input, Renderer& renderer, TimeHandler& clock);

int main() {
    InputHandler input;
    Window window(WIDTH, HEIGHT, VSYNC);
    if (!window.getStatus()) return 1;
    
    Renderer renderer;
    if (!renderer.getStatus()) return 1;
    
    TimeHandler clock(MAX_FPS);

    window.setInputListener(&input);
    window.setRendererListener(&renderer);
    
    while(!window.shouldClose()) {
        if constexpr (!VSYNC) clock.waitFramerate(window.getTime());
        clock.update(window.getTime());
        if (clock.isNewFps()) window.refreshTitle(clock.getNewFps());
        
        checkInput(input, renderer, clock);

        renderer.draw();
        window.endFrame();
    }

    return 0;
}

void checkInput(InputHandler& input, Renderer& renderer, TimeHandler& clock) {
    if (input.isKeyPressed(Key::POLYGON_MODE)) {
        renderer.setPolygonMode(
            renderer.getPolygonMode() == PolygonMode::FILL 
                                       ? PolygonMode::LINE
                                       : PolygonMode::FILL
        );
    }

    renderer.camera().updateSpeed(
        (input.isKeyDown(Key::SHIFT) ? 20.f : 5.f) * clock.getFrametime()
    );
        
    if (input.isKeyDown(Key::RIGHT) || input.isKeyPressed(Key::RIGHT)) {
        renderer.camera().updatePosition(Direction::RIGHT);
    }
    if (input.isKeyDown(Key::LEFT) || input.isKeyPressed(Key::LEFT)) {
        renderer.camera().updatePosition(Direction::LEFT);
    }
    if (input.isKeyDown(Key::DOWN) || input.isKeyPressed(Key::DOWN)) {
        renderer.camera().updatePosition(Direction::BACK);
    }
    if (input.isKeyDown(Key::UP) || input.isKeyPressed(Key::UP)) {
        renderer.camera().updatePosition(Direction::FORWARD);
    }
    if (input.isKeyDown(Key::SPACE) || input.isKeyPressed(Key::SPACE)) {
        renderer.camera().updatePosition(Direction::UP);
    }
    if (input.isKeyDown(Key::CTRL) || input.isKeyPressed(Key::CTRL)) {
        renderer.camera().updatePosition(Direction::DOWN);
    }
    if (input.isKeyPressed(Key::TOGGLE_FLASHLIGHT)) {
        renderer.updateInnerFlashBorder();
    }
    if (input.isKeyPressed(Key::TOGGLE_SHADERS)) {
        renderer.toggleShaderMode();
    }
    
    if (input.isMouseMoved()) {
        static float yaw = 180.f;
        static float pitch = 0.f;
        
        vec2f posOffset = input.getMousePosOffset();
        posOffset *= 0.05f * MOUSE_SENSITIVITY;
        yaw += posOffset.x;
        pitch += posOffset.y;

        if (pitch > 89.f) pitch = 89.f;
        else if (pitch < -89.f) pitch = -89.f;
        
        renderer.camera().updateFront(pitch, yaw);
    }
    if (input.isMouseScrolled()) {
        static float fov = 45.f;
        
        float scrollOffset = input.getScrollOffset();
        fov -= scrollOffset;
        if (fov < 1.f) fov = 1.f;
        else if (fov > 80.f) fov = 80.f;
        
        renderer.camera().updateFov(fov);
    }

    input.refreshBuffers();
}
