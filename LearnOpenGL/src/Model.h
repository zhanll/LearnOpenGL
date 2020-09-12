#pragma once
#include <vector>
#include <string>
#include "Mesh.h"
#include "glad/glad.h"

class Shader;
struct aiNode;
struct aiScene;
struct aiMesh;
struct aiMaterial;
enum aiTextureType;

class Model
{
public:
    Model() {}
    Model(const char* path)
    {
        loadModel(path);
    }
	void SetTextureWrapS(unsigned int S);
	void SetTextureWrapT(unsigned int T);
    void loadModel(std::string path);
    void Draw(Shader& shader);
   
private:
    // model data
    std::vector<Mesh> meshes;
    std::vector<Texture> textures_loaded;
    std:: string directory;
    unsigned int textureWrapS = GL_REPEAT;
    unsigned int textureWrapT = GL_REPEAT;

    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
    unsigned int TextureFromFile(const char* path, const std::string& directory);
};