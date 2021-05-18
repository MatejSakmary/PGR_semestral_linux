//
// Created by matejs on 13/05/2021.
//

#include "cubemap.h"

Cubemap::Cubemap(std::vector<std::string> nightSrc, std::vector<std::string> daySrc, float scale) {

    this->scale = scale;
    /* Prepare night textures----------------------------------------------------------*/
    glGenTextures(1, &nightTexID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, nightTexID);

    int width, height, nrChannels;
    unsigned char *data;
    for (unsigned int i = 0; i < nightSrc.size(); i++) {
        data = stbi_load(nightSrc[i].c_str(), &width, &height, &nrChannels, 0);
        if (!data) {
            std::cerr << "CUBEMAP::Failed to load texture at path " << nightSrc[i] << std::endl;
            stbi_image_free(data);
            continue;
        }
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height,
                     0, GL_RGB, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        /* Apparently this is needed for "legacy reasons", where there could happen that the edges don't
         * align properly?*/
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    /* Prepare day textures----------------------------------------------------------*/
    glGenTextures(1, &dayTexID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, dayTexID);
    for (unsigned int i = 0; i < daySrc.size(); i++) {
        data = stbi_load(daySrc[i].c_str(), &width, &height, &nrChannels, 0);
        if (!data) {
            std::cerr << "CUBEMAP::Failed to load texture at path " << daySrc[i] << std::endl;
            stbi_image_free(data);
            continue;
        }
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height,
                     0, GL_RGB, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        /* Apparently this is needed for "legacy reasons", where there could happen that the edges don't
         * align properly?*/
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
    CHECK_GL_ERROR();

    /* Prepare skybox geometric object */
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    /* This is essentially unit cube -> only position is required */
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);
    CHECK_GL_ERROR();
}

void Cubemap::Draw(Shader &shader, float time) {
    shader.use();
    /* bind and set day cube map texture and uniform */
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, dayTexID);
    shader.setInt("dayCubemap", 0);

    /* bind and set night cube map texture and uniform */
    CHECK_GL_ERROR();
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, nightTexID);
    shader.setInt("nightCubemap", 1);

    /* rotating animation */
    glm::mat4 rotationMat = glm::rotate(glm::mat4(1.0f), glm::radians(time), glm::vec3(0.0, 1.0, 0.0));
    /* sets the scale used for fog computation */
    glm::mat4 modelMatrix = glm::scale(glm::mat4(1.0), glm::vec3(scale, scale, scale));

    shader.setMat4fv("rotation", rotationMat);
    shader.setMat4fv("model", modelMatrix);
    shader.setFloat("mixVal",(glm::sin(time/5)+1)/2);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}
