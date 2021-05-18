// // Created by matejs on 12/05/2021.
//

#include "bezier.h"

glm::vec3 Bezier::getPosition(float t) {
    glm::vec3 position;
    if(!cubic){
        position = (float)(glm::pow(1 - t, 2)) * from +
                   (float)((1 - t) * 2 * t) * firstControlPoint +
                   (float)(t * t) * to;
    } else{
        position = (float)(glm::pow(1 - t, 3)) * from +
                   (float)(glm::pow(1 - t, 2) * 3 * t) * firstControlPoint +
                   (float)((1 - t) * 3 * t * t) * secondControlPoint +
                   (float)(t * t * t) * to;
    }
    return position;
}
