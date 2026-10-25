#include "shader.hpp"

#include "src/core/logger.hpp"

#include "glad/glad.h"

#include <stdexcept>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>

namespace {
    std::string loadShaderSource(const char* filename) {
        std::string shaderPath(SHADERS_DIR);
        shaderPath += filename;

        std::ifstream file(shaderPath);
        if (!file.is_open()) {
            std::string err("Open shader '");
            err += filename;
            err += "' failed";
            
            Logger::getInstance().printErr(err.c_str());
            return "";
        }
        
        file.exceptions(std::ios::badbit | std::ios::failbit);
        try {
            std::stringstream ss;
            ss << file.rdbuf();
            return ss.str();
        }
        catch (const std::ifstream::failure&) {
            std::string err("Read shader '");
            err += filename;
            err += "' failed";
            
            Logger::getInstance().printErr("Read shader file failed");
            return "";
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

    GLuint linkShaderProgram(const char* vertexFilename, const char* fragmentFilename) {
        std::string shaderSource = loadShaderSource(vertexFilename);
        if (shaderSource.empty())
            return 0;

        GLuint vertexShader = compileShader(GL_VERTEX_SHADER, shaderSource.c_str());
        if (!vertexShader)
            return 0;

        shaderSource = loadShaderSource(fragmentFilename);
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

Shader::Shader(const char* vertexFilename, const char* fragmentFilename)
    : id(linkShaderProgram(vertexFilename, fragmentFilename)) {}

Shader::~Shader() { if (id) glDeleteProgram(id); }

void Shader::use() const { glUseProgram(id); }

void Shader::setUniLoc(const UniName uniform, const char* uniName) {
    uniLocations[uniform] = glGetUniformLocation(id, uniName);
    if (uniLocations[uniform] == -1) {
        std::string msg("Uniform '");
        msg += uniName; 
        msg += "' not found";
        Logger::getInstance().printErr(msg.c_str());
    }
}

void Shader::setUniLoc(const std::string& uniform) {
    uniStrLocations[uniform.data()] = glGetUniformLocation(id, uniform.data());
    if (uniStrLocations[uniform.data()] == -1) {
        std::string msg("Uniform '");
        msg += uniform; 
        msg += "' not found";
        Logger::getInstance().printErr(msg.c_str());
    }
}

int Shader::getUniLoc(const UniName uniform) {
    auto it = uniLocations.find(uniform);
    if (it == uniLocations.end()) {
        std::string err("Uniform with type_id='");
        err += static_cast<int>(uniform);
        err += "' not created";
        Logger::getInstance().printErr(err.c_str());
        throw;
    }
    return it->second;
}

int Shader::getUniLoc(const std::string& uniform) {
    auto it = uniStrLocations.find(uniform);
    if (it == uniStrLocations.end()) {
        std::string err("Uniform with name='");
        err += uniform;
        err += "' not created";
        Logger::getInstance().printErr(err.c_str());
        throw std::runtime_error(err.c_str());
    }
    return it->second;
}