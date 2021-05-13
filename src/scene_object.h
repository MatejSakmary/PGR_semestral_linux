//
// Created by matejs on 14/05/2021.
//
#pragma once
#include "model.h"
#include "transform.h"
#include "shader.h"

class SceneObject {
public:
    Model* model;
    Shader* shader;
    Transform* transform;
    SceneObject(Model* model, Shader* shader, Transform* transform)
               : model{model}, shader{shader}, transform{transform}{};
};
