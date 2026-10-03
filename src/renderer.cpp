#include "renderer.hpp"
#include "logger.hpp"

#include <glad/glad.h>

#include <sstream>
#include <string>
#include <fstream>

namespace {
    GLfloat vertices[] {
        -0.5f, -0.5f, 0.f,
         0.5f, -0.5f, 0.f,
         0.0f,  0.5f, 0.f,
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
}

Renderer::Renderer(): _vao(0), _vbo(0), _ebo(0), _shader(linkShaderProgram()) {
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
        glVertexAttribPointer(
            0,                      // describe shader layout = 0
            3,                      // shader layout size = 3 (vec3)
            GL_FLOAT,               // shader var uses float
            GL_FALSE,               // do not normalize [0, 1] variable
            sizeof(GLfloat) * 3,    // stride between data
            (GLvoid*)0              // offset
        );
        glEnableVertexAttribArray(0);

        // unbind all objects
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        glClearColor(0.2f, 0.3f, 0.3f, 1.f);
        glBindVertexArray(_vao);
        glUseProgram(_shader);
    }
}

Renderer::~Renderer() {
    if (_shader) glDeleteProgram(_shader);
    if (_vao) glDeleteVertexArrays(1, &_vao);
    if (_vbo) glDeleteBuffers(1, &_vbo);
}

void Renderer::draw() {
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}