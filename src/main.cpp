#include <iostream>

#include <glad/glad.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <GLFW/glfw3.h>
#include "shader.h"
#include "camera.h"
#include "model.h"
#include "utils.h"
#include "light.h"
#include "game_state.h"
#include "imgui_state.h"

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
    std::vector<Vertex> vertices;
    std::vector<Texture> textures;
    std::vector<unsigned int> indices;

    for (unsigned int i = 0; i < terrainResolution; i++) {
        for (unsigned int j = 0; j < terrainResolution; j++) {
            Vertex vertex;
            glm::vec3 position = glm::vec3((float(i) / (terrainResolution - 1)),
                                           (0),
                                           (float(j) / (terrainResolution - 1)));
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

    for (unsigned int i = 0; i < terrainResolution - 1; i++) {
        for (unsigned int j = 0; j < terrainResolution - 1; j++) {
            int i0 = j + i * terrainResolution;
            int i1 = i0 + 1;
            int i2 = i0 + terrainResolution;
            int i3 = i2 + 1;
            indices.push_back(i0);
            indices.push_back(i2);
            indices.push_back(i1);
            indices.push_back(i1);
            indices.push_back(i2);
            indices.push_back(i3);
        }
    }
    auto heightTexID = loadTexture(heightTexPath);
    auto normalTexID = loadTexture(normalTexPath);
    auto diffuseTexID = loadTexture(diffuseTexPath);
    Texture heightTex{heightTexID, "texture_height", heightTexPath};
    Texture normalTex{normalTexID, "texture_normal", normalTexPath};
    Texture diffuseTex{diffuseTexID, "texture_diffuse", diffuseTexPath};
    textures.push_back(heightTex);
    textures.push_back(normalTex);
    textures.push_back(diffuseTex);
    std::vector<Mesh> meshes;
    Mesh mesh(vertices, indices, textures);
    meshes.push_back(mesh);
    Model model(meshes);
    return model;
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


    GameState gamestate = GameState("../data/GameScene.xml");
    ImguiState imguiState = ImguiState();
    imguiState_ptr = &imguiState;
    gameState_ptr = &gamestate;

    Shader cubeMapShader = *gamestate.shaders.find("cubemap")->second;
    Shader heightMapShader = *gamestate.shaders.find("height")->second;
    Shader fragLightShader = *gamestate.shaders.find("frag_light")->second;

    float half_scale = scale / 2.0f;

    std::string heightFileName = "../data/terrain_floor/displaced_floor/heightmap.tga";
    std::string normalFileName = "../data/terrain_floor/displaced_floor/normalmap.PNG";
    std::string diffuseFileName = "../data/terrain_floor/displaced_floor/colormap.png";

    Model terrain = prepareTerrainModel(300, heightFileName.c_str(), normalFileName.c_str(),
                                        diffuseFileName.c_str());

    /* Skybox texture ------------------------------------- */
    #pragma region Skybox
    float skyboxVertices[] = {
            // positions
            -1.0f, 1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, 1.0f, -1.0f,
            -1.0f, 1.0f, -1.0f,

            -1.0f, -1.0f, 1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f, 1.0f, -1.0f,
            -1.0f, 1.0f, -1.0f,
            -1.0f, 1.0f, 1.0f,
            -1.0f, -1.0f, 1.0f,

            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,

            -1.0f, -1.0f, 1.0f,
            -1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, -1.0f, 1.0f,
            -1.0f, -1.0f, 1.0f,

            -1.0f, 1.0f, -1.0f,
            1.0f, 1.0f, -1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            -1.0f, 1.0f, 1.0f,
            -1.0f, 1.0f, -1.0f,

            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f, 1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f, 1.0f,
            1.0f, -1.0f, 1.0f
    };
    unsigned int skyboxID;
    glGenTextures(1, &skyboxID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxID);

    std::vector<std::string> cubemapPaths;
//    cubemapPaths.push_back("../data/envmap_stormydays/stormydays_lf.tga");
//    cubemapPaths.push_back("../data/envmap_stormydays/stormydays_rt.tga");
//    cubemapPaths.push_back("../data/envmap_stormydays/stormydays_up.tga");
//    cubemapPaths.push_back("../data/envmap_stormydays/stormydays_dn.tga");
//    cubemapPaths.push_back("../data/envmap_stormydays/stormydays_ft.tga");
//    cubemapPaths.push_back("../data/envmap_stormydays/stormydays_bk.tga");

    cubemapPaths.push_back("../data/envmap_grimmnight/grimmnight_rt.tga");
    cubemapPaths.push_back("../data/envmap_grimmnight/grimmnight_lf.tga");
    cubemapPaths.push_back("../data/envmap_grimmnight/grimmnight_up.tga");
    cubemapPaths.push_back("../data/envmap_grimmnight/grimmnight_dn.tga");
    cubemapPaths.push_back("../data/envmap_grimmnight/grimmnight_bk.tga");
    cubemapPaths.push_back("../data/envmap_grimmnight/grimmnight_ft.tga");
    int width, height, nrChannels;
    unsigned char *data;
    for (unsigned int i = 0; i < cubemapPaths.size(); i++) {
        data = stbi_load(cubemapPaths[i].c_str(), &width, &height, &nrChannels, 0);
        if (!data) {
            std::cout << "MAIN::CUBEMAP::Failed to load texture at path " << cubemapPaths[i] << std::endl;
            stbi_image_free(data);
            continue;
        }
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height,
                     0, GL_RGB, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
    unsigned int VAO, VBO;
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);
    #pragma endregion

    gamestate.lights.push_back(new DirectionalLight(glm::vec3(0.1, 0.11, 0.12),
                                          glm::vec3(0.1, 0.13, 0.135),
                                          glm::vec3(0.05, 0.05, 0.05),
                                          glm::vec3(0.0, 0.1, 0.0)));
    for (int i = 0; i < 6; i++) {
        Light *pointLight = new PointLight(glm::vec3(0, 0, 0),
                                           glm::vec3(0.0, 0.0, 1.0),
                                           glm::vec3(0.024, 0.024, 1.0),
                                           glm::vec3(18.8, -5.6, -18.4),
                                           0.2f, 0.09f, 0.012f);
        gamestate.lights.push_back(pointLight);
    }
    while (!glfwWindowShouldClose(window)) {
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

        /* skybox rendering ---------------------------------*/
        #pragma region skybox
        glm::mat4 projectionMatrix1 = glm::perspective(glm::radians(45.0f),
                                                       io.DisplaySize.x / io.DisplaySize.y, 0.1f, 500.0f);
        glDepthMask(GL_FALSE);
        cubeMapShader.use();
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxID);
        cubeMapShader.setMat4fv("view", glm::mat4(glm::mat3(gameState_ptr->camera->getViewMatrix())));
        cubeMapShader.setMat4fv("projection", projectionMatrix1);
        glm::mat4 modelMatrix1 = glm::translate(glm::mat4(1.0), glm::vec3(0, 0, 0));
        modelMatrix1 = glm::scale(modelMatrix1, glm::vec3(scaleCube, scaleCube, scaleCube));
        cubeMapShader.setMat4fv("model", modelMatrix1);
        cubeMapShader.setVec3("cameraPosition", gameState_ptr->camera->getPos());
        cubeMapShader.setInt("cubemap", 0);
        cubeMapShader.setFloat("a", gamestate.fogParams.density);
        cubeMapShader.setFloat("b", gamestate.fogParams.treshold);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDepthMask(GL_TRUE);

        #pragma endregion
        /* objects rendering */
        #pragma region objects
        fragLightShader.use();

        glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f),
                                                      io.DisplaySize.x / io.DisplaySize.y, 0.1f, 500.0f);
        glm::mat4 cameraMatrix = gameState_ptr->camera->getViewMatrix();

        for (Object object : gamestate.objects) {
            object.shader->use();
            object.shader->setMat4fv("PVMmatrix", projectionMatrix * cameraMatrix * object.transform.getTransformMat());
            object.shader->setMat4fv("Model", object.transform.getTransformMat());
            object.shader->setMat4fv("NormalModel", glm::transpose(glm::inverse(object.transform.getTransformMat())));

            object.shader->setFloat("a",gamestate.fogParams.density);
            object.shader->setFloat("b",gamestate.fogParams.treshold);
            object.shader->setBool("normalTexUsed", false);
            object.shader->setVec3("cameraPosition", gameState_ptr->camera->getPos());
            object.shader->setFloat("material.shininess", 30.0f);
            object.shader->setInt("usedLights", gamestate.lightsUsed);
            for(unsigned int i = 0; i < gamestate.lightsUsed; i++){
                gamestate.lights[i]->setLightParam(i, *object.shader);
            }
            object.model->Draw(*object.shader);
        }
        #pragma endregion
        /* height map rendering */
        #pragma region heightMap
        glm::mat4 modelMatrix = glm::mat4(1.0f);
        heightMapShader.use();
        heightMapShader.setFloat("a", gamestate.fogParams.density);
        heightMapShader.setFloat("b", gamestate.fogParams.treshold);
        heightMapShader.setVec3("cameraPosition", gameState_ptr->camera->getPos());
        heightMapShader.setFloat("material.shininess", 0.5f);
        heightMapShader.setInt("usedLights", gamestate.lightsUsed);
        glUniform1f(glGetUniformLocation(heightMapShader.ID, "scale"), scale);
        glUniform1f(glGetUniformLocation(heightMapShader.ID, "half_scale"), half_scale);
        for(unsigned int i = 0; i < gamestate.lightsUsed; i++){
            gamestate.lights[i]->setLightParam(i, heightMapShader);
        }

        modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(300.0f, 300.0f, 300.0f));
        modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.5f, 0.0f, -0.5f));
        heightMapShader.setMat4fv("PVMmatrix", projectionMatrix * cameraMatrix * modelMatrix);
        heightMapShader.setMat4fv("Model", modelMatrix);
        heightMapShader.setBool("normalTexUsed", true);
        terrain.Draw(heightMapShader);
        CHECK_GL_ERROR();
        #pragma endregion

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
}