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
//#include "rapidxml/rapidxml_print.hpp"
//#include "rapidxml/rapidxml_ext.h"

#include "glm/glm.hpp"
#include "glm/ext.hpp"

typedef struct{
    float density;
    float treshold;
}FogParams;

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

class GameState{
public:
    int lightsUsed;
    float deltaTime;
    Camera *camera;
    MouseParameters mouseParameters;
    FogParams fogParams;
    std::vector<Light*> lights;
    std::vector<Object> objects;
    std::unordered_map<std::string,Shader*> shaders;
    std::unordered_map<std::string,Model*> models;

    rapidxml::xml_document<>* gameScene;
    GameState(std::string xmlPath);
    void writeToXML();

private:
    unsigned int loadShaders();
    unsigned int loadModels();
    unsigned int loadObjectInstances();

};
