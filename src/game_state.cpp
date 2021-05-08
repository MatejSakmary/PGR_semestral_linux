//
// Created by matejs on 08/05/2021.
//
#include "game_state.h"
GameState::GameState(std::string xmlPath)
{
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
            std::cerr << "GAMESTATE::LOADSHADERS::ERROR::Found two or more shaders with duplicate names not inserting" << std::endl;
            continue;
        } else {
            foundShadresCount++;
            std::cout << "GAMESTATE::LOADSHADERS::Inserting shader with name " << name << std::endl;
            shaders[name] = shader;
        }
    }
    return foundShadresCount;
}
