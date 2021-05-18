//
// Created by matejs on 12/05/2021.
//

#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

class Bezier {
public:
    /**
     * Create quadratic Bezier function object
     * @param from start point
     * @param to end point
     * @param firstControlPoint
     */
    Bezier(glm::vec3 from, glm::vec3 to, glm::vec3 firstControlPoint) :
                from{from}, to{to}, firstControlPoint{firstControlPoint}, cubic{false}{};
    /**
     * Create cubic Bezier function object
     * @param from start point
     * @param to end point
     * @param firstControlPoint
     * @param secondControlPoint
     */
    Bezier(glm::vec3 from, glm::vec3 to, glm::vec3 firstControlPoint, glm::vec3 secondControlPoint) :
            from{from}, to{to}, firstControlPoint{firstControlPoint},secondControlPoint{secondControlPoint},
            cubic{true}{};
    /**
    *
    * @param t interpolation parameter (weight)
    * @return position on the curve corresponding to the interpolation parameter specified
    */
    glm::vec3 getPosition(float t);

private:
    bool cubic;
    glm::vec3 from;
    glm::vec3 to;
    glm::vec3 firstControlPoint;
    glm::vec3 secondControlPoint;
};
