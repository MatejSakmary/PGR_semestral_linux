//
// Created by matejs on 08/05/2021.
//
#pragma once
#include "glm/glm.hpp"
#include "shader.h"
#include "utils.h"

#include <string>

typedef enum LType{
    DIRECTIONAL_LIGHT = 0,
    POINT_LIGHT = 1,
    SPOT_LIGHT = 2
}LightType;

class Light {
public:
    LightType type;

    glm::vec3 ambient{};
    glm::vec3 diffuse{};
    glm::vec3 specular{};
    virtual void setLightParam(int index, Shader& shader) = 0;
    virtual ~Light()= default;
};

class DirectionalLight : public Light{
public:
    //Note -> Direction is specified from the light source to each fragment
    glm::vec3 direction;
    DirectionalLight(glm::vec3 ambient,glm::vec3 diffuse,
                     glm::vec3 specular, glm::vec3 direction) : direction{direction}
    {
        this->type = DIRECTIONAL_LIGHT;
        this->ambient = ambient;
        this->diffuse = diffuse;
        this->specular = specular;
    }
    void setLightParam(int index, Shader& shader) override;
};

class PointLight : public Light{
public:
    glm::vec3 position;

    float constant;
    float linear;
    float quadratic;

    PointLight(glm::vec3 ambient,glm::vec3 diffuse,
               glm::vec3 specular, glm::vec3 position, float constant,
               float linear, float quadratic) :
               position{position}, constant{constant}, linear{linear},
               quadratic{quadratic}
    {
        this->type = POINT_LIGHT;
        this->ambient = ambient;
        this->diffuse = diffuse;
        this->specular = specular;
    }
    void setLightParam(int index, Shader& shader) override;
};

class SpotLight : public Light{
public:
    glm::vec3 direction;
    glm::vec3 position;

    float constant;
    float linear;
    float quadratic;

    float cutOff;
    float outerCutOff;

    SpotLight(glm::vec3 ambient,glm::vec3 diffuse,
    glm::vec3 specular, glm::vec3 position, glm::vec3 direction,float constant,
    float linear, float quadratic, float cutOff, float outerCutOff) :
    position{position},direction{direction}, constant{constant}, linear{linear},
    quadratic{quadratic}, cutOff{cutOff}, outerCutOff{outerCutOff}
    {
        this->type = SPOT_LIGHT;
        this->ambient = ambient;
        this->diffuse = diffuse;
        this->specular = specular;
    }
    void setLightParam(int index, Shader& shader) override;
};
