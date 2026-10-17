#pragma once
#include <unordered_map>

enum class UniName: unsigned char {
    TRANSFORM_MTX,
    PROJECTION_MTX,
    VIEW_MTX,
    CUBE_COLOR,
    LIGHT_COLOR
};

struct Shader {
    unsigned int id;
    std::unordered_map<UniName, int> uniLocations;

    Shader(const char* vertexFilename, const char* fragmentFilename);
    ~Shader();

    void use();
    
    void setUniLoc(const UniName uniform, const char* uniName);
    int getUniLoc(const UniName uniform) { return uniLocations[uniform]; }
};