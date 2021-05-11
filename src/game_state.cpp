//
// Created by matejs on 08/05/2021.
//
#include "game_state.h"
GameState::GameState(std::string xmlPath)
{
    lightsUsed = 1;
    mouseParameters = MouseParameters({0.0, 0.0f, -1.0f,
                                     false, true});
    fogParams = FogParams({0.165, 0.08});

    camera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f),
                        glm::vec3(0.0f, 0.0f, -1.0f),
                        glm::vec3(0.0f, 1.0f, 0.0f));

    rapidxml::xml_document<> doc;

    /* read xml doc for parsing */
    std::ifstream file(xmlPath);
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    std::string content(buffer.str());

    /* Parising xml */
    doc.parse<0>(&content[0]);

    unsigned int shaderCnt = loadShaders(&doc);
    std::cout << "GAMESTATE::CONSTRUCTOR::Loaded " << shaderCnt << " shaders" << std::endl;

    unsigned int modelsCnt = loadModels(&doc);
    std::cout << "GAMESTATE::CONSTRUCTOR::Loaded " << modelsCnt << " models" << std::endl;

    unsigned int objectsCnt = loadObjectInstances(&doc);
    std::cout << "GAMESTATE::CONSTRUCTOR::Loaded " << objectsCnt << " objects" << std::endl;
}

unsigned int GameState::loadShaders(rapidxml::xml_document<> *gameScene)
{
    unsigned int foundShadresCount = 0;
    rapidxml::xml_node<> *rootNode = gameScene->first_node("Root");
    rapidxml::xml_node<> *shadersNode = rootNode->first_node("Shaders");

    for (rapidxml::xml_node<> *shaderNode = shadersNode->first_node("Shader"); shaderNode;
         shaderNode = shaderNode->next_sibling())
    {
        std::string name = shaderNode->first_attribute("name")->value();
        auto shader = new Shader(shaderNode->first_attribute("vert_path")->value(),
                                  shaderNode->first_attribute("frag_path")->value());

        if(shaders.find(name) != shaders.end()){
            std::cerr << "GAMESTATE::LOADSHADERS::ERROR::Found two shaders with duplicate names not inserting" << std::endl;
            continue;
        } else {
            foundShadresCount++;
            std::cout << "GAMESTATE::LOADSHADERS::Inserting shader with name " << name << std::endl;
            shaders[name] = shader;
        }
    }
    return foundShadresCount;
}
unsigned int GameState::loadModels(rapidxml::xml_document<> *gameScene) {
    unsigned int foundModelsCount = 0;
    rapidxml::xml_node<> *rootNode = gameScene->first_node("Root");
    rapidxml::xml_node<> *modelsNode = rootNode->first_node("Models");

    for (rapidxml::xml_node<> *modelNode = modelsNode->first_node("Model"); modelNode;
         modelNode = modelNode->next_sibling())
    {
        std::string name = modelNode->first_attribute("name")->value();
        auto model = new Model(modelNode->first_attribute("path")->value());

        if(models.find(name) != models.end()){
            std::cerr << "GAMESTATE::LOADMODELS::ERROR::Found two or models with duplicate names not inserting" << std::endl;
            continue;
        } else {
            foundModelsCount++;
            std::cout << "GAMESTATE::LOADMODELS::Inserting model with name " << name << std::endl;
            models[name] = model;
        }
    }
    return foundModelsCount;
}

unsigned int GameState::loadObjectInstances(rapidxml::xml_document<>* gameScene){
    unsigned int foundObjectsCount = 0;
    rapidxml::xml_node<> *rootNode = gameScene->first_node("Root");
    rapidxml::xml_node<> *sceneObjectsNode = rootNode->first_node("SceneObjects");

    for (rapidxml::xml_node<> *sceneObjectNode = sceneObjectsNode->first_node("SceneObject"); sceneObjectNode;
         sceneObjectNode = sceneObjectNode->next_sibling())
    {
        std::string modelName = sceneObjectNode->first_attribute("modelName")->value();
        std::string shaderName = sceneObjectNode->first_attribute("shaderName")->value();
        rapidxml::xml_node<> *rotationNode = sceneObjectNode->first_node("Rotation");
        rapidxml::xml_node<> *positionNode = sceneObjectNode->first_node("Position");
        rapidxml::xml_node<> *scaleNode = sceneObjectNode->first_node("Scale");

        glm::vec3 position = glm::vec3(std::stof(positionNode->first_attribute("x")->value()),
                                       std::stof(positionNode->first_attribute("y")->value()),
                                       std::stof(positionNode->first_attribute("z")->value()));
        glm::vec3 rotation = glm::vec3(std::stof(rotationNode->first_attribute("x")->value()),
                                       std::stof(rotationNode->first_attribute("y")->value()),
                                       std::stof(rotationNode->first_attribute("z")->value()));
        glm::vec3 scale    = glm::vec3(std::stof(scaleNode->first_attribute("x")->value()),
                                       std::stof(scaleNode->first_attribute("y")->value()),
                                       std::stof(scaleNode->first_attribute("z")->value()));
        std::cout << "GAMESTATE:::LOADOBJECTINSTANCES::Found scene object with bound model: " << modelName <<
                     " shader: " << shaderName << " and transformations: " << glm::to_string(position) << " "
                     << glm::to_string(rotation) << " " << glm::to_string(scale) << std::endl;
        auto model = models.find(modelName);
        auto shader = shaders.find(shaderName);
        if(model == models.end()){
            std::cerr << "GAMESTATE::LOADOBJECTINSTANCES::Did not find model that should be bound: " << modelName << std::endl;
            continue;
        }
        if(shader == shaders.end()){
            std::cerr << "GAMESTATE::LOADOBJECTINSTANCES::Did not find shader that should be bound: " << shaderName << std::endl;
            continue;
        }

        Transform transform(position, rotation, scale);
        auto* object = new Object{model->second, shader->second, transform};
        objects.push_back(*object);
        foundObjectsCount++;
    }
    return foundObjectsCount;
}
