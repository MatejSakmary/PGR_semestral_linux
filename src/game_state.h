#pragma once
#include <vector>
#include <tuple>
#include <unordered_map>
#include <iostream>

#include "camera.h"
#include "model.h"
#include "light.h"
#include "glm/glm.hpp"
#include "rapidxml/rapidxml.hpp"
//#include "rapidxml/rapidxml_print.hpp"
//#include "rapidxml/rapidxml_utils.hpp"
//#include "rapidxml/rapidxml_iterators.hpp"

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
    std::vector<Model> models;
    std::vector<Light> lights;
    std::unordered_map<std::string,Shader*> shaders;
    GameState(std::string xmlPath);

private:
    unsigned int loadShaders(rapidxml::xml_document<>* gameScene);

};
