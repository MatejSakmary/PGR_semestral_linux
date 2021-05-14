#include <iostream>
#include <queue>

#include <glad/glad.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <GLFW/glfw3.h>
#include "shader.h"
#include "model.h"
#include "utils.h"
#include "light.h"
#include "game_state.h"
#include "imgui_state.h"
#include "bezier.h"
#include "cubemap.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "stb_image.h"


static float deltaTime = 0.0f;
static float lastFrame = 0.0f;
static float scale = 0.17;
static float translation = 12;
static float scaleCube = 150;

static GameState* gameState_ptr = nullptr;
static ImguiState* imguiState_ptr = nullptr;

void setupTriangle(unsigned int &VBO, unsigned int &VAO) {
    float vertices[] = {
            -0.5f, -0.5f, 0.0f,
            0.5f, -0.5f, 0.0f,
            0.0f, 0.5f, 0.0f
    };
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);
}

void mouseCallback(GLFWwindow *window, double x, double y) {
    if (!gameState_ptr->mouseParameters.mouseControl) return;

    if (gameState_ptr->mouseParameters.firstMouseInput) {
        gameState_ptr->mouseParameters.lastX = x;
        gameState_ptr->mouseParameters.lastY = y;
        gameState_ptr->mouseParameters.firstMouseInput = false;
    }

    float xoffset = x - gameState_ptr->mouseParameters.lastX;
    float yoffset = gameState_ptr->mouseParameters.lastY - y;
    gameState_ptr->mouseParameters.lastX = x;
    gameState_ptr->mouseParameters.lastY = y;
    gameState_ptr->camera->updateFrontVec(xoffset, yoffset);
}

void processInput(GLFWwindow *window) {
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    gameState_ptr->mouseParameters.pressDelay -= deltaTime;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS &&
       (gameState_ptr->mouseParameters.pressDelay < 0)) {
        if (gameState_ptr->mouseParameters.mouseControl)
            std::cout << "mouse control is now enabled" << std::endl;
        else
            std::cout << "mouse control is now disabled" << std::endl;
            gameState_ptr->mouseParameters.firstMouseInput = true;

        gameState_ptr->mouseParameters.pressDelay = 0.5f;
        gameState_ptr->mouseParameters.mouseControl = !gameState_ptr->mouseParameters.mouseControl;
        if (gameState_ptr->mouseParameters.mouseControl) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        } else{
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }

    if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS) {
        gameState_ptr->camera->switchToStatic(1);
    }
    if (glfwGetKey(window, GLFW_KEY_F2) == GLFW_PRESS) {
        gameState_ptr->camera->switchToStatic(2);
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        gameState_ptr->camera->forward(deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        gameState_ptr->camera->back(deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        gameState_ptr->camera->left(deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        gameState_ptr-> camera->right(deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        gameState_ptr->camera->up(deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        gameState_ptr->camera->down(deltaTime);
    }
}

Model prepareTerrainModel(unsigned int terrainResolution, const char *heightTexPath,
                          const char *normalTexPath, const char *diffuseTexPath) {
}

int main() {
    const char *glsl_version = "#version 130";

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(1920, 1080, "PGR_Semestral", nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetCursorPosCallback(window, mouseCallback);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glEnable(GL_DEPTH_TEST);

    /* setup imgui context ---------------*/
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void) io;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    GameState gamestate = GameState("sceneGraph.xml");
    ImguiState imguiState = ImguiState();
    imguiState_ptr = &imguiState;
    gameState_ptr = &gamestate;


    float half_scale = scale / 2.0f;

//    std::string heightFileName = "";
//    std::string normalFileName = "";
//    std::string diffuseFileName = "";
//
//    Model terrain = prepareTerrainModel(300, heightFileName.c_str(), normalFileName.c_str(),
//                                        diffuseFileName.c_str());

    Bezier bezier = Bezier(glm::vec3(-50.0f, 20.0f, 10.0f),
                                glm::vec3(-50.0f, 20.0f, -10.0f),
                                glm::vec3(200.0f, 20.0f, 170.0f),
                                glm::vec3(20.0f, 20.0f, -320.0f));
    /* Skybox texture ------------------------------------- */
    std::vector<std::string> dayCubemapPaths;
    dayCubemapPaths.emplace_back("../data/envmap_grimmnight/grimmnight_rt.tga");
    dayCubemapPaths.emplace_back("../data/envmap_grimmnight/grimmnight_lf.tga");
    dayCubemapPaths.emplace_back("../data/envmap_grimmnight/grimmnight_up.tga");
    dayCubemapPaths.emplace_back("../data/envmap_grimmnight/grimmnight_dn.tga");
    dayCubemapPaths.emplace_back("../data/envmap_grimmnight/grimmnight_bk.tga");
    dayCubemapPaths.emplace_back("../data/envmap_grimmnight/grimmnight_ft.tga");
    /* Second cubemap texture */
    std::vector<std::string> nightCubemapPaths;
    nightCubemapPaths.emplace_back("../data/envmap_miramar/miramar_rt.tga");
    nightCubemapPaths.emplace_back("../data/envmap_miramar/miramar_lf.tga");
    nightCubemapPaths.emplace_back("../data/envmap_miramar/miramar_up.tga");
    nightCubemapPaths.emplace_back("../data/envmap_miramar/miramar_dn.tga");
    nightCubemapPaths.emplace_back("../data/envmap_miramar/miramar_bk.tga");
    nightCubemapPaths.emplace_back("../data/envmap_miramar/miramar_ft.tga");
    Cubemap cubemap = Cubemap(nightCubemapPaths, dayCubemapPaths, 150);
    CHECK_GL_ERROR();
    /* Geometry construction */

    /*fireplace*/
    float basic_textured_square_vertices[] = {
            //x    y      z     u     v
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
            1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
            1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
            1.0f, -1.0f, 0.0f, 1.0f, 0.0f
    };
    unsigned int VBO, VAO;
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(basic_textured_square_vertices),
                 &basic_textured_square_vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof (float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof (float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    CHECK_GL_ERROR();
    unsigned int fireTexture = loadTexture("../data/fire/flame_sprite_sheet3.png");

    while (!glfwWindowShouldClose(window)) {
        float t = (float)(glm::sin(glfwGetTime()/3)+1)/2;
        PointLight* point = (PointLight*)gamestate.lights[1];
        point->linear = (sin(glfwGetTime())+1)/10;

        processInput(window);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGuiIO &io = ImGui::GetIO();

        imguiState_ptr->ImguiDraw(gamestate);
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 cameraMatrix = gamestate.camera->getViewMatrix(t);
        glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f),
                                                       io.DisplaySize.x / io.DisplaySize.y, 0.1f, 500.0f);

        CHECK_GL_ERROR();
        /* set common shader variables for each shader probably should be uniform buffer
         * but I don't have time */
        for(const auto& shader : gamestate.shaders){
            shader.second->use();
            shader.second->setMat4fv("view", glm::mat4(glm::mat3(gameState_ptr->camera->getViewMatrix(t))));
            shader.second->setMat4fv("projection", projectionMatrix);
            shader.second->setVec3("cameraPosition", gameState_ptr->camera->getPos());
            shader.second->setFloat("a",gamestate.fogParams.density);
            shader.second->setFloat("b",gamestate.fogParams.treshold);
            CHECK_GL_ERROR();
        }
        /* skybox rendering ---------------------------------*/
        #pragma region skybox
        glDepthMask(GL_FALSE);
        cubemap.Draw(*gamestate.shaders.find("cubemap")->second, (float)glfwGetTime());
        glDepthMask(GL_TRUE);
        #pragma endregion
        /* objects rendering */
        #pragma region objects

        /* fire draw */
        Shader fireLightShader = *gamestate.shaders.find("fire")->second;
        fireLightShader.use();
        glm::mat4 fireTransformMat = glm::translate(glm::mat4(1.0f), gamestate.objects[0]->transform->position + glm::vec3(0.0f, 0.5f, 0.0f));
        fireTransformMat = glm::scale(fireTransformMat, glm::vec3(0.5, 0.5, 0.5));
        fireLightShader.setMat4fv("PVMmatrix", projectionMatrix * cameraMatrix * fireTransformMat);
        fireLightShader.setInt("frame", (int)(6 * glfwGetTime())%200);
        glBindVertexArray(VAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, fireTexture);
        fireLightShader.setInt("fireTex", 0);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        fireTransformMat = glm::rotate(fireTransformMat,glm::radians(90.0f),glm::vec3(0.0f, 1.0f, 0.0f));
        fireLightShader.setMat4fv("PVMmatrix", projectionMatrix * cameraMatrix * fireTransformMat);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        for (SceneObject* object : gamestate.objects) {
            object->shader->use();
            object->shader->setMat4fv("PVMmatrix", projectionMatrix * cameraMatrix * object->transform->getTransformMat());
            object->shader->setMat4fv("Model", object->transform->getTransformMat());
            object->shader->setMat4fv("NormalModel", glm::transpose(glm::inverse(object->transform->getTransformMat())));

            object->shader->setBool("normalTexUsed", false);
            object->shader->setFloat("material.shininess", 30.0f);
            object->shader->setInt("usedLights", gamestate.lightsUsed);
            for(unsigned int i = 0; i < gamestate.lightsUsed; i++){
                gamestate.lights[i]->setLightParam(i, *object->shader);
            }
            CHECK_GL_ERROR();
            object->model->Draw(*object->shader);
        }
        /* experimental scene draw */
        std::queue<Node*> nodes;
        nodes.push(gamestate.rootNode);

        while(nodes.size() != 0){
            Node* currNode = nodes.front();
            nodes.pop();

            if(currNode->type == OBJECT){
                SceneObject* object = ((ObjectNode*)currNode)->object;
                object->shader->use();
                object->shader->setMat4fv("PVMmatrix", projectionMatrix * cameraMatrix * currNode->getTransform());
                object->shader->setMat4fv("Model", currNode->getTransform());
                object->shader->setMat4fv("NormalModel", glm::transpose(currNode->getTransform()));
                object->shader->setBool("normalTexUsed", false);
                object->shader->setFloat("material.shininess", 30.0f);
                object->shader->setInt("usedLights", gamestate.lightsUsed);
                for(unsigned int i = 0; i < gamestate.lightsUsed; i++){
                    gamestate.lights[i]->setLightParam(i, *object->shader);
                }
                if(currNode->name == "root"){
                    object->shader->setBool("normalTexUsed", true);
                    object->shader->setFloat("scale", gamestate.terrainParams.scale);
                }
                CHECK_GL_ERROR();
                object->model->Draw(*object->shader);
                CHECK_GL_ERROR();
            }
            for(auto child : currNode->children){
                nodes.push(child);
            }
        }

        #pragma endregion
        /* height map rendering */
        #pragma region heightMap
//        glm::mat4 modelMatrix = glm::mat4(1.0f);
//        Shader heightMapShader = *gamestate.shaders.find("height")->second;
//        heightMapShader.use();
//        heightMapShader.setFloat("material.shininess", 0.5f);
//        heightMapShader.setInt("usedLights", gamestate.lightsUsed);
//        glUniform1f(glGetUniformLocation(heightMapShader.ID, "scale"), scale);
//        glUniform1f(glGetUniformLocation(heightMapShader.ID, "half_scale"), half_scale);
//        for(unsigned int i = 0; i < gamestate.lightsUsed; i++){
//            gamestate.lights[i]->setLightParam(i, heightMapShader);
//        }
//        modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
//        modelMatrix = glm::scale(modelMatrix, glm::vec3(300.0f, 300.0f, 300.0f));
//        modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.5f, 0.0f, -0.5f));
//
//        heightMapShader.setMat4fv("PVMmatrix", projectionMatrix * cameraMatrix * modelMatrix);
//        heightMapShader.setMat4fv("Model", modelMatrix);
//        heightMapShader.setBool("normalTexUsed", true);
//        terrain.Draw(heightMapShader);
//        CHECK_GL_ERROR();
        #pragma endregion

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
        glfwPollEvents();
        if(gamestate.reloadParams.reloadShaders ||
           gamestate.reloadParams.reloadModels ||
           gamestate.reloadParams.reloadLights ||
           gamestate.reloadParams.reloadObjects ){
            gamestate.reloadHandle();
        }
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
}