#include "model.hpp"

#include "src/core/logger.hpp"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/material.h>
#include <assimp/types.h>
#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stbi_image/stb_image.h>

#include <string>
#include <iostream>

namespace {
    auto loadTexture(const char* texPath) -> unsigned int {
        uint texture = 0;
        int x, y, channels;
        stbi_set_flip_vertically_on_load(true);
        unsigned char* texData = stbi_load(texPath, &x, &y, &channels, 0);
        if (texData) {
            // glActiveTexture(GL_TEXTURE0 + texCount);
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);

            // what to do if texture will be smaller primitive
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            // texture filtering
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            uint texFormat = channels == 3 ? GL_RGB : GL_RGBA;
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
            glGenerateMipmap(GL_TEXTURE_2D);

            stbi_image_free(texData);
        }
        else {
            std::string errMsg("load texture failed at path: ");
            errMsg += texPath;
            Logger::getInstance().printErr(errMsg.c_str());
        }

        return texture;
    }
}

void Model::_loadModel(const std::string& path) {
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::string errMsg("ASSIMP: ");
        errMsg += importer.GetErrorString();
        Logger::getInstance().printErr(errMsg.c_str());
        return;
    }

    _directory = path.substr(0, path.find_last_of('/'));
    
    _processNode(scene->mRootNode, scene);
}

void Model::_processNode(aiNode* node, const aiScene* scene) {
    // process all meshes in node    
    for (uint i = 0; i < node->mNumMeshes; ++i) {    
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        _meshes.push_back(_processMesh(mesh, scene));
    }
    // process same work for all node's childs
    for (uint i = 0; i < node->mNumChildren; ++i) {
        _processNode(node->mChildren[i], scene);
    }
}

auto Model::_processMesh(aiMesh* mesh, const aiScene* scene) -> Mesh {
    std::vector<Vertex> vertices;
    std::vector<uint> indices;
    std::vector<Texture> textures;

    // process coords, normal and texCoords
    for (uint i = 0; i < mesh->mNumVertices; ++i) {
        Vertex vertex;
        vertex.position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
        vertex.normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };
        vertex.texCoords = mesh->mTextureCoords[0]
                         ? glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y)
                         : glm::vec2(0.f, 0.f);
        vertices.push_back(vertex);
    }

    // process indices
    for (uint i = 0; i < mesh->mNumFaces; ++i) {
        aiFace face = mesh->mFaces[i];
        for (uint j = 0; j < face.mNumIndices; ++j)
            indices.push_back(face.mIndices[j]);
    }

    // process material
    if (mesh->mMaterialIndex >= 0) {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        std::vector<Texture> diffuseMaps = _loadMaterialTexs(
            material, aiTextureType_DIFFUSE, TexType::DIFFUSE
        );
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

        std::vector<Texture> specularMaps = _loadMaterialTexs(
            material, aiTextureType_SPECULAR, TexType::SPECULAR
        );
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    }

    return Mesh(vertices, indices, textures);
}

auto Model::_loadMaterialTexs(aiMaterial* material, aiTextureType type, TexType texType)
    -> std::vector<Texture>
{
    std::vector<Texture> textures;
    for (uint i = 0; i < material->GetTextureCount(type); ++i) {
        aiString texFilename;
        material->GetTexture(type, i, &texFilename);

        bool skip = false;
        for (uint j = 0; j < _loadedTextures.size(); ++j) {
            if (std::strcmp(_loadedTextures[j].path.c_str(), texFilename.C_Str()) == 0) {
                textures.push_back(_loadedTextures[j]);
                skip = true;
                break;
            }
        }

        if (!skip) {
            std::string texPath(_directory + '/');
            texPath += texFilename.C_Str();
            Texture texture({ loadTexture(texPath.c_str()), texType, texFilename.C_Str() });
            textures.push_back(texture);
            _loadedTextures.push_back(texture);
        }
    }

    return textures;
}
