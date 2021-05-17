//
// Created by matejs on 13/05/2021.
//

#pragma once
#include <iostream>
#include <utility>
#include <vector>
#include <string>

#include "transform.h"
#include "light.h"
#include "scene_object.h"

#include "glm/glm.hpp"
#include "bezier.h"

typedef enum NType{
    LIGHT = 0,
    PURE_TRANSFORM = 1,
    OBJECT = 2,
    LINEAR_ANIMATION = 3,
    CURVE_ANIMATION = 4,
}NodeType;

class Node{
public:
    NodeType type;
    Transform* transform;
    Node* parent;
    std::vector<Node*> children;
    std::string name;

    Node(Transform* transform, Node* parent, std::vector<Node*> children, std::string name ,NodeType type = PURE_TRANSFORM) :
         type{type}, transform{transform}, parent{parent}, children{std::move(children)}, name{std::move(name)}{};
    glm::mat4 getTransform(float t);
    void addChildren(std::vector<Node*> children);
};

class LightNode : public Node{
public:
    LightNode(Transform* transform, Node* parent, std::vector<Node*> children, Light* light, std::string name) :
              Node(transform, parent, std::move(children),std::move(name), LIGHT), light{light}{};
    Light* light;
};

class ObjectNode : public Node{
public:
    ObjectNode(Transform* transform, Node* parent, std::vector<Node*> children, SceneObject* object, std::string name) :
               Node(transform, parent, std::move(children),std::move(name), OBJECT), object{object}{};
    SceneObject* object;
};
class AnimationLinearNode : public Node{
public:
    AnimationLinearNode(Transform* start, Transform* end, Node* parent, std::vector<Node*> children, std::string name):
                        Node(start, parent, std::move(children), std::move(name), LINEAR_ANIMATION), end{end}{};
    Transform* end;
    glm::mat4 getTransformMat(float t);
};

class AnimationCurveNode : public Node{
public:
    AnimationCurveNode(Transform* start, Transform* end, Bezier* curve, Node* parent, std::vector<Node*> children, std::string name) :
                        Node(start, parent, std::move(children), std::move(name), CURVE_ANIMATION), end{end}, curve{curve}{};
    Bezier* curve;
    Transform* end;
    glm::mat4 getTransformMat(float t);
};
