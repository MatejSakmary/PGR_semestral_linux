#pragma once
#include <vector>
#include <tuple>
#include <unordered_map>
#include <iostream>

#include "camera.h"
#include "model.h"
#include "light.h"
#include "transform.h"

#include "rapidxml/rapidxml.hpp"

#include "glm/glm.hpp"
#include "glm/ext.hpp"

typedef struct {
    Model* model;
    Shader* shader;
    Transform transform;
}Object;

typedef struct{
    float lastX;
    float lastY;
    float pressDelay;
    bool mouseControl;
    bool firstMouseInput;
}MouseParameters;

typedef struct{
    glm::vec4 clearColor;
}RenderParameters;

class GameState{
public:
    float deltaTime;
    Camera *camera;
    MouseParameters mouseParameters;
    RenderParameters renderParameters;
    std::vector<Light> lights;
    std::vector<Object> objects;
    std::unordered_map<std::string,Shader*> shaders;
    std::unordered_map<std::string,Model*> models;
    GameState(std::string xmlPath);

private:
    unsigned int loadShaders(rapidxml::xml_document<>* gameScene);
    unsigned int loadModels(rapidxml::xml_document<>* gameScene);
    unsigned int loadObjectInstances(rapidxml::xml_document<>* gameScene);

};
