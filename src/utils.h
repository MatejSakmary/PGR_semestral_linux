//
// Created by matejs on 05/05/2021.
//
#ifndef UTILS
#define UTILS
#include <iostream>
#include <string>

#include "glad/glad.h"
#include "stb_image.h"

void checkGLError(const char *where, int line);
#ifndef CHECK_GL_ERROR
    #define CHECK_GL_ERROR() do { checkGLError(__FUNCTION__, __LINE__); } while(0)
#endif
unsigned int loadTexture(const char* path);
#endif

