//
// Created by matejs on 11/05/2021.
//

#include "imgui_state.h"

void ImguiState::ImguiDraw(GameState &gameState)
{
    ImGui::Begin("PGR SEMESTRAL");

    if(ImGui::Button("Reload Shaders")){ gameState.reloadParams.reloadShaders = true;}
    ImGui::SameLine();
    if(ImGui::Button("Reload Objects")){ gameState.reloadParams.reloadObjects = true;}
    ImGui::SameLine();
    if(ImGui::Button("Reload Models")){ gameState.reloadParams.reloadModels = true;}
    ImGui::SameLine();
    if(ImGui::Button("Reload Lights")){ gameState.reloadParams.reloadLights = true;}

    if(ImGui::Button("Camera to Dynamic")){gameState.camera->switchToDynamic();}
    ImGui::Text("use WASD to move around");
    ImGui::Text("use SPACE to fly up CTRL to fly downwards");
    ImGui::Text("press \"Q\" to enable cursor and disable mouse control");
    /* Open or close individual sub-windows */
    ImGui::Checkbox("Camera Parameters", &showCamWindow);
    ImGui::Checkbox("Light Parameters", &showLightsWindow);
    ImGui::Checkbox("Objects Parameters", &showObjectsWindow);
    ImGui::InputFloat("fog a", &gameState.fogParams.density, 0.001, 0.01);
    ImGui::InputFloat("fog b", &gameState.fogParams.treshold, 0.001, 0.01);

    for (unsigned int i = 0; i < 2; i++) {
        if (ImGui::Button(("Switch to camera " + std::to_string(i + 1)).c_str())) {
            gameState.camera->switchToStatic(i + 1);
            ImGui::SameLine();
        }
    }
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
                ImGui::GetIO().Framerate);
    ImGui::End();

    // 3. Show another simple window.
    if (showCamWindow) {
        glm::vec3 cameraPos = gameState.camera->getPos();
        ImGui::Begin("Camera Parameters",&showCamWindow);
        if (ImGui::Button("Close")) { showCamWindow = false; }
        ImGui::Text("Camera position is");
        ImGui::Text("x: %f", cameraPos.x);
        ImGui::SameLine();
        ImGui::Text("y: %f", cameraPos.y);
        ImGui::SameLine();
        ImGui::Text("z: %f", cameraPos.z);
        ImGui::Text("Yaw: %f   Pitch: %f", gameState.camera->getYaw(), gameState.camera->getPitch());
        ImGui::End();
    }
    if(showObjectsWindow){
        ImGui::Begin("Object Properties", &showObjectsWindow);
        std::queue<Node*> nodes;
        nodes.push(gameState.rootNode);
        while(!nodes.empty()){
           Node* currNode = nodes.front();
           nodes.pop();
           if(ImGui::CollapsingHeader(("Object " + currNode->name).c_str())){
               ImGui::SliderFloat3(("position_"+currNode->name).c_str(), (float*) &currNode->transform->position, -100, 100);
               ImGui::SliderFloat3(("scale"+currNode->name).c_str(), (float*) &currNode->transform->scale, 0, 2);
           }
           for (auto child : currNode->children){
               nodes.push(child);
           }
        }
        ImGui::End();
    }

    /* lights prop */
    if (showLightsWindow) {
        ImGui::Begin("Lights Properties", &showLightsWindow);
        ImGui::SliderInt("Used Lights", &gameState.lightsUsed, 1, gameState.lights.size());
        std::vector<Light*>& lights = gameState.lights;

        for(unsigned int i = 0; i < gameState.lightsUsed; i++){
            if(ImGui::CollapsingHeader(("Light " + std::to_string(i)).c_str())){
                ImVec4 Ambient = ImVec4(lights[i]->ambient.x,lights[i]->ambient.y,lights[i]->ambient.z,1.0);
                ImVec4 Diffuse = ImVec4(lights[i]->diffuse.x,lights[i]->diffuse.y,lights[i]->diffuse.z,1.0);
                ImVec4 Specular = ImVec4(lights[i]->specular.x,lights[i]->specular.y,lights[i]->specular.z,1.0);
                ImGui::ColorEdit3("Ambient",(float*)&Ambient);
                ImGui::ColorEdit3("Diffuse",(float*)&Diffuse);
                ImGui::ColorEdit3("Specular",(float*)&Specular);
                lights[i]->ambient = glm::vec3(Ambient.x, Ambient.y, Ambient.z);
                lights[i]->diffuse = glm::vec3(Diffuse.x, Diffuse.y, Diffuse.z);
                lights[i]->specular = glm::vec3(Specular.x, Specular.y, Specular.z);

                if( lights[i]->type == DIRECTIONAL_LIGHT){
                    ImGui::Text("Direction");
                    auto* light = (DirectionalLight*)lights[i];
                    ImVec4 Direction = ImVec4(light->direction.x, light->direction.y, light->direction.z, 1.0);
                    ImGui::InputFloat("x",&Direction.x , 0.1, 1.0);
                    ImGui::InputFloat("y",&Direction.y , 0.1, 1.0);
                    ImGui::InputFloat("z",&Direction.z , 0.1, 1.0);
                    light->direction = glm::vec3(Direction.x, Direction.y, Direction.z);
                } else {
                    ImGui::Text("Position");
                    auto* light = (PointLight*)lights[i];
                    ImVec4 Position = ImVec4(light->position.x, light->position.y, light->position.z, 1.0);
                    ImGui::InputFloat("x",&Position.x ,  0.1, 1.0);
                    ImGui::InputFloat("y",&Position.y ,  0.1, 1.0);
                    ImGui::InputFloat("z",&Position.z ,  0.1, 1.0);
                    light->position = glm::vec3(Position.x, Position.y, Position.z);
                    ImGui::InputFloat("Constant", &light->constant, 0.1, 1.0);
                    ImGui::InputFloat("Linear", &light->linear, 0.1, 1.0);
                    ImGui::InputFloat("Quadratic", &light->quadratic, 0.01, 0.1);
                }
            }
        }
        ImGui::End();
    }
}
