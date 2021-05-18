//
// Created by matejs on 08/05/2021.
//
#include "game_state.h"
GameState::GameState(std::string xmlPath)
{
    rootNode = nullptr;
    terrainParams.heightTexID = 1892;
    reloadParams = ReloadParams({false, false, false, false});
    this->xmlPath = xmlPath;
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

    loadSceneGraph();
    prepareFireModel();

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

std::vector<Node*> GameState::processChildren(Node *parentNode, rapidxml::xml_node<> *childrenNode) {
    std::vector<Node*> finalChildren;
    for (rapidxml::xml_node<> *childNode = childrenNode->first_node("Node"); childNode;
         childNode = childNode->next_sibling())
    {
        Node* childGraphNode;
        /* get node params */
        NodeType type = (NodeType)std::stoi(childNode->first_attribute("type")->value());
        std::string nodeName = childNode->first_attribute("name")->value();
        Transform* transform;

        if(type != LINEAR_ANIMATION && type != CURVE_ANIMATION) {
            /* get transform node NODE->TRANSFORM */
            rapidxml::xml_node<> *transformNode = childNode->first_node("Transform");
            /* get individual transforms from transform not NODE->TRANSFORM->ROTATION
             *                                                             ->POSITION
             *                                                             ->SCALE    */
            rapidxml::xml_node<> *rotationNode = transformNode->first_node("Rotation");
            rapidxml::xml_node<> *positionNode = transformNode->first_node("Position");
            rapidxml::xml_node<> *scaleNode = transformNode->first_node("Scale");

            /* get individual attributes from those nodes */
            glm::vec3 position = glm::vec3(std::stof(positionNode->first_attribute("x")->value()),
                                           std::stof(positionNode->first_attribute("y")->value()),
                                           std::stof(positionNode->first_attribute("z")->value()));
            glm::vec3 rotation = glm::vec3(std::stof(rotationNode->first_attribute("x")->value()),
                                           std::stof(rotationNode->first_attribute("y")->value()),
                                           std::stof(rotationNode->first_attribute("z")->value()));
            glm::vec3 scale = glm::vec3(std::stof(scaleNode->first_attribute("x")->value()),
                                        std::stof(scaleNode->first_attribute("y")->value()),
                                        std::stof(scaleNode->first_attribute("z")->value()));
            /* create new transform object */
            transform = new Transform(position, rotation, scale);
        }
        /*PURE TRANSFORM node processing ---------------------------------------------------------------------------- */
        if(type == PURE_TRANSFORM){
            childGraphNode = new Node(transform, parentNode, std::vector<Node*>{},nodeName);
            std::cout << "GAMESTATE::PROCESS_CHILDREN::Created node " << nodeName << " of type " << type << std::endl;
        }
        /*OBJECT node processing ------------------------------------------------------------------------------------ */
        else if (type == OBJECT){
            /* get scene object node NODE->SCENEOBJECT */
            rapidxml::xml_node<> *sceneObjectNode = childNode->first_node("SceneObject");
            /* get scene object parameters*/
            std::string modelName = sceneObjectNode->first_attribute("model")->value();
            std::string shaderName = sceneObjectNode->first_attribute("shader")->value();
            /* find appropriate models and shaders and for later linking */
            auto model = models.find(modelName);
            auto shader = shaders.find(shaderName);
            if(model == models.end()){
                std::cerr << "GAMESTATE::PROCESS_CHILDREN::Did not find model that should be bound: " << modelName << std::endl;
                continue;
            }
            if(shader == shaders.end()){
                std::cerr << "GAMESTATE::PROCESS_CHILDREN::Did not find shader that should be bound: " << shaderName << std::endl;
                continue;
            }
            auto* object = new SceneObject(model->second, shader->second, transform);
            /* create child node */
            childGraphNode = new ObjectNode(transform, parentNode, std::vector<Node*>{},object, nodeName);
            std::cout << "GAMESTATE::PROCESS_CHILDREN::Created node " << nodeName << " of type " << type << std::endl;
        }
        /*LIGHT node processing ------------------------------------------------------------------------------------- */
        else if (type == LIGHT){
            std::cerr << "GAMESTATE::PROCESS_CHILDREN::Not implemented yet";
            continue;
        }
        /*LINEAR ANIMATION processing --------------------------------------------------------------------------------*/
        else if (type == LINEAR_ANIMATION){
            childGraphNode = processAnimationNode(childNode, parentNode, nodeName);
            std::cout << "GAMESTATE::PROCESS_CHILDREN::Created node " << nodeName << " of type " << type << std::endl;
        }
        else if (type == CURVE_ANIMATION){
            childGraphNode = processAnimationCurveNode(childNode, parentNode, nodeName);
            std::cout << "GAMESTATE::PROCESS_CHILDREN::Created node " << nodeName << " of type " << type << std::endl;
        }
        /*HANDLE ERRORS --------------------------------------------------------------------------------------------- */
        else {
            std::cerr << "GAMESTATE::PROCESS_CHILDREN::Node " << nodeName << " has unknown type " << type << std::endl;
            continue;
        }
        rapidxml::xml_node<> *childrensNode = childNode->first_node("Children");
        /* recursively process children nodes */
        childGraphNode->addChildren(processChildren(childGraphNode,childrensNode));
        if(nodeName == "UFO"){
            ufoNode = childGraphNode;
            std::cout << "found ufo node" << std::endl;
        }
        finalChildren.push_back(childGraphNode);
    }
    return finalChildren;
}

Node *GameState::processAnimationCurveNode(rapidxml::xml_node<> *animationNode, Node *parent, std::string name) {
    Node* returnNode;
    rapidxml::xml_node<>* startNode = animationNode->first_node("Start");
    rapidxml::xml_node<>* endNode = animationNode->first_node("End");
    rapidxml::xml_node<>* controlNode1 = animationNode->first_node("Control1");
    rapidxml::xml_node<>* controlNode2 = animationNode->first_node("Control2");
    glm::vec3 start, end, control1, control2;

    start = glm::vec3(std::stof(startNode->first_attribute("x")->value()),
                     std::stof(startNode->first_attribute("y")->value()),
                     std::stof(startNode->first_attribute("z")->value()));
    end = glm::vec3(std::stof(endNode->first_attribute("x")->value()),
                      std::stof(endNode->first_attribute("y")->value()),
                      std::stof(endNode->first_attribute("z")->value()));
    control1 = glm::vec3(std::stof(controlNode1->first_attribute("x")->value()),
                      std::stof(controlNode1->first_attribute("y")->value()),
                      std::stof(controlNode1->first_attribute("z")->value()));
    control2 = glm::vec3(std::stof(controlNode2->first_attribute("x")->value()),
                      std::stof(controlNode2->first_attribute("y")->value()),
                      std::stof(controlNode2->first_attribute("z")->value()));
    auto* startTransform = new Transform(start, glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 0.0));
    auto* endTransform = new Transform(end, glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 0.0));
    auto* bezier = new Bezier(start, end, control1, control2);
    returnNode = new AnimationCurveNode(startTransform, endTransform, bezier, parent, std::vector<Node*>{}, name);
    return returnNode;
}

Node* GameState::processAnimationNode(rapidxml::xml_node<>* animationNode, Node* parent, std::string name){
    Node* returnNode;
    /* get start and end nodes NODE->START NODE->END */
    rapidxml::xml_node<>* startNode = animationNode->first_node("Start");
    rapidxml::xml_node<>* endNode = animationNode->first_node("End");

    rapidxml::xml_node<> *rotationNode = startNode->first_node("Rotation");
    rapidxml::xml_node<> *positionNode = startNode->first_node("Position");
    rapidxml::xml_node<> *scaleNode = startNode->first_node("Scale");

    glm::vec3 position = glm::vec3(std::stof(positionNode->first_attribute("x")->value()),
                                   std::stof(positionNode->first_attribute("y")->value()),
                                   std::stof(positionNode->first_attribute("z")->value()));
    glm::vec3 rotation = glm::vec3(std::stof(rotationNode->first_attribute("x")->value()),
                                   std::stof(rotationNode->first_attribute("y")->value()),
                                   std::stof(rotationNode->first_attribute("z")->value()));
    glm::vec3 scale    = glm::vec3(std::stof(scaleNode->first_attribute("x")->value()),
                                   std::stof(scaleNode->first_attribute("y")->value()),
                                   std::stof(scaleNode->first_attribute("z")->value()));
    auto* startTransform = new Transform(position, rotation, scale);

    rotationNode = endNode->first_node("Rotation");
    positionNode = endNode->first_node("Position");
    scaleNode = endNode->first_node("Scale");
    position = glm::vec3(std::stof(positionNode->first_attribute("x")->value()),
                                   std::stof(positionNode->first_attribute("y")->value()),
                                   std::stof(positionNode->first_attribute("z")->value()));
    rotation = glm::vec3(std::stof(rotationNode->first_attribute("x")->value()),
                                   std::stof(rotationNode->first_attribute("y")->value()),
                                   std::stof(rotationNode->first_attribute("z")->value()));
    scale    = glm::vec3(std::stof(scaleNode->first_attribute("x")->value()),
                                   std::stof(scaleNode->first_attribute("y")->value()),
                                   std::stof(scaleNode->first_attribute("z")->value()));
    auto* endTransform = new Transform(position, rotation, scale);
    returnNode = new AnimationLinearNode(startTransform, endTransform, parent, std::vector<Node*>{}, name);
    return  returnNode;
}

unsigned int GameState::loadSceneGraph(){
    rapidxml::xml_node<> *xmlRootNode = gameScene->first_node("Root");
    rapidxml::xml_node<> *sceneGraphNode = xmlRootNode->first_node("SceneGraph");
    rapidxml::xml_node<> *sceneGraphRootNode = sceneGraphNode->first_node("Node");

    NodeType type = (NodeType)std::stoi(sceneGraphRootNode->first_attribute("type")->value());
    std::string nodeName = sceneGraphRootNode->first_attribute("name")->value();
    if(type != PURE_TRANSFORM){
        std::cerr << "GAMESTATE::LOAD_SCENE_GRAPH::Error sceneGraphRootNode must be of type 1" << std::endl;
    }
    rapidxml::xml_node<> *transformNode = sceneGraphRootNode->first_node("Transform");
    rapidxml::xml_node<> *rotationNode = transformNode->first_node("Rotation");
    rapidxml::xml_node<> *positionNode = transformNode->first_node("Position");
    rapidxml::xml_node<> *scaleNode = transformNode->first_node("Scale");

    glm::vec3 position = glm::vec3(std::stof(positionNode->first_attribute("x")->value()),
                                   std::stof(positionNode->first_attribute("y")->value()),
                                   std::stof(positionNode->first_attribute("z")->value()));
    glm::vec3 rotation = glm::vec3(std::stof(rotationNode->first_attribute("x")->value()),
                                   std::stof(rotationNode->first_attribute("y")->value()),
                                   std::stof(rotationNode->first_attribute("z")->value()));
    glm::vec3 scale    = glm::vec3(std::stof(scaleNode->first_attribute("x")->value()),
                                   std::stof(scaleNode->first_attribute("y")->value()),
                                   std::stof(scaleNode->first_attribute("z")->value()));
    auto* transform = new Transform(position, rotation, scale);
    this->rootNode = new ObjectNode(transform, nullptr, std::vector<Node*>{}, prepareTerrainModel(),nodeName);
    rapidxml::xml_node<> *childrenNode = sceneGraphRootNode->first_node("Children");
    this->rootNode->addChildren(processChildren(rootNode, childrenNode));
    return 0;
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

        auto* transform = new Transform(position, rotation, scale);
        auto* object = new SceneObject(model->second, shader->second, transform);
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
        }else if(type == SPOT_LIGHT){
            rapidxml::xml_node<> *positionNode =  lightNode->first_node("Position");
            rapidxml::xml_node<> *directionNode = lightNode->first_node("Direction");
            rapidxml::xml_node<> *paramNode = lightNode->first_node("Params");

            glm::vec3 position = glm::vec3(std::stof(positionNode->first_attribute("x")->value()),
                                           std::stof(positionNode->first_attribute("y")->value()),
                                           std::stof(positionNode->first_attribute("z")->value()));

            glm::vec3 direction = glm::vec3(std::stof(directionNode->first_attribute("x")->value()),
                                            std::stof(directionNode->first_attribute("y")->value()),
                                            std::stof(directionNode->first_attribute("z")->value()));
            float constant = std::stof(paramNode->first_attribute("constant")->value());
            float linear = std::stof(paramNode->first_attribute("linear")->value());
            float quadratic = std::stof(paramNode->first_attribute("quadratic")->value());
            float cutOff = std::stof(paramNode->first_attribute("cutOff")->value());
            float outerCutOff = std::stof(paramNode->first_attribute("outerCutOff")->value());
            lights.push_back(new SpotLight(ambient, diffuse, specular, position, direction, constant,
                                           linear, quadratic, cutOff, outerCutOff));
        }
        foundObjectsCount++;
    }
    return foundObjectsCount;
}

void GameState::reloadShadersAndObjects() {
    for (auto& shader : shaders){
        delete shader.second;
    }
    shaders.clear();
    loadShaders();
    reloadObjects();
    reloadParams.reloadShaders = false;
}

void GameState::reloadObjects() {
    for(auto & object : objects){
        delete object;
    }
    objects.clear();
    loadObjectInstances();
    loadSceneGraph();
    reloadParams.reloadObjects = false;
}

void GameState::reloadHandle() {

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

    if(reloadParams.reloadLights){
        reloadLights();
    }else if (reloadParams.reloadShaders){
        reloadShadersAndObjects();
    }else if (reloadParams.reloadModels){
        reloadModelsAndObjects();
    }else if (reloadParams.reloadObjects){
        reloadObjects();
    }
}

void GameState::reloadModelsAndObjects() {
    for (auto& model : models){
        delete model.second;
    }
    models.clear();
    loadModels();
    loadSceneGraph();
    reloadObjects();
    reloadParams.reloadModels = false;
}

void GameState::reloadLights() {
    for(auto & light : lights){
        delete light;
    }
    lights.clear();
    loadLights();
    reloadParams.reloadLights = false;
}

SceneObject* GameState::prepareTerrainModel() {
    loadHeightMapParams();
    std::vector<Vertex> vertices;
    std::vector<Texture> textures;
    std::vector<unsigned int> indices;

    for (unsigned int i = 0; i < terrainParams.resolution; i++) {
        for (unsigned int j = 0; j < terrainParams.resolution; j++) {
            Vertex vertex;
            glm::vec3 position = glm::vec3((float(i) / (terrainParams.resolution - 1)),
                                           (0),
                                           (float(j) / (terrainParams.resolution - 1)));
            /* Height map loads normal from texture, thus there is no need to specify normal coords */
            glm::vec3 normal = glm::vec3(0.0, 0.0, 0.0);
            /* Texture coords are the same as position, since the generated plane is always unit len*/
            glm::vec2 textureCoords = glm::vec2(position.x, position.z);
            vertex.Position = position;
            vertex.Normal = normal;
            vertex.TexCoords = textureCoords;
            vertices.push_back(vertex);
        }
    }

    for (unsigned int i = 0; i < terrainParams.resolution - 1; i++) {
        for (unsigned int j = 0; j < terrainParams.resolution - 1; j++) {
            int i0 = j + i * terrainParams.resolution;
            int i1 = i0 + 1;
            int i2 = i0 + terrainParams.resolution;
            int i3 = i2 + 1;
            indices.push_back(i0);
            indices.push_back(i2);
            indices.push_back(i1);
            indices.push_back(i1);
            indices.push_back(i2);
            indices.push_back(i3);
        }
    }
    if(terrainParams.heightTexID == 1892){
        std::cout << "loading texture" << std::endl;
        terrainParams.heightTexID = loadTexture(terrainParams.heightTexPath.c_str());
        terrainParams.normalTexID = loadTexture(terrainParams.normalTexPath.c_str());
        terrainParams.diffuseTexID = loadTexture(terrainParams.diffuseTexPath.c_str());
    }
    Texture heightTex{terrainParams.heightTexID, "texture_height", terrainParams.heightTexPath};
    Texture normalTex{terrainParams.normalTexID, "texture_normal", terrainParams.normalTexPath};
    Texture diffuseTex{terrainParams.diffuseTexID, "texture_diffuse", terrainParams.diffuseTexPath};
    textures.push_back(heightTex);
    textures.push_back(normalTex);
    textures.push_back(diffuseTex);
    std::vector<Mesh> meshes;
    Mesh mesh(vertices, indices, textures);
    meshes.push_back(mesh);
    Model* model = new Model(meshes);
    auto shader = shaders.find("height");
    if(shader == shaders.end()){
        std::cerr << "GAMESTATE::PREPARE_TERRAIN_MODEL::Height shader not found, please insert the shader (did you call " <<
                     "prepare height map before prepare shaders?)" << std::endl;
        return nullptr;
    }

    SceneObject* sceneObject = new SceneObject(model, shader->second, nullptr);
    return sceneObject;
}

void GameState::loadHeightMapParams() {
    rapidxml::xml_node<> *xmlRootNode = gameScene->first_node("Root");
    rapidxml::xml_node<> *terrainNode = xmlRootNode->first_node("Terrain");
    terrainParams.scale=std::stof(terrainNode->first_attribute("scale")->value());
    terrainParams.resolution = std::stof(terrainNode->first_attribute("resolution")->value());
    terrainParams.heightTexPath = terrainNode->first_attribute("heightMapPath")->value();
    terrainParams.diffuseTexPath = terrainNode->first_attribute("diffuseMapPath")->value();
    terrainParams.normalTexPath = terrainNode->first_attribute("normalMapPath")->value();
}

void GameState::prepareFireModel() {
    float basic_textured_square_vertices[] = {
            //x    y      z     u     v
            -0.5f,  0.8f, 0.0f, 0.0f, 1.0f,
            -0.5f, -0.2f, 0.0f, 0.0f, 0.0f,
             0.5f,  0.8f, 0.0f, 1.0f, 1.0f,
            -0.5f, -0.2f, 0.0f, 0.0f, 0.0f,
             0.5f,  0.8f, 0.0f, 1.0f, 1.0f,
             0.5f, -0.2f, 0.0f, 1.0f, 0.0f
    };
    glGenBuffers(1, &fire.VBO);
    glGenVertexArrays(1, &fire.VAO);
    glBindVertexArray(fire.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, fire.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(basic_textured_square_vertices),
                 &basic_textured_square_vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof (float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof (float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    fire.fireTexture = loadTexture("../data/fire/flame_sprite_sheet3.png");
    CHECK_GL_ERROR();
}

void GameState::drawFire(glm::mat4 transform, glm::mat4* projectionMatrix, glm::mat4* cameraMatrix, float time) {

    Shader fireLightShader = *shaders.find("fire")->second;
    fireLightShader.use();
    glm::mat4 fireTransformMat = transform;
//    glm::mat4 fireTransformMat = glm::translate(glm::mat4(1.0f),*transform);
//    fireTransformMat = glm::translate(fireTransformMat,glm::vec3(0.0f, 0.5f, 0.0f));
//    fireTransformMat = glm::scale(fireTransformMat, glm::vec3(0.5, 0.5, 0.5));
    fireLightShader.setMat4fv("PVMmatrix", (*projectionMatrix) * (*cameraMatrix) * fireTransformMat);
    fireLightShader.setInt("frame", (int)(12 * time)%200);
    glBindVertexArray(fire.VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fire.fireTexture);
    fireLightShader.setInt("fireTex", 0);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    fireTransformMat = glm::rotate(fireTransformMat,glm::radians(90.0f),glm::vec3(0.0f, 1.0f, 0.0f));
    fireLightShader.setMat4fv("PVMmatrix", (*projectionMatrix) * (*cameraMatrix) * fireTransformMat);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}




