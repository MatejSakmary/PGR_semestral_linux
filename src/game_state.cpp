//
// Created by matejs on 08/05/2021.
//
#include "game_state.h"
GameState::GameState(std::string xmlPath)
{
    reload_shaders = false;
    this->xmlPath = xmlPath;
    lightsUsed = 1;
    mouseParameters = MouseParameters({0.0, 0.0f, -1.0f,
                                     false, true});
    fogParams = FogParams({0.060, 0.08});

    Bezier* bezier = new Bezier(glm::vec3(-50.0f, 20.0f, 10.0f),
                           glm::vec3(-50.0f, 20.0f, -10.0f),
                           glm::vec3(200.0f, 20.0f, 170.0f),
                           glm::vec3(20.0f, 20.0f, -320.0f));

    camera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f),
                        glm::vec3(0.0f, 0.0f, -1.0f),
                        glm::vec3(0.0f, 1.0f, 0.0f),
                        bezier);

    gameScene = new rapidxml::xml_document<>();

    /* read xml doc for parsing */
    std::ifstream file(xmlPath);
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    xmlContent = buffer.str();

    /* Parising xml */
    gameScene->parse<0>(&xmlContent[0]);

    unsigned int shaderCnt = loadShaders();
    unsigned int modelsCnt = loadModels();
    unsigned int objectsCnt = loadObjectInstances();
    unsigned int lighthsCnt = loadLights();

    std::cout << "GAMESTATE::CONSTRUCTOR::Loaded " << shaderCnt << " shaders" << std::endl;
    std::cout << "GAMESTATE::CONSTRUCTOR::Loaded " << modelsCnt << " models" << std::endl;
    std::cout << "GAMESTATE::CONSTRUCTOR::Loaded " << objectsCnt << " objects" << std::endl;
    std::cout << "GAMESTATE::CONSTRUCTOR::Loaded " << lighthsCnt << " lights" << std::endl;
}

unsigned int GameState::loadShaders()
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

unsigned int GameState::loadModels() {
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

unsigned int GameState::loadObjectInstances(){
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
        objects.push_back(object);
        foundObjectsCount++;
    }
    return foundObjectsCount;
}

unsigned int GameState::loadLights() {
    unsigned int foundObjectsCount = 0;
    rapidxml::xml_node<> *rootNode = gameScene->first_node("Root");
    rapidxml::xml_node<> *lightsArrNode = rootNode->first_node("Lights");

    for (rapidxml::xml_node<> *lightNode = lightsArrNode->first_node("Light"); lightNode;
         lightNode = lightNode->next_sibling()) {

        if (foundObjectsCount == 8){
            std::cerr << "GAMESCENE::LOAD_LIGHTS::ERROR::Already loaded maximum number of supported lights not loading more"
                      << std::endl;
        }

        std::string lightType = lightNode->first_attribute("type")->value();
        LightType type;
        if (lightType == "Point"){type = POINT_LIGHT;}
        else if (lightType == "Directional"){type = DIRECTIONAL_LIGHT;}
        else if (lightType == "Spotlight"){type = SPOT_LIGHT;}
        else{
            std::cerr << "GAMESCENE::LOAD_LIGTHTS::ERROR::Unable to parse light type" << std::endl;
            continue;
        }
        rapidxml::xml_node<> *ambientNode = lightNode->first_node("Ambient");
        rapidxml::xml_node<> *diffuseNode = lightNode->first_node("Diffuse");
        rapidxml::xml_node<> *specularNode = lightNode->first_node("Specular");
        glm::vec3 ambient = glm::vec3(std::stof(ambientNode->first_attribute("r")->value()),
                                      std::stof(ambientNode->first_attribute("g")->value()),
                                      std::stof(ambientNode->first_attribute("b")->value()));
        glm::vec3 diffuse = glm::vec3(std::stof(diffuseNode->first_attribute("r")->value()),
                                      std::stof(diffuseNode->first_attribute("g")->value()),
                                      std::stof(diffuseNode->first_attribute("b")->value()));
        glm::vec3 specular = glm::vec3(std::stof(specularNode->first_attribute("r")->value()),
                                      std::stof(specularNode->first_attribute("g")->value()),
                                      std::stof(specularNode->first_attribute("b")->value()));
        if(type == POINT_LIGHT){
            rapidxml::xml_node<> *positionNode = lightNode->first_node("Position");
            rapidxml::xml_node<> *paramNode = lightNode->first_node("Params");
            glm::vec3 position = glm::vec3(std::stof(positionNode->first_attribute("x")->value()),
                                           std::stof(positionNode->first_attribute("y")->value()),
                                           std::stof(positionNode->first_attribute("z")->value()));
            float constant = std::stof(paramNode->first_attribute("constant")->value());
            float linear = std::stof(paramNode->first_attribute("linear")->value());
            float quadratic = std::stof(paramNode->first_attribute("quadratic")->value());
            lights.push_back(new PointLight(ambient, diffuse, specular, position, constant, linear, quadratic));
        }else if (type == DIRECTIONAL_LIGHT){
            rapidxml::xml_node<> *directionNode = lightNode->first_node("Direction");
            glm::vec3 direction = glm::vec3(std::stof(directionNode->first_attribute("x")->value()),
                                            std::stof(directionNode->first_attribute("y")->value()),
                                            std::stof(directionNode->first_attribute("z")->value()));
            lights.push_back(new DirectionalLight(ambient, diffuse, specular, direction));
        }
        foundObjectsCount++;
    }
    return foundObjectsCount;
}

void GameState::writeToXML() {
    rapidxml::xml_node<> *rootNode = gameScene->first_node("Root");
    rapidxml::xml_node<> *sceneObjectsNode = rootNode->first_node("SceneObjects");

    unsigned int objectidx = 0;
    for (rapidxml::xml_node<> *sceneObjectNode = sceneObjectsNode->first_node("SceneObject"); sceneObjectNode;
         sceneObjectNode = sceneObjectNode->next_sibling())
    {
        Object currObject = *objects[objectidx++];
        rapidxml::xml_node<> *rotationNode = sceneObjectNode->first_node("Rotation");
        rapidxml::xml_node<> *positionNode = sceneObjectNode->first_node("Position");
        rapidxml::xml_node<> *scaleNode = sceneObjectNode->first_node("Scale");
        char* x = gameScene->allocate_string(std::to_string(currObject.transform.rotation.x).c_str());
        char* y = gameScene->allocate_string(std::to_string(currObject.transform.rotation.y).c_str());
        char* z = gameScene->allocate_string(std::to_string(currObject.transform.rotation.z).c_str());
        rotationNode->first_attribute("x")->value(x);
        rotationNode->first_attribute("y")->value(y);
        rotationNode->first_attribute("z")->value(z);
        x = gameScene->allocate_string(std::to_string(currObject.transform.position.x).c_str());
        y = gameScene->allocate_string(std::to_string(currObject.transform.position.y).c_str());
        z = gameScene->allocate_string(std::to_string(currObject.transform.position.z).c_str());
        positionNode->first_attribute("x")->value(x);
        positionNode->first_attribute("y")->value(y);
        positionNode->first_attribute("z")->value(z);
        x = gameScene->allocate_string(std::to_string(currObject.transform.scale.x).c_str());
        y = gameScene->allocate_string(std::to_string(currObject.transform.scale.y).c_str());
        z = gameScene->allocate_string(std::to_string(currObject.transform.scale.z).c_str());
        scaleNode->first_attribute("x")->value(x);
        scaleNode->first_attribute("y")->value(y);
        scaleNode->first_attribute("z")->value(z);
    }

    std::string(xml_as_string);
    std::ofstream fileStore("export.xml");

    rapidxml::print(std::back_inserter(xml_as_string), *gameScene);
    fileStore << xml_as_string;
    fileStore.close();

//    DEPRECATED WAY OF EXPORTING XML, I NOW JUST MODIFY ALREADY EXISTING ONE
//    rapidxml::xml_document<> exportDoc;
//    rapidxml::xml_node<>* decl = exportDoc.allocate_node(rapidxml::node_declaration);
//    decl->append_attribute(exportDoc.allocate_attribute("version","1.0"));
//    decl->append_attribute(exportDoc.allocate_attribute("encoding","utf-8"));
//    exportDoc.append_node(decl);
//
//    rapidxml::xml_node<>* root = exportDoc.allocate_node(rapidxml::node_element, "Root");
//    rapidxml::xml_node<>* modelsNode = exportDoc.allocate_node(rapidxml::node_element, "Models");
//    for(auto& model : models){
//        rapidxml::xml_node<>* modelNode = exportDoc.allocate_node(rapidxml::node_element, "Model");
//        char* nodeName = exportDoc.allocate_string(model.first.c_str());
//        char* nodePath = exportDoc.allocate_string(model.second->path.c_str());
//        modelNode->append_attribute(exportDoc.allocate_attribute("name", nodeName));
//        modelNode->append_attribute(exportDoc.allocate_attribute("path", nodePath));
//        modelsNode->append_node(modelNode);
//    }
//    root->append_node(modelsNode);
//    exportDoc.append_node( root );
//    gameScene.clear();
}

void GameState::reloadShadersAndObjects() {
    delete gameScene;
    gameScene = new rapidxml::xml_document<>();

    /* read xml doc for parsing */
    std::ifstream file(xmlPath);
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    xmlContent = buffer.str();

    /* Parsing xml */
    gameScene->parse<0>(&xmlContent[0]);

    for (auto& shader : shaders){
        delete shader.second;
    }
    shaders.clear();
    loadShaders();
    reloadObjects();
    reload_shaders = false;
}
void GameState::reloadObjects() {
    for(unsigned int i = 0; i < objects.size(); i++){
        delete objects[i];
    }
    std::cout << "successfully deleted objects" << std::endl;
    objects.clear();
    loadObjectInstances();
}

