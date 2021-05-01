/* this is heavily inspired (almost copied) by Learn OpenGL from Joey de Vries 
   https://learnopengl.com/Model-Loading/Mesh */
#pragma once
#include <iostream>
#include <vector>
#include <string>

#include "stb_image.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "mesh.h"
#include "shader.h"

class Model
{
    public:
        Model(char *path)
        {
            loadModel(path);
        }
        void Draw(Shader &shader);
    private:
        std::vector<Mesh> meshes;
        std::string directory;

        void loadModel(std::string path);
        void processNode(aiNode *node, const aiScene *scene);
        Mesh processMesh(aiMesh *mesh, const aiScene *scene);
        std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type,
                                                  std::string typeName);
        unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma);

};