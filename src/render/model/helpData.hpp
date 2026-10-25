#pragma once

#include <glm/glm.hpp>

#include <string>

enum class TexType: char {
    DIFFUSE,
    SPECULAR,
};

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
};

struct Texture {
    unsigned int id;
    TexType type;
    std::string path;
};