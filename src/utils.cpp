//
// Created by matejs on 05/05/2021.
//
#include "utils.h"

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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    stbi_image_free(texture);
    return textureID;
}

