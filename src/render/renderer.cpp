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

#include <string>

namespace {
    bool _toggleOn = false;

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

    constexpr glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f),
        glm::vec3( 2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3( 2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3( 1.3f, -2.0f, -2.5f),
        glm::vec3( 1.5f,  2.0f, -2.5f),
        glm::vec3( 1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };

    constexpr float POINT_COL[] = { 0.5f, 0.3f, 0.f };
    
    constexpr int POINT_LIGHT_NUM = 5;
    constexpr glm::vec3 lightPointPos[POINT_LIGHT_NUM] = {
        glm::vec3(-1.f, 3.f, -2.f),
       	glm::vec3( 0.7f,  0.2f,  2.0f),
    	glm::vec3( 2.3f, -3.3f, -4.0f),
    	glm::vec3(-4.0f,  2.0f, -12.0f),
    	glm::vec3( 0.0f,  0.0f, -3.0f)
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
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, (GLvoid*)(3 * sizeof(GLfloat)));

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, (GLvoid*)(5 * sizeof(GLfloat)));

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
        /* getting uniforms locations */
        // matrixes
        _shader1.setUniLoc(UniName::TRANSFORM_MTX, "transformMatrix");
        _shader1.setUniLoc(UniName::PROJECTION_MTX, "projectionMatrix");
        _shader1.setUniLoc(UniName::VIEW_MTX, "viewMatrix");

        // miscellaneous
        _shader1.setUniLoc("ambientLight");
        _shader1.setUniLoc(UniName::VIEW_POS, "viewPos");

        // direct light
        _shader1.setUniLoc("directLight.direction");
        _shader1.setUniLoc("directLight.diffuse");
        _shader1.setUniLoc("directLight.specular");

        // point light
        for (int i = 0; i < POINT_LIGHT_NUM; ++i) {
            std::string uniformNum("pointLight[");
            uniformNum += '0' + i;
            uniformNum += "].";

            std::string uniformAttrib = uniformNum + "position";
            _shader1.setUniLoc(uniformAttrib);
            uniformAttrib = uniformNum + "diffuse";
            _shader1.setUniLoc(uniformAttrib);
            uniformAttrib = uniformNum + "specular";
            _shader1.setUniLoc(uniformAttrib);
            uniformAttrib = uniformNum + "constant";
            _shader1.setUniLoc(uniformAttrib);
            uniformAttrib = uniformNum + "linear";
            _shader1.setUniLoc(uniformAttrib);
            uniformAttrib = uniformNum + "quadratic";
            _shader1.setUniLoc(uniformAttrib);
        }

        // flashlight (spotlight)
        _shader1.setUniLoc("flashlight.position");
        _shader1.setUniLoc("flashlight.direction");
        _shader1.setUniLoc("flashlight.innerBorder");
        _shader1.setUniLoc("flashlight.outerBorder");
        _shader1.setUniLoc("flashlight.diffuse");
        _shader1.setUniLoc("flashlight.specular");
        _shader1.setUniLoc("flashlight.constant");
        _shader1.setUniLoc("flashlight.linear");
        _shader1.setUniLoc("flashlight.quadratic");

        // material
        _shader1.setUniLoc("material.diffuse");
        _shader1.setUniLoc("material.specular");
        _shader1.setUniLoc("material.shininess");

        /* setting uniforms */
        // miscellaneous
        glUniform3f(
            _shader1.getUniLoc(UniName::VIEW_POS),
            _camera.position().x, _camera.position().y, _camera.position().z
        );
        glUniform3f(_shader1.getUniLoc("ambientLight"), 0.05f, 0.05f, 0.05f);

        // direct light
        glUniform3f(_shader1.getUniLoc("directLight.direction"), -1.f, 0.f, 0.f);
        glUniform3f(_shader1.getUniLoc("directLight.diffuse"), 0.3f, 0.3f, 0.3f);
        glUniform3f(_shader1.getUniLoc("directLight.specular"), 0.3f, 0.3f, 0.3f);

        // point light
        for (int i = 0; i < POINT_LIGHT_NUM; ++i) {
            std::string uniformNum("pointLight[");
            uniformNum += '0' + i;
            uniformNum += "].";

            std::string uniformAttrib = uniformNum + "position";
            glUniform3f(
                _shader1.getUniLoc(uniformAttrib),
                lightPointPos[i].x, lightPointPos[i].y, lightPointPos[i].z
            );
            uniformAttrib = uniformNum + "diffuse";
            glUniform3f(
                _shader1.getUniLoc(uniformAttrib), POINT_COL[0], POINT_COL[1], POINT_COL[2]
            );
            uniformAttrib = uniformNum + "specular";
            glUniform3f(
                _shader1.getUniLoc(uniformAttrib), POINT_COL[0], POINT_COL[1], POINT_COL[2]
            );
            uniformAttrib = uniformNum + "constant";
            glUniform1f(_shader1.getUniLoc(uniformAttrib), 1.f);
            uniformAttrib = uniformNum + "linear";
            glUniform1f(_shader1.getUniLoc(uniformAttrib), 0.4f);
            uniformAttrib = uniformNum + "quadratic";
            glUniform1f(_shader1.getUniLoc(uniformAttrib), 0.001f);
        }

        // flashlight (spotligh)
        glUniform3f(
            _shader1.getUniLoc("flashlight.position"),
            _camera.position().x, _camera.position().y, _camera.position().z
        );
        glUniform3f(
            _shader1.getUniLoc("flashlight.direction"),
            _camera.front().x, _camera.front().y, _camera.front().z
        );
        glUniform1f(_shader1.getUniLoc("flashlight.innerBorder"), glm::cos(glm::radians(12.5f)));
        glUniform1f(_shader1.getUniLoc("flashlight.outerBorder"), 2.f);
        glUniform3f(_shader1.getUniLoc("flashlight.diffuse"), 1.f, 1.f, 1.f);
        glUniform3f(_shader1.getUniLoc("flashlight.specular"), 1.f, 1.f, 1.f);
        glUniform1f(_shader1.getUniLoc("flashlight.constant"), 1.f);
        glUniform1f(_shader1.getUniLoc("flashlight.linear"), 0.01f);
        glUniform1f(_shader1.getUniLoc("flashlight.quadratic"), 0.001f);

        // material
        {
            int texCount = 0;
            std::string texPath(IMAGES_DIR);
            texPath += "container.png";
            loadTexture(texPath.c_str(), texCount);
            glUniform1i(_shader1.getUniLoc("material.diffuse"), texCount);
            ++texCount;

            texPath = IMAGES_DIR;
            texPath += "container_border.png";
            loadTexture(texPath.c_str(), texCount);
            glUniform1i(_shader1.getUniLoc("material.specular"), texCount);
            ++texCount;
        }
        glUniform1f(
            _shader1.getUniLoc("material.shininess"), 64.f
        );

        _shader2.use();
        _shader2.setUniLoc(UniName::TRANSFORM_MTX, "transformMatrix");
        _shader2.setUniLoc(UniName::PROJECTION_MTX, "projectionMatrix");
        _shader2.setUniLoc(UniName::VIEW_MTX, "viewMatrix");
        _shader2.setUniLoc(UniName::LIGHT_COLOR, "lightColor");

        glUniform3f(_shader2.getUniLoc(UniName::LIGHT_COLOR), POINT_COL[0], POINT_COL[1], POINT_COL[2]);

        glEnable(GL_DEPTH_TEST);
        glClearColor(0.2f, 0.2f, 0.2f, 1.f);
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

    glUniform3f(
        _shader1.getUniLoc("flashlight.position"),
        _camera.position().x, _camera.position().y, _camera.position().z
    );
    glUniform3f(
        _shader1.getUniLoc("flashlight.direction"),
        _camera.front().x, _camera.front().y, _camera.front().z
    );

    glBindVertexArray(_vao);

    for (int i = 0; i < 10; ++i) {
        float rotateDegree = 10.f * i;
        glm::mat4 transform(1.f);
        transform = glm::translate(transform, cubePositions[i] * 2.f);
        transform = glm::rotate(transform, glm::radians(rotateDegree), glm::vec3(1.f, 1.f, 0.f));
        glUniformMatrix4fv(_shader1.getUniLoc(UniName::TRANSFORM_MTX), 1, GL_FALSE, glm::value_ptr(transform));

        glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(GLuint), GL_UNSIGNED_INT, 0);
    }

    _shader2.use();
    glUniformMatrix4fv(_shader2.getUniLoc(UniName::PROJECTION_MTX), 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(_shader2.getUniLoc(UniName::VIEW_MTX), 1, GL_FALSE, glm::value_ptr(view));

    glBindVertexArray(_vao2);

    for (int i = 0; i < POINT_LIGHT_NUM; ++i) {
        glm::mat4 transform = 1.f;
        transform = glm::translate(transform, lightPointPos[i]);
        transform = glm::scale(transform, glm::vec3(0.3f, 0.3f, 0.3f));
    
        glUniformMatrix4fv(_shader2.getUniLoc(UniName::TRANSFORM_MTX), 1, GL_FALSE, glm::value_ptr(transform));
    
        glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(GLuint), GL_UNSIGNED_INT, 0);
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
    _toggleOn = !_toggleOn;
    _shader1.use();
    glUniform1f(_shader1.getUniLoc("flashlight.outerBorder"), _toggleOn ? glm::cos(glm::radians(17.5f)) : 2.f);
}
