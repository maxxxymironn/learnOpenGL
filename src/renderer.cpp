#include "renderer.hpp"
#include "logger.hpp"

#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <sstream>
#include <string>
#include <fstream>
#include <iostream>

namespace {
    GLint transformMatrixUniformLocation = 0;
    GLint colorCoeffUniformLocation = 0;
    
    constexpr GLfloat vertices[] {
        -0.5f, -0.5f,   1.f, 0.f, 0.f,   0.f, 0.f,
         0.5f, -0.5f,   0.f, 1.f, 0.f,   1.f, 0.f,
        -0.5f,  0.5f,   1.f, 0.f, 1.f,   0.f, 1.f,
         0.5f,  0.5f,   0.f, 0.f, 1.f,   1.f, 1.f,
    };

    constexpr GLuint indices[] = {
        0, 1, 2,
        1, 2, 3
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
    : _vao(0)
    , _vbo(0)
    , _ebo(0)
    , _shader(linkShaderProgram())
    , _texture(0)
    , _textureData(nullptr)
{
    if (_shader) {
        // 1) bind vao
        glGenVertexArrays(1, &_vao);
        glBindVertexArray(_vao);

        // 2) bind and setting vbo
        glGenBuffers(1, &_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        // copy my data to buffer
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        // 3) tell how to interpret data
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(
            0,                      // describe shader layout = 0
            2,                      // shader layout size = 2 (vec2)
            GL_FLOAT,               // shader var uses float
            GL_FALSE,               // do not normalize [0, 1] variable
            sizeof(GLfloat) * 7,    // stride between data
            (GLvoid*)0              // offset
        );

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(
            1, 3, GL_FLOAT, GL_FALSE, 
            sizeof(GLfloat) * 7,
            (GLvoid*)(sizeof(GLfloat) * 2)
        );

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(
            2, 2, GL_FLOAT, GL_FALSE,
            sizeof(GLfloat) * 7,
            (GLvoid*)(sizeof(GLfloat) * 5)
        );

        // 4) bind and setting ebo
        glGenBuffers(1, &_ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        // unbind all objects
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        glClearColor(0.1f, 0.1f, 0.1f, 1.f);
        glBindVertexArray(_vao);
        glUseProgram(_shader);

        // uniforms
        colorCoeffUniformLocation = glGetUniformLocation(_shader, "colorCoeff");
        if (colorCoeffUniformLocation == -1)
            Logger::getInstance().printErr("Uniform 'colorCoeff' not found in shaders");
        
        transformMatrixUniformLocation = glGetUniformLocation(_shader, "transformMatrix");
        if (transformMatrixUniformLocation == -1)
            Logger::getInstance().printErr("Uniform 'transformMatrix' not found");

        /* textures */
        stbi_set_flip_vertically_on_load(true);
        int textureCount = 0;
        
        // load texture №1
        std::string imagePath(IMAGES_DIR);
        imagePath += "tex1.png";
        loadTexture(imagePath.c_str(), textureCount);
        glUniform1i(glGetUniformLocation(_shader, "tex1"), textureCount);
        ++textureCount;
        
        // load texture №2
        imagePath = IMAGES_DIR;
        imagePath += "tex2.jpg";
        loadTexture(imagePath.c_str(), textureCount);
        glUniform1i(glGetUniformLocation(_shader, "tex2"), textureCount);
        ++textureCount;
    }
}

Renderer::~Renderer() {
    if (_shader) glDeleteProgram(_shader);
    if (_vao) glDeleteVertexArrays(1, &_vao);
    if (_vbo) glDeleteBuffers(1, &_vbo);
    if (_ebo) glDeleteBuffers(1, &_ebo);
    
}

void Renderer::setWireframeMode() { glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); }
void Renderer::setNormalMode() { glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); }

void Renderer::draw() {
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(GLuint), GL_UNSIGNED_INT, 0);
}

void Renderer::updateColorCoeffUni(const float newValue) { 
    glUniform1f(colorCoeffUniformLocation, newValue);
}

void Renderer::updateTransformMatrixUni(const float deegres, const float moveX, const float moveY) {
    glm::mat4 transform(1.f);
    transform = glm::translate(transform, glm::vec3(moveX, moveY, 0.f));
    transform = glm::rotate(transform, glm::radians(deegres), glm::vec3(0.f, 0.f, 1.f));
    glUniformMatrix4fv(transformMatrixUniformLocation, 1, GL_FALSE, glm::value_ptr(transform));
}
