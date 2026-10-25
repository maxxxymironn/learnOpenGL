#pragma once

#include "src/render/model/helpData.hpp"
#include "src/render/shader.hpp"

class Mesh {
    unsigned int _vao, _vbo, _ebo;

    std::vector<Vertex> _vertices;
    std::vector<unsigned int> _indices;
    std::vector<Texture> _textures;

    void _setupMesh();
    
public:
    Mesh() = default;
    Mesh(
        const std::vector<Vertex>& vertices,
        const std::vector<uint>& indices,
        const std::vector<Texture>& textures
    );
    ~Mesh();

    void draw(const Shader& shader) const;
};
