#pragma once

#include "src/render/model/mesh.hpp"
#include "src/render/model/helpData.hpp"
#include "src/render/shader.hpp"

#include <assimp/scene.h>

#include <vector>
#include <string>

class Model {
    std::vector<Mesh> _meshes;
    std::vector<Texture> _loadedTextures;
    std::string _directory;

    void _loadModel(const std::string& modelPath);
    void _processNode(aiNode* node, const aiScene* scene);
    auto _processMesh(aiMesh* mesh, const aiScene* scene) -> Mesh;
    auto _loadMaterialTexs(aiMaterial* material, aiTextureType type, TexType texType) 
        -> std::vector<Texture>;

public:
    void loadModel(const char* modelPath) { _loadModel(modelPath); }

    void draw(const Shader& shader) const {
        shader.use();
        for (const Mesh& mesh : _meshes)
            mesh.draw(shader);
    }
};
