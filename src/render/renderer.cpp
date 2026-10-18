#include "renderer.hpp"

#include "glm/trigonometric.hpp"
#include "src/core/logger.hpp"

#include "glad/glad.h"
#include "glm/gtc/type_ptr.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "src/render/shader.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace {
    constexpr float lightPos[3] = { 2.f, 3.f, -1.f };
    constexpr float lightColor[3] = { 1.f, 1.f, 1.f };
    
    constexpr GLfloat cube[] {
        //  position        texCoord        normales
        // front side
        -1.f, -1.f,  1.f,   0.f, 0.f,    0.f,  0.f,  1.f,
         1.f, -1.f,  1.f,   1.f, 0.f,    0.f,  0.f,  1.f,
        -1.f,  1.f,  1.f,   0.f, 1.f,    0.f,  0.f,  1.f,
         1.f,  1.f,  1.f,   1.f, 1.f,    0.f,  0.f,  1.f,
        // back side
        -1.f, -1.f, -1.f,   0.f, 0.f,    0.f,  0.f, -1.f,
         1.f, -1.f, -1.f,   1.f, 0.f,    0.f,  0.f, -1.f,
        -1.f,  1.f, -1.f,   0.f, 1.f,    0.f,  0.f, -1.f,
         1.f,  1.f, -1.f,   1.f, 1.f,    0.f,  0.f, -1.f,
        // top side
        -1.f,  1.f,  1.f,   0.f, 0.f,    0.f,  1.f,  0.f,
        -1.f,  1.f, -1.f,   0.f, 1.f,    0.f,  1.f,  0.f,
         1.f,  1.f,  1.f,   1.f, 0.f,    0.f,  1.f,  0.f,
         1.f,  1.f, -1.f,   1.f, 1.f,    0.f,  1.f,  0.f,
        // bottom side
        -1.f, -1.f,  1.f,   0.f, 0.f,    0.f, -1.f,  0.f,
         1.f, -1.f,  1.f,   1.f, 0.f,    0.f, -1.f,  0.f,
        -1.f, -1.f, -1.f,   0.f, 1.f,    0.f, -1.f,  0.f,
         1.f, -1.f, -1.f,   1.f, 1.f,    0.f, -1.f,  0.f,
        // left side
        -1.f, -1.f,  1.f,   0.f, 0.f,   -1.f,  0.f,  0.f,
        -1.f, -1.f, -1.f,   1.f, 0.f,   -1.f,  0.f,  0.f,
        -1.f,  1.f,  1.f,   0.f, 1.f,   -1.f,  0.f,  0.f,
        -1.f,  1.f, -1.f,   1.f, 1.f,   -1.f,  0.f,  0.f,
        // rigth side
         1.f, -1.f,  1.f,   0.f, 0.f,    1.f,  0.f,  0.f,
         1.f, -1.f, -1.f,   1.f, 0.f,    1.f,  0.f,  0.f,
         1.f,  1.f,  1.f,   0.f, 1.f,    1.f,  0.f,  0.f,
         1.f,  1.f, -1.f,   1.f, 1.f,    1.f,  0.f,  0.f,
    };

    constexpr GLuint indices[] = {
        0, 1, 2,
        2, 3, 1,

        4, 5, 6,
        6, 7, 5,

        8, 9, 10,
        10, 11, 9,

        12, 13, 14,
        14, 15, 13,

        16, 17, 18,
        18, 19, 17,

        20, 21, 22,
        22, 23, 21
    };

    void loadTexture(const char* texPath, const int texCount) {
        int x, y, channels;
        unsigned char* texData = stbi_load(texPath, &x, &y, &channels, 0);
        if (texData) {
            glActiveTexture(GL_TEXTURE0 + texCount);
            GLuint texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
    
            // what to do if texture will be smaller primitive
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            // texture filtering
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            GLint texFormat = channels == 3 ? GL_RGB : GL_RGBA;
            glTexImage2D(
                GL_TEXTURE_2D,
                0,              // autogenerate mipmap
                texFormat,
                x,
                y,
                0,              // legacy parameter
                texFormat,
                GL_UNSIGNED_BYTE,
                texData
            );

            stbi_image_free(texData);
        }
        else Logger::getInstance().printErr("load texture failed");
    }
}

Renderer::Renderer()
    : _shader1(Shader("object.vert", "object.frag"))
    , _shader2(Shader("light.vert", "light.frag"))
    , _vao(0), _vao2(0), _vbo(0), _ebo(0)
    , _aspectRatio(1.f)
    , _polygonMode(PolygonMode::FILL)
{
    if (_shader1.id && _shader2.id) {
        glGenVertexArrays(1, &_vao);
        glBindVertexArray(_vao);

        glGenBuffers(1, &_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, (GLvoid*)0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, (GLvoid*)(5 * sizeof(GLfloat)));

        glGenBuffers(1, &_ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        // light VAO
        glGenVertexArrays(1, &_vao2);
        glBindVertexArray(_vao2);

        glBindBuffer(GL_ARRAY_BUFFER, _vbo);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, (GLvoid*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, (GLvoid*)(5 * sizeof(GLfloat)));

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);

        glBindVertexArray(0);

        _shader1.use();
        _shader1.setUniLoc(UniName::TRANSFORM_MTX, "transformMatrix");
        _shader1.setUniLoc(UniName::PROJECTION_MTX, "projectionMatrix");
        _shader1.setUniLoc(UniName::VIEW_MTX, "viewMatrix");
        _shader1.setUniLoc(UniName::VIEW_POS, "viewPos");

        _shader1.setUniLoc("light.position");
        _shader1.setUniLoc("light.ambient");
        _shader1.setUniLoc("light.diffuse");
        _shader1.setUniLoc("light.specular");

        _shader1.setUniLoc("material.ambient");
        _shader1.setUniLoc("material.diffuse");
        _shader1.setUniLoc("material.specular");
        _shader1.setUniLoc("material.shininess");

        glUniform3f(
            _shader1.getUniLoc(UniName::VIEW_POS),
            _camera.position().x, _camera.position().y, _camera.position().z
        );
        
        glUniform3f(
            _shader1.getUniLoc("light.position"), lightPos[0], lightPos[1], lightPos[2]
        );
        glUniform3f(
            _shader1.getUniLoc("light.diffuse"), lightColor[0], lightColor[1], lightColor[2]
        );
        glUniform3f(
            _shader1.getUniLoc("light.ambient"), 0.05f, 0.05f, 0.05f
        );
        glUniform3f(
            _shader1.getUniLoc("light.specular"), 1.f, 1.f, 1.f
        );

        glUniform3f(
            _shader1.getUniLoc("material.ambient"), 0.19225f, 0.19225f, 0.19225f
        );
        glUniform3f(
            _shader1.getUniLoc("material.diffuse"), 0.50754f, 0.50754f, 0.50754f
        );
        glUniform3f(
            _shader1.getUniLoc("material.specular"), 0.508273f, 0.508273f, 0.508273f
        );
        glUniform1f(
            _shader1.getUniLoc("material.shininess"), 64.f
        );

        _shader2.use();
        _shader2.setUniLoc(UniName::TRANSFORM_MTX, "transformMatrix");
        _shader2.setUniLoc(UniName::PROJECTION_MTX, "projectionMatrix");
        _shader2.setUniLoc(UniName::VIEW_MTX, "viewMatrix");
        _shader2.setUniLoc(UniName::LIGHT_COLOR, "lightColor");
        
        glUniform3f(_shader2.getUniLoc(UniName::LIGHT_COLOR), lightColor[0], lightColor[1], lightColor[2]);
        
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.0f, 0.0f, 0.0f, 1.f);
    }
}

Renderer::~Renderer() {
    if (_vao) glDeleteVertexArrays(1, &_vao);
    if (_vao2) glDeleteVertexArrays(1, &_vao2);
    if (_vbo) glDeleteBuffers(1, &_vbo);
    if (_ebo) glDeleteBuffers(1, &_ebo);
}

void Renderer::draw() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 transform(1.f);
    glm::mat4 projection = glm::perspective(glm::radians(_camera.fov()), _aspectRatio, 0.1f, 100.f);
    glm::mat4 view = glm::lookAt(
        _camera.position(),
        _camera.position() + _camera.front(),
        _camera.up()
    );

    _shader1.use();
    glUniformMatrix4fv(_shader1.getUniLoc(UniName::PROJECTION_MTX), 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(_shader1.getUniLoc(UniName::VIEW_MTX), 1, GL_FALSE, glm::value_ptr(view));

    glUniform3f(
        _shader1.getUniLoc(UniName::VIEW_POS),
        _camera.position().x, _camera.position().y, _camera.position().z
    );
        
    glBindVertexArray(_vao);

    transform = glm::translate(transform, glm::vec3(0.f, 0.f, -5.f));
    glUniformMatrix4fv(_shader1.getUniLoc(UniName::TRANSFORM_MTX), 1, GL_FALSE, glm::value_ptr(transform));
    
    glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(GLuint), GL_UNSIGNED_INT, 0);

    _shader2.use();
    glUniformMatrix4fv(_shader2.getUniLoc(UniName::PROJECTION_MTX), 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(_shader2.getUniLoc(UniName::VIEW_MTX), 1, GL_FALSE, glm::value_ptr(view));
    
    glBindVertexArray(_vao2);
    transform = 1.f;
    transform = glm::translate(transform, glm::vec3(lightPos[0], lightPos[1], lightPos[2]));
    transform = glm::scale(transform, glm::vec3(0.3f, 0.3f, 0.3f));
    
    glUniformMatrix4fv(_shader2.getUniLoc(UniName::TRANSFORM_MTX), 1, GL_FALSE, glm::value_ptr(transform));
    
    glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(GLuint), GL_UNSIGNED_INT, 0);
}

void Renderer::setPolygonMode(const PolygonMode newMode) {
    _polygonMode = newMode;
    switch (newMode) {
        case PolygonMode::FILL: glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); break;
        case PolygonMode::LINE: glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); break;
    }
}