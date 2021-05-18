//
// Created by matejs on 05/05/2021.
//
#include "utils.h"
void PrepareHandLoadedObject(){
    rockTexID = loadTexture("../data/handLoadedRock/Rock_Base_Color.jpg");
    CHECK_GL_ERROR();
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float) * rockNVertices, rockVertices, GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * sizeof(unsigned) * rockNTriangles, rockTriangles, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8* sizeof(float), 0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8* sizeof(float), (void*)(3*sizeof(float)));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8* sizeof(float), (void*)(6*sizeof(float)));

    CHECK_GL_ERROR();
    glBindVertexArray(0);
}

void DrawHandLoadedObject(Shader* shader){
    shader->use();
    glActiveTexture(GL_TEXTURE0);
    shader->setInt("material.texture_diffuse1", 0);
    glBindTexture(GL_TEXTURE_2D, rockTexID);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 3*rockNTriangles, GL_UNSIGNED_INT, 0);
}
void checkGLError(const char *where = 0, int line = 0) {
    GLenum err = glGetError();
    if (err == GL_NONE)
        return;

    std::string errString = "<unknown>";
    switch (err) {
        case GL_INVALID_ENUM:
            errString = "GL_INVALID_ENUM";
            break;
        case GL_INVALID_VALUE:
            errString = "GL_INVALID_VALUE";
            break;
        case GL_INVALID_OPERATION:
            errString = "GL_INVALID_OPERATION";
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            errString = "GL_INVALID_FRAMEBUFFER_OPERATION";
            break;
        case GL_OUT_OF_MEMORY:
            errString = "GL_OUT_OF_MEMORY";
            break;
        default:;
    }
    if (where == 0 || *where == 0)
        std::cerr << "GL error occurred: " << errString << std::endl;
    else
        std::cerr << "GL error occurred in " << where << ":" << line << ": " << errString << std::endl;
}

unsigned int loadTexture(const char *path)
{
    /* prepare height map */
    int width, height, nrComponents;
    unsigned char *texture = stbi_load(path, &width, &height, &nrComponents, 0);
    if(texture)
    {
        std::cout << "UTILS::LOAD TEXTURE::Successfully loaded texture " << path << std::endl;
    } else {
        std::cout << "UTILS::LOAD TEXTURE::Failed to load texture " << path << std::endl;
        return 0;
    }
    GLenum format;
    if(nrComponents == 1)
        format = GL_RED;
    else if (nrComponents == 3)
        format = GL_RGB;
    else if (nrComponents == 4)
        format = GL_RGBA;

    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, texture);
    glGenerateMipmap(GL_TEXTURE_2D);

    /* I want exact values of the height map */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    stbi_image_free(texture);
    return textureID;
}

