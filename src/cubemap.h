//
// Created by matejs on 13/05/2021.
//
#pragma once
#include "utils.h"
#include "shader.h"
#include "stb_image.h"

#include <vector>
#include <string>
#include <glad/glad.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#pragma region vertices
static float skyboxVertices[] = {
        // positions
        -1.0f, 1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,

        -1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,

        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,

        -1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, 1.0f
};
#pragma endregion

class Cubemap {
public:
    /**
     * Create new Cube map Object
     * @param nightSrc ordered images of night skybox
     * @param daySrc ordered images of day skybox
     * @param scale define the scale of skybox -> this is used for fog generation later in the shader
     */
    Cubemap(std::vector<std::string> nightSrc, std::vector<std::string> daySrc, float scale);
    /**
     * Render the cube map
     * @param shader Shader which cube map uses for rendering
     * @param time This specifies the rotation and interpolation between day and night textures
     */
    void Draw(Shader& shader, float time);
private:
    unsigned int nightTexID;
    unsigned int dayTexID;
    unsigned int VAO;
    unsigned int VBO;
    float scale;
};
