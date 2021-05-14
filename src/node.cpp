//
// Created by matejs on 13/05/2021.
//

#include "node.h"

glm::mat4 Node::getTransform(float t) {
    std::vector<glm::mat4> transformList;
    glm::mat4 finalTransform = glm::mat4(1.0f);

    Node* movingParent = this->parent;
    while(movingParent != nullptr)
    {
        if(movingParent->type == LINEAR_ANIMATION){
            transformList.push_back(((AnimationLinearNode*)movingParent)->getTransformMat(t));
        }else{
            transformList.push_back(movingParent->transform->getTransformMat(false));
        }
        movingParent = movingParent->parent;
    }

    for(int i = transformList.size(); i > 0; i--){
        finalTransform *= transformList[i-1];
    }
    if(this->type == LINEAR_ANIMATION){
        finalTransform *= ((AnimationLinearNode*)this)->getTransformMat(t);
    }else{
        finalTransform*= this->transform->getTransformMat();
    }
    return finalTransform;
}

void Node::addChildren(std::vector<Node *> newChildren) {
    for(auto node : newChildren)
    {
        children.push_back(node);
    }
}

glm::mat4 AnimationLinearNode::getTransformMat(float t) {
    /* interpolated rotation matrix */
    Quaternion result;
    Quaternion::Slerp(result, transform->rotation, end->rotation,t);
    glm::mat4 rotation = result.getRotMatrix();

    /* interpolated translation matrix */
    glm::vec3 finalPos = ((1-t) * transform->position) + (t * end->position);
    glm::mat4 translation = glm::translate(glm::mat4(1.0f), finalPos);

    /* interpolated scale matrix */
    glm::vec3 finalScale = ((1-t) * transform->scale) + (t * end->scale);
    glm::mat4 scale = glm::scale(glm::mat4(1.0f), finalScale);

    return translation * rotation * scale;
}
