/* this is heavily (almost copied) inspired by Learn OpenGL from Joey de Vries 
   https://learnopengl.com/Model-Loading/Mesh */

#pragma once
#include <iostream>
#include <vector>
#include <string>

#include "shader.h"
#include "glm/glm.hpp"
#include "utils.h"

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

struct Texture{
    unsigned int id;
    std::string type;
    std::string path;
};

class Mesh {
    public: 
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture> textures;

        Mesh(std::vector<Vertex> verticies, std::vector<unsigned int> indicies,
            std::vector<Texture> textures);
        void Draw(Shader &shader);

    private:
        unsigned int VAO, VBO, EBO;

        void setupMesh();
};
