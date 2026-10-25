#include "renderer.hpp"

#include "src/render/shader.hpp"

#include <glad/glad.h>
#include <glm/trigonometric.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

#include <string>

std::string modelDir(MODELS_DIR);

Renderer::Renderer()
    : _mainShader("main.vert", "main.frag")
    , _simpleShader("simple.vert", "simple.frag")
    , _aspectRatio(1.f)
    , _polygonMode(PolygonMode::FILL)
    , _isShaderMain(true)
{
    if (_mainShader.id && _simpleShader.id) {
        _model.loadModel((modelDir + "backpack/backpack.obj").c_str());

        /* MAIN SHADER */
        _mainShader.use();
        // matrixes
        _mainShader.setUniLoc("uTransformMat");
        _mainShader.setUniLoc("uViewMat");
        _mainShader.setUniLoc("uProjectionMat");

        // material shininess
        _mainShader.setUniLoc("material.shininess");
        glUniform1f(_mainShader.getUniLoc("material.shininess"), 32.f);

        // flashlight (spotlight)
        _mainShader.setUniLoc("flashlight.position");
        _mainShader.setUniLoc("flashlight.direction");
        _mainShader.setUniLoc("flashlight.innerBorder");
        _mainShader.setUniLoc("flashlight.outerBorder");
        _mainShader.setUniLoc("flashlight.diffuse");
        _mainShader.setUniLoc("flashlight.specular");
        _mainShader.setUniLoc("flashlight.constant");
        _mainShader.setUniLoc("flashlight.linear");
        _mainShader.setUniLoc("flashlight.quadratic");
        glUniform3f(
            _mainShader.getUniLoc("flashlight.position"),
            _camera.position().x, _camera.position().y, _camera.position().z
        );
        glUniform3f(
            _mainShader.getUniLoc("flashlight.direction"),
            _camera.front().x, _camera.front().y, _camera.front().z
        );
        glUniform1f(_mainShader.getUniLoc("flashlight.innerBorder"), glm::cos(glm::radians(12.5f)));
        glUniform1f(_mainShader.getUniLoc("flashlight.outerBorder"), 2.f);
        glUniform3f(_mainShader.getUniLoc("flashlight.diffuse"), 1.f, 1.f, 1.f);
        glUniform3f(_mainShader.getUniLoc("flashlight.specular"), 1.f, 1.f, 1.f);
        glUniform1f(_mainShader.getUniLoc("flashlight.constant"), 1.f);
        glUniform1f(_mainShader.getUniLoc("flashlight.linear"), 0.07f);
        glUniform1f(_mainShader.getUniLoc("flashlight.quadratic"), 0.01f);

        // direct light
        _mainShader.setUniLoc("directLight.direction");
        _mainShader.setUniLoc("directLight.diffuse");
        _mainShader.setUniLoc("directLight.specular");
        glUniform3f(_mainShader.getUniLoc("directLight.direction"), -0.2f, -1.f, -0.2f);
        glUniform3f(_mainShader.getUniLoc("directLight.diffuse"), 1.f, 1.f, 1.f);
        glUniform3f(_mainShader.getUniLoc("directLight.specular"), 1.f, 1.f, 1.f);
        
        // common uniforms
        _mainShader.setUniLoc("ambientLight");
        glUniform3f(_mainShader.getUniLoc("ambientLight"), 0.1f, 0.1f, 0.1f);
        _mainShader.setUniLoc("viewPos");

        /* SIMPLE SHADER */
        _simpleShader.use();
        // matrixes
        _simpleShader.setUniLoc("uTransformMat");
        _simpleShader.setUniLoc("uViewMat");
        _simpleShader.setUniLoc("uProjectionMat");
        
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.2f, 0.2f, 0.2f, 1.f);
    }
}

void Renderer::draw() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 projection = glm::perspective(glm::radians(_camera.fov()), _aspectRatio, 0.1f, 1000.f);
    glm::mat4 view = glm::lookAt(
        _camera.position(),
        _camera.position() + _camera.front(),
        _camera.up()
    );
    glm::mat4 transform(1.f);
    transform = glm::translate(transform, glm::vec3(0.f, 0.f, 0.f));
    transform = glm::scale(transform, glm::vec3(1.f, 1.f, 1.f));
    
    if (_isShaderMain) {
        _mainShader.use();
        
        glUniformMatrix4fv(_mainShader.getUniLoc("uProjectionMat"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(_mainShader.getUniLoc("uViewMat"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(_mainShader.getUniLoc("uTransformMat"), 1, GL_FALSE, glm::value_ptr(transform));
        glUniform3f(
            _mainShader.getUniLoc("flashlight.position"),
            _camera.position().x, _camera.position().y, _camera.position().z
        );
        glUniform3f(
            _mainShader.getUniLoc("flashlight.direction"),
            _camera.front().x, _camera.front().y, _camera.front().z
        );
        glUniform3f(_mainShader.getUniLoc("viewPos"), _camera.position().x, _camera.position().y, _camera.position().z);

        _model.draw(_mainShader);
    }
    else {
        _simpleShader.use();
        
        glUniformMatrix4fv(_simpleShader.getUniLoc("uProjectionMat"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(_simpleShader.getUniLoc("uViewMat"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(_simpleShader.getUniLoc("uTransformMat"), 1, GL_FALSE, glm::value_ptr(transform));
        
        _model.draw(_simpleShader);
    }

    glUseProgram(0);
}

void Renderer::setPolygonMode(const PolygonMode newMode) {
    _polygonMode = newMode;
    switch (newMode) {
        case PolygonMode::FILL: glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); break;
        case PolygonMode::LINE: glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); break;
    }
}

void Renderer::updateInnerFlashBorder() {
    static bool flashlightOn = false;
    flashlightOn = !flashlightOn;
    _mainShader.use();
    glUniform1f(_mainShader.getUniLoc("flashlight.outerBorder"), flashlightOn ? glm::cos(glm::radians(17.5f)) : 2.f);
}
