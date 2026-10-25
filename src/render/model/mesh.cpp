#include "src/render/model/mesh.hpp"
#include "src/core/logger.hpp"
#include "src/render/model/helpData.hpp"

#include <glad/glad.h>

#include <string>
#include <iostream>

void Mesh::_setupMesh() {
    glGenVertexArrays(1, &_vao);
    glGenBuffers(1, &_vbo);
    glGenBuffers(1, &_ebo);

    glBindVertexArray(_vao);

    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * _vertices.size(), _vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * _indices.size(), _indices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0, sizeof(Vertex::position) / sizeof(float),
        GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(offsetof(Vertex, position))
    );
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1, sizeof(Vertex::normal) / sizeof(float),
        GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(offsetof(Vertex, normal))
    );
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(
        2, sizeof(Vertex::texCoords) / sizeof(float),
        GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(offsetof(Vertex, texCoords))
    );

    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

Mesh::Mesh(
    const std::vector<Vertex>& vertices,
    const std::vector<uint>& indices,
    const std::vector<Texture>& textures
): _vao(0), _vbo(0), _ebo(0), _vertices(vertices)
 , _indices(indices), _textures(textures) { _setupMesh(); }

Mesh::~Mesh() {
    // if (_vao) glDeleteVertexArrays(1, &_vao);
    // if (_vbo) glDeleteBuffers(1, &_vbo);
    // if (_ebo) glDeleteBuffers(1, &_ebo);
}

void Mesh::draw(const Shader& shader) const {
    uint diffuseTexNum = 1;
    uint specularTexNum = 1;

    for (uint i = 0; i < _textures.size(); ++i) {
        glActiveTexture(GL_TEXTURE0 + i);
        std::string uniformTexName("material.texture_");

        const bool isDiffuseTex = _textures[i].type == TexType::DIFFUSE;
        uniformTexName += isDiffuseTex ? "diffuse" : "specular";
        uniformTexName += std::to_string(isDiffuseTex ? diffuseTexNum++ : specularTexNum++);
        
        glUniform1i(glGetUniformLocation(shader.id, uniformTexName.c_str()), i);
        glBindTexture(GL_TEXTURE_2D, _textures[i].id);
    }
    
    glBindVertexArray(_vao);
    glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glActiveTexture(GL_TEXTURE0);
}