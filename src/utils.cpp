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

