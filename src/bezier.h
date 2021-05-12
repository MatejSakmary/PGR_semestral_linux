//
// Created by matejs on 12/05/2021.
//

#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

class Bezier {
public:
    Bezier(glm::vec3 from, glm::vec3 to, glm::vec3 firstControlPoint) :
                from{from}, to{to}, firstControlPoint{firstControlPoint}, cubic{false}{};
    Bezier(glm::vec3 from, glm::vec3 to, glm::vec3 firstControlPoint, glm::vec3 secondControlPoint) :
            from{from}, to{to}, firstControlPoint{firstControlPoint},secondControlPoint{secondControlPoint},
            cubic{true}{};

    glm::vec3 getPosition(float t);

private:
    bool cubic;
    glm::vec3 from;
    glm::vec3 to;
    glm::vec3 firstControlPoint;
    glm::vec3 secondControlPoint;
};
