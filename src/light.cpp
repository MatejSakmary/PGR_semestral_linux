//
// Created by matejs on 08/05/2021.
//
#include "light.h"

void DirectionalLight::setLightParam(int index, Shader& shader) {
    // create prefix lights[i].
    std::string prefix = "lights[" + std::to_string(index) + "].";
    shader.setInt(prefix+"type", this->type);
    shader.setVec3(prefix + "direction", this->direction);
    shader.setVec3(prefix + "ambient", this->ambient);
    shader.setVec3(prefix + "diffuse", this->diffuse);
    shader.setVec3(prefix + "specular", this->specular);
    CHECK_GL_ERROR();
}

void PointLight::setLightParam(int index, Shader &shader) {
    // create prefix lights[i].
    std::string prefix = "lights[" + std::to_string(index) + "].";
    shader.setInt(prefix+"type", this->type);
    shader.setVec3(prefix + "position", this->position);
    shader.setVec3(prefix + "ambient", this->ambient);
    shader.setVec3(prefix + "diffuse", this->diffuse);
    shader.setVec3(prefix + "specular", this->specular);

    shader.setFloat(prefix + "constant", this->constant);
    shader.setFloat(prefix + "linear", this->linear);
    shader.setFloat(prefix + "quadratic", this->quadratic);
    CHECK_GL_ERROR();
}

void SpotLight::setLightParam(int index, Shader &shader) {
    // create prefix lights[i].
    std::string prefix = "lights[" + std::to_string(index) + "].";
    shader.setInt(prefix+"type", this->type);

    shader.setVec3(prefix + "direction", this->direction);
    shader.setVec3(prefix + "position", this->position);

    shader.setVec3(prefix + "ambient", this->ambient);
    shader.setVec3(prefix + "diffuse", this->diffuse);
    shader.setVec3(prefix + "specular", this->specular);

    shader.setFloat(prefix + "constant", this->constant);
    shader.setFloat(prefix + "linear", this->linear);
    shader.setFloat(prefix + "quadratic", this->quadratic);

    shader.setFloat(prefix + "cutOff", this->cutOff);
    shader.setFloat(prefix + "outerCutOff", this->outerCutOff);
    CHECK_GL_ERROR();
}
