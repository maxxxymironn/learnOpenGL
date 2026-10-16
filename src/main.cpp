#include "timeHandler.hpp"
#include "window.hpp"
#include "renderer.hpp"
#include "inputHandler.hpp"

#include <cmath>
#include <functional>

namespace {
    constexpr int WIDTH = 800;
    constexpr int HEIGHT = 800;
    constexpr float MOUSE_SENSITIVITY = 1.f;
    
    constexpr int MAX_FPS = 60;     // if VSYNC=false
    constexpr bool VSYNC = true;
}

void checkInput(InputHandler& input, Renderer& renderer);

int main() {
    InputHandler input;
    Window window(WIDTH, HEIGHT, VSYNC);
    if (!window.getStatus()) return 1;
    
    Renderer renderer;
    if (!renderer.initializeSuccess()) return 1;
    
    TimeHandler clock(MAX_FPS);

    window.setFunUpdateKeyStatus([&input](const Key key, const bool isPressed) { input.updateKeyStatus(key, isPressed); });
    window.setFunUpdateMouseStatus([&input](const Mouse key, const float value) { input.updateMouseStatus(key, value); });    
    window.setFunUpdateMouseScroll([&input](const float value) { input.updateMouseScroll(value); });
    
    while(!window.shouldClose()) {
        if constexpr (!VSYNC) clock.waitFramerate(window.getTime());
        clock.update(window.getTime());
        if (clock.newFps) window.refreshTitle(clock.prevFrameAccumulator);
        
        checkInput(input, renderer);

        float time = window.getTime();
        renderer.updateDeltaTime(clock.deltaTime);
        renderer.updateColorCoeffUni(std::sin(time) * 0.5f + 0.5f);
        vec2f windowSize = window.getSize();
        float aspect = windowSize.x / windowSize.y;
        renderer.drawManyCubes(aspect, time * 50.f);
        
        window.endFrame();
    }

    return 0;
}

void checkInput(InputHandler& input, Renderer& renderer) {
    if (input.isKeyPressed(Key::POLYGON_MODE)) {
        renderer.isWireframeMode() ? renderer.setNormalMode()
                                   : renderer.setWireframeMode();
    }

    float cameraSpeed = 5.f;
    if (input.isKeyDown(Key::SHIFT)) {
        cameraSpeed = 20.f;
    }
    if (input.isKeyDown(Key::RIGHT) || input.isKeyPressed(Key::RIGHT)) {
        renderer.moveCameraPos(Direction::RIGHT, cameraSpeed);
    }
    if (input.isKeyDown(Key::LEFT) || input.isKeyPressed(Key::LEFT)) {
        renderer.moveCameraPos(Direction::LEFT, cameraSpeed);
    }
    if (input.isKeyDown(Key::DOWN) || input.isKeyPressed(Key::DOWN)) {
        renderer.moveCameraPos(Direction::BACK, cameraSpeed);
    }
    if (input.isKeyDown(Key::UP) || input.isKeyPressed(Key::UP)) {
        renderer.moveCameraPos(Direction::FORWARD, cameraSpeed);
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
        
        renderer.moveCameraFront(pitch, yaw);
    }
    if (input.isMouseScrolled()) {
        static float fov = 45.f;
        
        float scrollOffset = input.getScrollOffset();
        fov -= scrollOffset;
        if (fov < 1.f) fov = 1.f;
        else if (fov > 80.f) fov = 80.f;
        
        renderer.changeCameraFov(fov);
    }

    input.refreshBuffers();
}
