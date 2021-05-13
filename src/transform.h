//
// Created by matejs on 08/05/2021.
//

#pragma once
#include <iostream>

#include "glm/glm.hpp"
#include "glm/ext.hpp"

class Transform {
public:
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;

    Transform(Transform const &transform) = default;
    Transform(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale) :
              position{position}, rotation{rotation}, scale{scale}{};
    glm::mat4 getTransformMat();

};


