//
// Created by matejs on 08/05/2021.
//
#include "game_state.h"

GameState::GameState() {
    rapidxml::xml_document<> doc;
    std::ifstream file("../data/GameScene.xml");
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    std::string content(buffer.str());
    doc.parse<0>(&content[0]);
    rapidxml::xml_node<> *rootNode = doc.first_node("Root");
    rapidxml::xml_node<> *shadersNode = rootNode->first_node("Shaders");

    for (rapidxml::xml_node<> *shaderNode = shadersNode->first_node("Shader"); shaderNode;
         shaderNode = shaderNode->next_sibling()) {
        std::cout << "Found shader with name: " << shaderNode->first_attribute()->value() << std::endl;
    }


}
