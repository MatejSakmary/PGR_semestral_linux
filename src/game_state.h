#pragma once
#include <vector>
#include <unordered_map>
#include <iostream>
#include <utility>

#include "camera.h"
#include "model.h"
#include "light.h"
#include "transform.h"

#include "rapidxml/rapidxml_ext.h"

#include "glm/glm.hpp"
#include "glm/ext.hpp"

typedef struct{
    bool reloadShaders;
    bool reloadObjects;
    bool reloadModels;
    bool reloadLights;
}ReloadParams;

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
    ReloadParams reloadParams;
    std::vector<Light*> lights;
    std::vector<Object*> objects;
    std::unordered_map<std::string,Shader*> shaders;
    std::unordered_map<std::string,Model*> models;

    rapidxml::xml_document<>* gameScene;
    explicit GameState(std::string xmlPath);
    void reloadHandle();

    // !!! ONLY WRITES SCENEOBJECT TRANSFORMS NOTHING ELSE
    void writeToXML();

private:
    // this is needed by rapidxml
    std::string xmlPath;
    std::string xmlContent;
    unsigned int loadShaders();
    unsigned int loadModels();
    unsigned int loadObjectInstances();
    unsigned int loadLights();
    void reloadShadersAndObjects();
    void reloadModelsAndObjects();
    void reloadObjects();
    void reloadLights();

};
