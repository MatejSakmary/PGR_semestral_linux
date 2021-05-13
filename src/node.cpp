//
// Created by matejs on 13/05/2021.
//

#include "node.h"

glm::mat4 Node::getTransform() {
    std::vector<Transform*> transformList;
    transformList.push_back(this->transform);
    glm::mat4 finalTransform = glm::mat4(1.0f);

    Node* movingParent = this->parent;
    while(movingParent != nullptr)
    {
        transformList.push_back(movingParent->transform);
        movingParent = movingParent->parent;
    }

    for(unsigned int i = transformList.size() - 1; i >= 0; i--){
        finalTransform *= transformList[i]->getTransformMat();
    }
    return finalTransform;
}

void Node::addChildren(std::vector<Node *> newChildren) {
    for(auto node : newChildren)
    {
        children.push_back(node);
    }
}
