#pragma once
#include <vector>
#include <unordered_map>
#include <iostream>
#include <utility>

#include "node.h"
#include "camera.h"
#include "model.h"
#include "light.h"
#include "transform.h"
#include "scene_object.h"

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

typedef struct{
    unsigned int VAO;
    unsigned int VBO;
    unsigned int fireTexture;
}Fire;

typedef struct{
    float lastX;
    float lastY;
    float pressDelay;
    bool mouseControl;
    bool firstMouseInput;
}MouseParameters;

typedef struct{
   float scale;
   unsigned int heightTexID;
    unsigned int normalTexID;
    unsigned int diffuseTexID;
   unsigned int resolution;
   std::string heightTexPath;
   std::string normalTexPath;
   std::string diffuseTexPath;
}TerrainParams;

class GameState{
public:
    int lightsUsed;
    float deltaTime;

    Camera *camera;
    MouseParameters mouseParameters;
    FogParams fogParams;
    ReloadParams reloadParams;
    TerrainParams terrainParams;

    Node* ufoNode;
    std::vector<Light*> lights;
    std::vector<SceneObject*> objects;
    std::unordered_map<std::string,Shader*> shaders;
    std::unordered_map<std::string,Model*> models;

    Node* rootNode;
    rapidxml::xml_document<>* gameScene;
    explicit GameState(std::string xmlPath);
    /**
     * Processes Reload params and executes proper action required
     * Note -> at least one parameter in reload params must be set to true for this to do anything
     */
    void reloadHandle();
    /**
     * Draw two 90 deg rotated animated fire sprites
     * @param transform position where fire sprites should be rendered
     * @param projectionMatrix current projection matrix
     * @param cameraMatrix current view matrix
     * @param time this parameter controls the animation frame
     */
    void drawFire(glm::mat4 transform,glm::mat4* projectionMatrix, glm::mat4* cameraMatrix,  float time);

private:
    // this is needed by rapidxml -> as long as rapidxml document exists so must the original string
    std::string xmlPath;
    std::string xmlContent;

    Node* portalNode;
    Fire fire;
    unsigned int loadShaders();
    unsigned int loadModels();
    unsigned int loadObjectInstances();
    unsigned int loadLights();
    unsigned int loadSceneGraph();
    void loadHeightMapParams();
    std::vector<Node*> processChildren(Node* parentNode, rapidxml::xml_node<> *childrenNode);
    SceneObject* prepareTerrainModel();
    void reloadShadersAndObjects();
    void reloadModelsAndObjects();
    void reloadObjects();
    void reloadLights();
    void prepareFireModel();
    Node* processAnimationNode(rapidxml::xml_node<>* animationNode, Node* parent, std::string name);
    Node* processAnimationCurveNode(rapidxml::xml_node<>* animationNode, Node* parent, std::string name);
};
