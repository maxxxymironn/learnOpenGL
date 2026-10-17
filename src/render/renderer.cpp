#include "renderer.hpp"

#include "glm/trigonometric.hpp"
#include "src/core/logger.hpp"

#include "glad/glad.h"
#include "glm/gtc/type_ptr.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <sstream>
#include <string>
#include <fstream>
#include <iostream>

namespace {
    GLint viewMatrixUniLoc = 0;
    GLint projectionMatrixUniLoc = 0;
    GLint transformMatrixUniLoc = 0;

    constexpr GLfloat cube[] {
        // front side
        -1.f, -1.f,  1.f,   0.f, 0.f,
         1.f, -1.f,  1.f,   1.f, 0.f,
        -1.f,  1.f,  1.f,   0.f, 1.f,
         1.f,  1.f,  1.f,   1.f, 1.f,
        // back side
        -1.f, -1.f, -1.f,   0.f, 0.f,
         1.f, -1.f, -1.f,   1.f, 0.f,
        -1.f,  1.f, -1.f,   0.f, 1.f,
         1.f,  1.f, -1.f,   1.f, 1.f,
        // top side
        -1.f,  1.f,  1.f,   0.f, 0.f,
        -1.f,  1.f, -1.f,   0.f, 1.f,
         1.f,  1.f,  1.f,   1.f, 0.f,
         1.f,  1.f, -1.f,   1.f, 1.f,
        // bottom side
        -1.f, -1.f,  1.f,   0.f, 0.f,
         1.f, -1.f,  1.f,   1.f, 0.f,
        -1.f, -1.f, -1.f,   0.f, 1.f,
         1.f, -1.f, -1.f,   1.f, 1.f,
        // left side
        -1.f, -1.f,  1.f,   0.f, 0.f,
        -1.f, -1.f, -1.f,   1.f, 0.f,
        -1.f,  1.f,  1.f,   0.f, 1.f,
        -1.f,  1.f, -1.f,   1.f, 1.f,
        // rigth side
         1.f, -1.f,  1.f,   0.f, 0.f,
         1.f, -1.f, -1.f,   1.f, 0.f,
         1.f,  1.f,  1.f,   0.f, 1.f,
         1.f,  1.f, -1.f,   1.f, 1.f,
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

    std::string loadShaderSource(const char* filename) {
        std::string shaderPath(SHADERS_DIR);
        shaderPath += filename;

        std::ifstream file(shaderPath);
        if (!file.is_open()) {
            Logger::getInstance().printErr("Open shader file failed");
            return nullptr;
        }
        
        file.exceptions(std::ios::badbit | std::ios::failbit);
        try {
            std::stringstream ss;
            ss << file.rdbuf();
            return ss.str();
        }
        catch (const std::ifstream::failure&) {
            Logger::getInstance().printErr("Read shader file failed");
            return nullptr;
        }
    }

    GLuint compileShader(GLenum shaderType, const char* shaderSource) {
        GLuint shader = glCreateShader(shaderType);
        glShaderSource(shader, 1, &shaderSource, nullptr);
        glCompileShader(shader);

        GLint success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            GLint logLength = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);

            std::string log(logLength, '\0');
            glGetShaderInfoLog(shader, logLength, nullptr, log.data());

            Logger::getInstance().printErr(log.c_str());
            return 0;
        }
        return shader;
    }

    GLuint linkShaderProgram() {
        std::string shaderSource = loadShaderSource("vertex.glsl");
        if (shaderSource.empty())
            return 0;

        GLuint vertexShader = compileShader(GL_VERTEX_SHADER, shaderSource.c_str());
        if (!vertexShader)
            return 0;

        shaderSource = loadShaderSource("fragment.glsl");
        if (shaderSource.empty())
            return 0;

        GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, shaderSource.c_str());
        if (!fragmentShader)
            return 0;

        GLuint program = glCreateProgram();
        glAttachShader(program, vertexShader);
        glAttachShader(program, fragmentShader);
        glLinkProgram(program);

        GLint success;
        glGetProgramiv(program, GL_LINK_STATUS, &success);
        if (!success) {
            GLint logLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);

            std::string log(logLength, '\0');
            glGetProgramInfoLog(program, logLength, nullptr, log.data());

            Logger::getInstance().printErr(log.c_str());
            return 0;
        }

        glDetachShader(program, fragmentShader);
        glDetachShader(program, vertexShader);
        glDeleteShader(fragmentShader);
        glDeleteShader(vertexShader);

        return program;
    }

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
    : _vao(0), _vbo(0), _ebo(0)
    , _shader(linkShaderProgram())
    , _aspectRatio(1.f)
    , _polygonMode(PolygonMode::FILL)
{
    if (_shader) {
        // 1) bind vao
        glGenVertexArrays(1, &_vao);
        glBindVertexArray(_vao);

        // 2) bind and setting vbo
        glGenBuffers(1, &_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);

        glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 5, (GLvoid*)0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 5, (GLvoid*)(sizeof(GLfloat) * 3));

        // 4) bind and setting ebo
        glGenBuffers(1, &_ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        // unbind all objects
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        glBindVertexArray(_vao);
        glUseProgram(_shader);

        /* uniforms */
        // colorCoeffUniLoc = glGetUniformLocation(_shader, "colorCoeff");
        // if (colorCoeffUniLoc == -1)
        //     Logger::getInstance().printErr("Uniform 'colorCoeff' not found in shaders");
        
        transformMatrixUniLoc = glGetUniformLocation(_shader, "transformMatrix");
        if (transformMatrixUniLoc == -1)
            Logger::getInstance().printErr("Uniform 'transformMatrix' not found");

        projectionMatrixUniLoc = glGetUniformLocation(_shader, "projectionMatrix");
        if (projectionMatrixUniLoc == -1)
            Logger::getInstance().printErr("Uniform 'projectionMatrix' not found");

        viewMatrixUniLoc = glGetUniformLocation(_shader, "viewMatrix");
        if (viewMatrixUniLoc == -1)
            Logger::getInstance().printErr("Uniform 'viewMatrix' not found");
        
        /* textures */
        stbi_set_flip_vertically_on_load(true);
        int textureCount = 0;
        
        // load texture №1
        std::string imagePath(IMAGES_DIR);
        imagePath += "tex3.jpg";
        loadTexture(imagePath.c_str(), textureCount);
        glUniform1i(glGetUniformLocation(_shader, "tex1"), textureCount);
        ++textureCount;
        
        // load texture №2
        imagePath = IMAGES_DIR;
        imagePath += "tex2.jpg";
        loadTexture(imagePath.c_str(), textureCount);
        glUniform1i(glGetUniformLocation(_shader, "tex2"), textureCount);
        ++textureCount;

        // enable Z-test
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.f, 0.f, 0.f, 1.f);
    }
}

Renderer::~Renderer() {
    if (_shader) glDeleteProgram(_shader);
    if (_vao) glDeleteVertexArrays(1, &_vao);
    if (_vbo) glDeleteBuffers(1, &_vbo);
    if (_ebo) glDeleteBuffers(1, &_ebo);
}

void Renderer::draw() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 transform(1.f);
    transform = glm::translate(transform, glm::vec3(0.f, 0.f, -5.f));
    
    glm::mat4 projection = glm::perspective(glm::radians(_camera.fov()), _aspectRatio, 0.1f, 100.f);
    
    glm::mat4 view = glm::lookAt(
        _camera.position(),
        _camera.position() + _camera.front(),
        _camera.up()
    );

    glUniformMatrix4fv(transformMatrixUniLoc, 1, GL_FALSE, glm::value_ptr(transform));
    glUniformMatrix4fv(projectionMatrixUniLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(viewMatrixUniLoc, 1, GL_FALSE, glm::value_ptr(view));
    
    glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(GLuint), GL_UNSIGNED_INT, 0);
}

void Renderer::setPolygonMode(const PolygonMode newMode) {
    _polygonMode = newMode;
    switch (newMode) {
        case PolygonMode::FILL: glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); break;
        case PolygonMode::LINE: glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); break;
    }
}