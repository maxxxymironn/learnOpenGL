#include "timeHandler.hpp"
#include "window.hpp"
#include "renderer.hpp"
#include "keyHandler.hpp"

#include <cmath>

void checkInput(Renderer& renderer);

int main() {
    Window window;
    if (window.getStatus() == 1)
        return 1;

    Renderer renderer;
    if (!renderer.initializeSuccess())
        return 1;

    TimeHandler clock;
    
    while(!window.shouldClose()) {
        if (!window.vSync())
            clock.waitFramerate(window.getTime());
        clock.update(window.getTime());
        
        checkInput(renderer);
        KeyHandler::updateBuffers();

        float time = window.getTime();
        renderer.updateDeltaTime(clock.deltaTime);
        renderer.updateColorCoeffUni(std::sin(time) * 0.5f + 0.5f);
        float aspect = static_cast<float>(window.getWidth()) / static_cast<float>(window.getHeight());
        renderer.drawManyCubes(aspect, time * 50.f);
        window.endFrame();
    }

    return 0;
}

void checkInput(Renderer& renderer) {
    if (KeyHandler::isKeyPressed(Key::POLYGON_MODE)) {
        renderer.isWireframeMode() ? renderer.setNormalMode()
                                   : renderer.setWireframeMode();
    }

    float cameraSpeed = 5.f;
    if (KeyHandler::isKeyDown(Key::SHIFT)) {
        cameraSpeed = 20.f;
    }
    if (KeyHandler::isKeyDown(Key::RIGHT) || KeyHandler::isKeyPressed(Key::RIGHT)) {
        renderer.moveCameraPos(Direction::RIGHT, cameraSpeed);
    }
    if (KeyHandler::isKeyDown(Key::LEFT) || KeyHandler::isKeyPressed(Key::LEFT)) {
        renderer.moveCameraPos(Direction::LEFT, cameraSpeed);
    }
    if (KeyHandler::isKeyDown(Key::DOWN) || KeyHandler::isKeyPressed(Key::DOWN)) {
        renderer.moveCameraPos(Direction::BACK, cameraSpeed);
    }
    if (KeyHandler::isKeyDown(Key::UP) || KeyHandler::isKeyPressed(Key::UP)) {
        renderer.moveCameraPos(Direction::FORWARD, cameraSpeed);
    }
    if (KeyHandler::isCursorMoved) {
        renderer.moveCameraFront(KeyHandler::pitch, KeyHandler::yaw);
        KeyHandler::isCursorMoved = false;
    }
    if (KeyHandler::isScrolled) {
        renderer.changeCameraFov(KeyHandler::scrollOffset);
        KeyHandler::isScrolled = false;
    }
}
