//
// Created by matejs on 08/05/2021.
//

#pragma once
#include <vector>

#include "camera.h"
#include "model.h"

class GameState{
    public:
        Camera *camera;
        float lastX;
        float lastY;
        float deltaTime;
        std::vector<Model> models;
//        std::vector<Light> lights;

    GameState();
};

