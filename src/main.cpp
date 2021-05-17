#include <iostream>
#include <queue>

#include <glad/glad.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <GLFW/glfw3.h>
#include "utils.h"
#include "light.h"
#include "game_state.h"
#include "imgui_state.h"
#include "bezier.h"
#include "cubemap.h"
#include "quaternion.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "stb_image.h"

static std::vector<std::string> Names;

static float deltaTime = 0.0f;
static float lastFrame = 0.0f;
static float scale = 0.17;
static float translation = 12;

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

//    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
//        glfwSetWindowShouldClose(window, true);
    int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    if(state == GLFW_PRESS && gameState_ptr->mouseParameters.pressDelay < 0){
        unsigned int ClickedIndex;
        double xpos, ypos;
        if(!gameState_ptr->mouseParameters.mouseControl){
            glfwGetCursorPos(window, &xpos, &ypos);
        }else {
            xpos = 1920/2;
            ypos = 1080/2;
        }
        glReadPixels(xpos, 1080-ypos-1, 1, 1, GL_STENCIL_INDEX, GL_UNSIGNED_INT, &ClickedIndex);
        if(ClickedIndex > 0){
            if(Names[ClickedIndex-1].find("fire") != std::string::npos){
                if (gameState_ptr->fireplace_active)
                {
                    gameState_ptr->fireplace_active = false;
                    gameState_ptr->lightsUsed = 2;
                }else{
                    gameState_ptr->fireplace_active = true;
                    gameState_ptr->lightsUsed = 3;
                }
            }
        }
        gameState_ptr->mouseParameters.pressDelay = 0.2f;
    }

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS &&
       (gameState_ptr->mouseParameters.pressDelay < 0)) {
        if (!gameState_ptr->mouseParameters.mouseControl)
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

    Quaternion* qat1 = new Quaternion(0.0, 77, 0);
    Quaternion* qat2 = new Quaternion(22, 0.0, 0.0);
    Quaternion* qat3 = new Quaternion(0.0, 0.0, 44);
    Quaternion quat3 = *qat1 * *qat2 * *qat3 ;
    glm::mat4 rot1 = glm::rotate(glm::mat4(1.0f), glm::radians(77.0f) , glm::vec3(0.0f, 1.0f, 0.0f));

    glm::mat4 test = glm::mat4(1.0f);
    test[0][0] = 2;
    test[0][1] = 2;
    test[0][2] = 2;
    std::cout << glm::to_string(test) << std::endl;
    rot1 = glm::rotate(rot1, glm::radians(22.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    rot1 = glm::rotate(rot1, glm::radians(44.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    std::cout << glm::to_string((quat3.getRotMatrix())) << std::endl;
    std::cout << glm::to_string(rot1) << std::endl;

    GameState gamestate = GameState("sceneGraph.xml");
    ImguiState imguiState = ImguiState();
    imguiState_ptr = &imguiState;
    gameState_ptr = &gamestate;

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
        glClearStencil(0);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        glEnable(GL_STENCIL_TEST);

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

//        for (SceneObject* object : gamestate.objects) {
//            object->shader->use();
//            object->shader->setMat4fv("PVMmatrix", projectionMatrix * cameraMatrix * object->transform->getTransformMat());
//            object->shader->setMat4fv("Model", object->transform->getTransformMat());
//            object->shader->setMat4fv("NormalModel", glm::transpose(glm::inverse(object->transform->getTransformMat())));
//
//            object->shader->setBool("normalTexUsed", false);
//            object->shader->setFloat("material.shininess", 30.0f);
//            object->shader->setInt("usedLights", gamestate.lightsUsed);
//            for(unsigned int i = 0; i < gamestate.lightsUsed; i++){
//                gamestate.lights[i]->setLightParam(i, *object->shader);
//            }
//            CHECK_GL_ERROR();
//            object->model->Draw(*object->shader);
//        }
        /* experimental scene draw */
        std::queue<Node*> nodes;
        nodes.push(gamestate.rootNode);

        Names.clear();
        int counter = 1;
        while(nodes.size() != 0){
            Node* currNode = nodes.front();
            nodes.pop();

            if(currNode->type == OBJECT){
                SceneObject* object = ((ObjectNode*)currNode)->object;
                object->shader->use();
                object->shader->setMat4fv("PVMmatrix", projectionMatrix * cameraMatrix * currNode->getTransform(t));
                object->shader->setMat4fv("Model", currNode->getTransform(t));
                object->shader->setMat4fv("NormalModel", glm::transpose(glm::inverse(currNode->getTransform(t))));
                object->shader->setBool("normalTexUsed", false);
                object->shader->setFloat("material.shininess", 30.0f);
                object->shader->setInt("usedLights", gamestate.lightsUsed);
                for(unsigned int i = 0; i < gamestate.lightsUsed; i++){
                    gamestate.lights[i]->setLightParam(i, *object->shader);
                }
                if(currNode->name.find("fire") != std::string::npos && gamestate.fireplace_active){
                    gamestate.drawFire(currNode->getTransform(t), &projectionMatrix, &cameraMatrix, (float)glfwGetTime());
                }
                if(currNode->name == "root"){
                    object->shader->setBool("normalTexUsed", true);
                    object->shader->setFloat("scale", gamestate.terrainParams.scale);
                }
                CHECK_GL_ERROR();
                Names.push_back(currNode->name);
                glStencilFunc(GL_ALWAYS, counter++, -1);
                object->model->Draw(*object->shader);
                CHECK_GL_ERROR();
            }
            for(auto child : currNode->children){
                nodes.push(child);
            }
        }

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