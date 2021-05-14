//
// Created by matejs on 08/05/2021.
//

#pragma once
#include <iostream>

#include "quaternion.h"

#include "glm/glm.hpp"
#include "glm/ext.hpp"

class Transform {
public:
    glm::vec3 position;
    glm::vec3 scale;
    Quaternion rotation;

    Transform(Transform const &transform) = default;
    Transform(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale) :
              position{position}, scale{scale}{
        this->rotation = Quaternion(rotation.x, rotation.y, rotation.z);
    };
    glm::mat4 getTransformMat(bool doScale = true);

};


