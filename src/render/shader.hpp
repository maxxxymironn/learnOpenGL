#pragma once
#include <unordered_map>
#include <string>

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
    std::unordered_map<std::string, int> uniStrLocations;

    Shader(const char* vertexFilename, const char* fragmentFilename);
    ~Shader();

    void use();
    
    void setUniLoc(const UniName uniform, const char* uniName);
    void setUniLoc(const std::string& uniform);
    
    int getUniLoc(const UniName uniform);
    int getUniLoc(const std::string& uniform);
};