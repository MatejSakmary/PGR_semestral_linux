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
#include "game_state.h"
#include "light.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "stb_image.h"

static std::vector<Light *> lights;
static Camera *camera = nullptr;
static float lightAngle = 0.0f;

static float lastX = 0.0f;
static float lastY = 0.0f;

static float deltaTime = 0.0f;
static float lastFrame = 0.0f;
static float pressDelay = -1.0f;
static bool firstFrame = true;
static bool mouseControl = false;
static float scale = 0.17;
static float a = 0.0035;
static float b = 0.161;

bool show_another_window = false;
bool show_Lights_window = false;
bool show_Objects_window = false;
static GameState* gameState_ptr = nullptr;

ImVec4 clear_color = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);

static int lights_used = 2;


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

void ImGuiDraw() {
    ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

    ImGui::Text("use WASD to move around");
    ImGui::Text("use SPACE to fly up CTRL to fly downwards");
    ImGui::Text("press \"Q\" to enable cursor and disable mouse control");  // Display some text (you can use a format strings too)
    ImGui::Checkbox("Camera Parameters", &show_another_window);
    ImGui::Checkbox("Light Parameters", &show_Lights_window);
    ImGui::Checkbox("Objects Parameters", &show_Objects_window);
    ImGui::SliderFloat("Terrain height scale", &scale, 0.0, 1.0);
    ImGui::SliderFloat("fog a", &a, 0.0, 0.1);
    ImGui::SliderFloat("fog b", &b, 0.0, 0.3);

    ImGui::ColorEdit3("clear color", (float *) &clear_color); // Edit 3 floats representing a color

    for (unsigned int i = 0; i < 2; i++) {
        if (ImGui::Button(("Switch to camera " + std::to_string(i + 1)).c_str())) {
            camera->switchToStatic(i + 1);
            ImGui::SameLine();
        }
    }
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
                ImGui::GetIO().Framerate);
    ImGui::End();

    // 3. Show another simple window.
    if (show_another_window) {
        glm::vec3 cameraPos = camera->getPos();
        ImGui::Begin("Camera Parameters",&show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        if (ImGui::Button("Close")) { show_another_window = false; }
        ImGui::Text("Camera position is");
        ImGui::Text("x: %f", cameraPos.x);
        ImGui::SameLine();
        ImGui::Text("y: %f", cameraPos.y);
        ImGui::SameLine();
        ImGui::Text("z: %f", cameraPos.z);
        ImGui::Text("Yaw: %f   Pitch: %f", camera->getYaw(), camera->getPitch());
        ImGui::End();
    }
    if(show_Objects_window){
        ImGui::Begin("Object Properties", &show_Objects_window);
        for(unsigned int i = 0; i < gameState_ptr->objects.size(); i++)
        {
            Object& currObj = gameState_ptr->objects[i];
            if(ImGui::CollapsingHeader(("Object " + std::to_string(i)).c_str())){
                ImGui::SliderFloat3(("Position_" + std::to_string(i)).c_str(), (float*) &currObj.transform.position, -50.0f, 50.0f);
                ImGui::SliderFloat3(("Rotation_" + std::to_string(i)).c_str(), (float*) &currObj.transform.rotation,.0f, 360.0f);
                ImGui::SliderFloat3(("Scale_" + std::to_string(i)).c_str(), (float*) &currObj.transform.scale,0.0f, 1.0f);
            }
        }
        ImGui::End();
    }
    /* lights prop */
    if (show_Lights_window) {
        ImGui::Begin("Lights Properties", &show_Lights_window);
        ImGui::SliderInt("Used Lights", &lights_used, 1, 7);

        for(unsigned int i = 0; i <= 7; i++){
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

void mouseCallback(GLFWwindow *window, double x, double y) {
    if (!mouseControl) return;

    if (firstFrame) {
        lastX = x;
        lastY = y;
        firstFrame = false;
    }

    float xoffset = x - lastX;
    float yoffset = lastY - y;
    lastX = x;
    lastY = y;
    camera->updateFrontVec(xoffset, yoffset);
}

void processInput(GLFWwindow *window) {
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    pressDelay -= deltaTime;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS && (pressDelay < 0)) {
        if (mouseControl)
            std::cout << "mouse control is now enabled" << std::endl;
        else
            std::cout << "mouse control is now disabled" << std::endl;

        pressDelay = 0.5f;
        mouseControl = !mouseControl;
        if (mouseControl) glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        else glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

    if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS) {
        camera->switchToStatic(1);
    }
    if (glfwGetKey(window, GLFW_KEY_F2) == GLFW_PRESS) {
        camera->switchToStatic(2);
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera->forward(deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera->back(deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera->left(deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera->right(deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        camera->up(deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        camera->down(deltaTime);
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
    gameState_ptr = &gamestate;

    Shader cubeMapShader = *gamestate.shaders.find("cubemap")->second;
    Shader heightMapShader = *gamestate.shaders.find("height")->second;
    Shader fragLightShader = *gamestate.shaders.find("frag_light")->second;

    /* setup camera ---------------*/
    camera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f),
                        glm::vec3(0.0f, 0.0f, -1.0f),
                        glm::vec3(0.0f, 1.0f, 0.0f));

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
    cubemapPaths.push_back("../data/envmap_stormydays/stormydays_lf.tga");
    cubemapPaths.push_back("../data/envmap_stormydays/stormydays_rt.tga");
    cubemapPaths.push_back("../data/envmap_stormydays/stormydays_up.tga");
    cubemapPaths.push_back("../data/envmap_stormydays/stormydays_dn.tga");
    cubemapPaths.push_back("../data/envmap_stormydays/stormydays_ft.tga");
    cubemapPaths.push_back("../data/envmap_stormydays/stormydays_bk.tga");

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

    lights.push_back(new DirectionalLight(glm::vec3(0.1, 0.11, 0.12),
                                          glm::vec3(0.1, 0.13, 0.135),
                                          glm::vec3(0.05, 0.05, 0.05),
                                          glm::vec3(0.0, 0.1, 0.0)));
    for (int i = 0; i < 6; i++) {
        Light *pointLight = new PointLight(glm::vec3(0, 0, 0),
                                           glm::vec3(0.0, 0.0, 1.0),
                                           glm::vec3(0.024, 0.024, 1.0),
                                           glm::vec3(18.8, -5.6, -18.4),
                                           0.2f, 0.09f, 0.012f);
        lights.push_back(pointLight);
    }
    while (!glfwWindowShouldClose(window)) {
        PointLight* poin = (PointLight*)lights[1];
        poin->linear = (sin(glfwGetTime())+1)/10;

        processInput(window);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGuiIO &io = ImGui::GetIO();

        ImGuiDraw();
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w,
                     clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /* skybox rendering ---------------------------------*/
        glm::mat4 projectionMatrix1 = glm::perspective(glm::radians(45.0f),
                                                       io.DisplaySize.x / io.DisplaySize.y, 0.1f, 500.0f);
        glDepthMask(GL_FALSE);
        cubeMapShader.use();
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxID);
        cubeMapShader.setMat4fv("view", glm::mat4(glm::mat3(camera->getViewMatrix())));
        cubeMapShader.setMat4fv("projection", projectionMatrix1);
        cubeMapShader.setInt("cubemap", 0);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDepthMask(GL_TRUE);

        /* objects rendering */
        fragLightShader.use();

        glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f),
                                                      io.DisplaySize.x / io.DisplaySize.y, 0.1f, 500.0f);
        glm::mat4 cameraMatrix = camera->getViewMatrix();

        for (Object object : gamestate.objects) {
            object.shader->use();
            object.shader->setMat4fv("PVMmatrix", projectionMatrix * cameraMatrix * object.transform.getTransformMat());
            object.shader->setMat4fv("Model", object.transform.getTransformMat());
            object.shader->setMat4fv("NormalModel", glm::transpose(glm::inverse(object.transform.getTransformMat())));

            object.shader->setFloat("a",a);
            object.shader->setFloat("b",b);
            object.shader->setBool("normalTexUsed", false);
            object.shader->setVec3("cameraPosition", camera->getPos());
            object.shader->setFloat("material.shininess", 30.0f);
            object.shader->setInt("usedLights", lights_used);
            for(unsigned int i = 0; i < lights_used; i++){
                lights[i]->setLightParam(i, *object.shader);
            }
            object.model->Draw(*object.shader);
        }
        /* load height map */
        glm::mat4 modelMatrix = glm::mat4(1.0f);
        heightMapShader.use();
        heightMapShader.setFloat("a", a);
        heightMapShader.setFloat("b", b);
        heightMapShader.setVec3("cameraPosition", camera->getPos());
        heightMapShader.setFloat("material.shininess", 0.5f);
        heightMapShader.setInt("usedLights", lights_used);
        glUniform1f(glGetUniformLocation(heightMapShader.ID, "scale"), scale);
        glUniform1f(glGetUniformLocation(heightMapShader.ID, "half_scale"), half_scale);
        for(unsigned int i = 0; i < lights_used; i++){
            lights[i]->setLightParam(i, heightMapShader);
        }

        modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(300.0f, 300.0f, 300.0f));
        modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.5f, 0.0f, -0.5f));
        heightMapShader.setMat4fv("PVMmatrix", projectionMatrix * cameraMatrix * modelMatrix);
        heightMapShader.setMat4fv("Model", modelMatrix);
        heightMapShader.setBool("normalTexUsed", true);
        terrain.Draw(heightMapShader);
        CHECK_GL_ERROR();

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