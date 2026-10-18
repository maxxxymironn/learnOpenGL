#pragma once
#include <unordered_map>

enum class UniName: unsigned char {
    TRANSFORM_MTX,
    PROJECTION_MTX,
    VIEW_MTX,
    OBJECT_COLOR,
    LIGHT_COLOR,
    LIGHT_POS,
    VIEW_POS,
};

struct Shader {
    unsigned int id;
    std::unordered_map<UniName, int> uniLocations;
    std::unordered_map<const char*, int> uniStrLocations;

    Shader(const char* vertexFilename, const char* fragmentFilename);
    ~Shader();

    void use();
    
    void setUniLoc(const UniName uniform, const char* uniName);
    void setUniLoc(const char* uniform);
    
    int getUniLoc(const UniName uniform);
    int getUniLoc(const char* uniform);
};