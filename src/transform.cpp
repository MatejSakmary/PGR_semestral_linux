//
// Created by matejs on 08/05/2021.
//

#include "transform.h"

glm::mat4 Transform::getTransformMat(bool doScale) {
    rotation.Normalize();
    glm::vec3 rotationAngles =  rotation.getEulerAngles();
    glm::mat4 transformMat = glm::mat4(1.0f);
    transformMat = glm::translate(transformMat, position);
//    transformMat = glm::rotate(transformMat, (rotationAngles.x), glm::vec3(1.0, 0.0, 0.0));
//    transformMat = glm::rotate(transformMat, (rotationAngles.y), glm::vec3(0.0, 1.0, 0.0));
//    transformMat = glm::rotate(transformMat, (rotationAngles.z), glm::vec3(0.0, 0.0, 1.0));
    transformMat = transformMat * rotation.getRotMatrix();
    if(doScale){
        transformMat = glm::scale(transformMat, scale);
    }

    return transformMat;
}
