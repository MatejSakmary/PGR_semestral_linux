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

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "stb_image.h"

static std::vector<std::string> Names;

static float lastFrame = 0.0f;

static GameState* gameState_ptr = nullptr;
static ImguiState* imguiState_ptr = nullptr;

void mouseCallback(GLFWwindow *window, double x, double y) {
    /* Check if I want to move camera by moving mouse */
    if (!gameState_ptr->mouseParameters.mouseControl) return;

    /* Prevent initial jump of camera orientation when switching on mouse control */
    if (gameState_ptr->mouseParameters.firstMouseInput) {
        gameState_ptr->mouseParameters.lastX = x;
        gameState_ptr->mouseParameters.lastY = y;
        gameState_ptr->mouseParameters.firstMouseInput = false;
    }

    /* Update camera parameters */
    float xoffset = x - gameState_ptr->mouseParameters.lastX;
    float yoffset = gameState_ptr->mouseParameters.lastY - y;
    gameState_ptr->mouseParameters.lastX = x;
    gameState_ptr->mouseParameters.lastY = y;
    gameState_ptr->camera->updateFrontVec(xoffset, yoffset);
}

/* Handle most of general input */
void processInput(GLFWwindow *window) {
    float currentFrame = glfwGetTime();
    /* update delta time */
    gameState_ptr->deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    gameState_ptr->mouseParameters.pressDelay -= gameState_ptr->deltaTime;

    /* end program */
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
        glfwSetWindowShouldClose(window, true);
    }

    /* Handle left click ----------------------------------------------------------*/
    int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    if(state == GLFW_PRESS && gameState_ptr->mouseParameters.pressDelay < 0){
        unsigned int ClickedIndex;
        double xpos, ypos;
        /* If mouse cursor is enable use it's position, if it is disabled use center of screen*/
        if(!gameState_ptr->mouseParameters.mouseControl){
            glfwGetCursorPos(window, &xpos, &ypos);
        }else {
            /*TODO: Change from hardcoded 1920 and 1080 to actuall screen size */
            xpos = 1920/2;
            ypos = 1080/2;
        }
        /*Check for clickable objects, if one is hit toggle it's state */
        glReadPixels(xpos, 1080-ypos-1, 1, 1, GL_STENCIL_INDEX, GL_UNSIGNED_INT, &ClickedIndex);
        if(ClickedIndex > 0){
            if(Names[ClickedIndex-1].find("fire") != std::string::npos){
                gameState_ptr->lights[2]->enabled = !gameState_ptr->lights[2]->enabled;
            }else if(Names[ClickedIndex-1].find("portal") != std::string::npos){
                gameState_ptr->lights[1]->enabled = !gameState_ptr->lights[1]->enabled;
            }else if(Names[ClickedIndex-1].find("UFO") != std::string::npos){
                gameState_ptr->lights[3]->enabled = !gameState_ptr->lights[3]->enabled;
            }
        }
        /*Set press delay so that when one click spans multiple frames there is no rapid switching on and off */
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
        gameState_ptr->camera->forward(gameState_ptr->deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        gameState_ptr->camera->back(gameState_ptr->deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        gameState_ptr->camera->left(gameState_ptr->deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        gameState_ptr-> camera->right(gameState_ptr->deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        gameState_ptr->camera->up(gameState_ptr->deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        gameState_ptr->camera->down(gameState_ptr->deltaTime);
    }
}

int main() {
    const char *glsl_version = "#version 130";

    /* Init GLFW----------------------------------------------------------------------------- */
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

    /* Init glad (Load OGL function pointers */
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    /* Enable Depth testing so objects are rendered properly */
    glEnable(GL_DEPTH_TEST);

    /* setup imgui context -----------------------------------------------------------------------------------*/
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

    Bezier bezier = Bezier(glm::vec3(-50.0f, 20.0f, 10.0f),
                                glm::vec3(-50.0f, 20.0f, -10.0f),
                                glm::vec3(200.0f, 20.0f, 170.0f),
                                glm::vec3(20.0f, 20.0f, -320.0f));
    /* Prepare skybox texture pahs ------------------------------------------------------------ */
    /* TODO: if there is time, move those paths to config xml and load them in game state constructor*/
    std::vector<std::string> dayCubemapPaths;
    dayCubemapPaths.emplace_back("../data/envmap_grimmnight/grimmnight_rt.tga");
    dayCubemapPaths.emplace_back("../data/envmap_grimmnight/grimmnight_lf.tga");
    dayCubemapPaths.emplace_back("../data/envmap_grimmnight/grimmnight_up.tga");
    dayCubemapPaths.emplace_back("../data/envmap_grimmnight/grimmnight_dn.tga");
    dayCubemapPaths.emplace_back("../data/envmap_grimmnight/grimmnight_bk.tga");
    dayCubemapPaths.emplace_back("../data/envmap_grimmnight/grimmnight_ft.tga");
    std::vector<std::string> nightCubemapPaths;
    nightCubemapPaths.emplace_back("../data/envmap_miramar/miramar_rt.tga");
    nightCubemapPaths.emplace_back("../data/envmap_miramar/miramar_lf.tga");
    nightCubemapPaths.emplace_back("../data/envmap_miramar/miramar_up.tga");
    nightCubemapPaths.emplace_back("../data/envmap_miramar/miramar_dn.tga");
    nightCubemapPaths.emplace_back("../data/envmap_miramar/miramar_bk.tga");
    nightCubemapPaths.emplace_back("../data/envmap_miramar/miramar_ft.tga");
    Cubemap cubemap = Cubemap(nightCubemapPaths, dayCubemapPaths, 150);
    CHECK_GL_ERROR();
    PrepareHandLoadedObject();
    CHECK_GL_ERROR();

    /* Main render loop -------------------------------------------------------------*/
    while (!glfwWindowShouldClose(window)) {
        /* Update light positions and intensities */
        /* TODO: Move into "update" function */
        float t = (float)(glm::sin(glfwGetTime()/3)+1)/2;
        PointLight* point = (PointLight*)gamestate.lights[1];
        /* TODO: Make the light intensity fluctuation more intensive -> target quadratic component?*/
        point->linear = (sin(glfwGetTime())+1)/10;

        /* Update UFO reflector */
        SpotLight* spot = (SpotLight*)gamestate.lights[3];
        spot->position =  gamestate.ufoNode->getTransform(t) * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        spot->direction = glm::mat3(gamestate.ufoNode->getTransform(t)) * glm::vec3(0.0f, 0.0f, -1.0f);

        processInput(window);

        /* ImGui render process ------------------------*/
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGuiIO &io = ImGui::GetIO();
        imguiState_ptr->ImguiDraw(gamestate);
        ImGui::Render();

        /* Setup OpenGL framebuffer ------------------------*/
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        /* This is the value stencil buffer is cleared with -> similar to clear color for framebuffer*/
        glClearStencil(0);
        /* Setup stencil function for object picking -> Update only if both depth and stencil tests pass otherwise
         * keep previous value */
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        glEnable(GL_STENCIL_TEST);

        /* Init camera and projection matrix */
        glm::mat4 cameraMatrix = gamestate.camera->getViewMatrix(t);
        glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f),
                                                       io.DisplaySize.x / io.DisplaySize.y, 0.1f, 500.0f);

        CHECK_GL_ERROR();
        /* TODO: Uniform buffer? */
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
        /* skybox rendering -------------------------------------------------------------------------------*/
        #pragma region skybox
        gamestate.shaders.find("cubemap")->second->use();
        CHECK_GL_ERROR();
        unsigned int lights_index = 0;
        /* Set all enabled light uniforms in shader ------------------------*/
        for(unsigned int i = 0; i < gamestate.lights.size(); i++){
            if(gamestate.lights[i]->enabled){
                gamestate.lights[i]->setLightParam(lights_index++, *gamestate.shaders.find("cubemap")->second);
            }
        }
        /* This is the counter use in previous for loop which at the end has the count of all enabled lights */
        gamestate.lightsUsed = lights_index;
        gamestate.shaders.find("cubemap")->second->setInt("usedLights", gamestate.lightsUsed);
        /* Don't write to depth buffer, since skybox is rendered as unit cube at the scene origin (after view transform)
         * If depth test was not disabled I would only see objects that are closer than this cube -> almost all objects
         * would fail depth test */
        glDepthMask(GL_FALSE);
        cubemap.Draw(*gamestate.shaders.find("cubemap")->second, (float)glfwGetTime()/4);
        glDepthMask(GL_TRUE);
        #pragma endregion

        /* objects rendering ----------------------------------------------------------------------------*/
        #pragma region objects
        /* hand drawn object */
        Shader* rockShader = gamestate.shaders.find("frag_light")->second;
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-100, 5.9, -100));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.01, 0.01, 0.01));
        rockShader->use();
        rockShader->setMat4fv("PVMmatrix", projectionMatrix * cameraMatrix * model);
        rockShader->setMat4fv("Model", model);
        rockShader->setMat4fv("NormalModel", glm::transpose(glm::inverse(model)));
        rockShader->setBool("normalTexUsed", false);
        rockShader->setFloat("material.shininess", 30.0f);
        rockShader->setInt("usedLights", gamestate.lightsUsed);
        DrawHandLoadedObject(rockShader);
        /* This is standard Stack parse of a tree*/
        std::queue<Node*> nodes;
        nodes.push(gamestate.rootNode);

        /* TODO: Name Setting should be only done in first frame and and after graph reload not each frame*/
        Names.clear();
        /*This counter is used as the value written into stencil buffer -> essentially id of object */
        int counter = 1;
        while(!nodes.empty()){
            Node* currNode = nodes.front();
            nodes.pop();

            if(currNode->type == OBJECT){
                /* Set uniforms -------------------------------------------------------------------------------*/
                SceneObject* object = ((ObjectNode*)currNode)->object;
                object->shader->use();
                object->shader->setMat4fv("PVMmatrix", projectionMatrix * cameraMatrix * currNode->getTransform(t));
                object->shader->setMat4fv("Model", currNode->getTransform(t));
                object->shader->setMat4fv("NormalModel", glm::transpose(glm::inverse(currNode->getTransform(t))));
                object->shader->setBool("normalTexUsed", false);
                object->shader->setFloat("material.shininess", 30.0f);
                object->shader->setInt("usedLights", gamestate.lightsUsed);
                lights_index = 0;
                /* Set light uniforms---------------------------------------------------------------*/
                for(unsigned int i = 0; i < gamestate.lights.size(); i++){
                    if(gamestate.lights[i]->enabled){
                        gamestate.lights[i]->setLightParam(lights_index++, *object->shader);
                    }
                }
                /* TODO: This is really messy way to add fire animation */
                /* Add fire animation -> very hacky but I don't have time to come up with something better*/
                if(currNode->name.find("fire") != std::string::npos && gamestate.lights[2]->enabled){
                    gamestate.drawFire(currNode->getTransform(t), &projectionMatrix, &cameraMatrix, (float)glfwGetTime());
                }
                /* Again very messy way to render terrain but not enough time :( */
                if(currNode->name == "root"){
                    object->shader->setBool("normalTexUsed", true);
                    object->shader->setFloat("scale", gamestate.terrainParams.scale);
                }
                CHECK_GL_ERROR();
                /* The order at which nodes are rendered corresponds to the order they are stored in names vector
                 * This makes it so that I can easily lookup Node name which was clicked on by retrieving the value
                 * stored in stencil buffer and using it as index-1 into names vector */
                Names.push_back(currNode->name);
                /* Always write to stencil buffer with a mask of 255 -> since the size of stencil buffer is 255
                 * every value will be written to stencil buffer
                 * NOTE -> stencil test is performed after depth test, so fragment first has to pass depth test in
                 * order have it's index written into stencil buffer, this means only the "front most" object
                 * is selected by mouse */
                glStencilFunc(GL_ALWAYS, counter++, -1);
                object->model->Draw(*object->shader);
                CHECK_GL_ERROR();
            }
            /*Add node's children to queue for rendering */
            for(auto child : currNode->children){
                nodes.push(child);
            }
        }

        #pragma endregion

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
        glfwPollEvents();
        /* If there was request to reload any part of the scene do it here before the next render cycle */
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