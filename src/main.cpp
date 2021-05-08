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
#include "glm/gtc/type_ptr.hpp"

#include "stb_image.h"

static Camera *camera = NULL;
static float lightAngle = 0.0f;
static float lightHeight = 1.0f;
static float lightRadius = 1.0f;

static float lastX = 0.0f;
static float lastY = 0.0f;

static float deltaTime = 0.0f;
static float lastFrame = 0.0f;
static float pressDelay = -1.0f;
static bool firstFrame = true;
static bool mouseControl = false;

bool show_another_window = false;
ImVec4 clear_color = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);

static float xrotation = 270;
static float yrotation = 0;
static float zrotation = 0;
static int indices_offset = 0;


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
    ImGui::Begin(
            "Hello, world!");                          // Create a window called "Hello, world!" and append into it.

    ImGui::Text("use WASD to move around");
    ImGui::Text("use SPACE to fly up CTRL to fly downwards");
    ImGui::Text(
            "press \"Q\" to enable cursor and disable mouse control");  // Display some text (you can use a format strings too)
    ImGui::Checkbox("Camera Parameters", &show_another_window);

    ImGui::SliderFloat("light angle", &lightAngle, 0.0f,
                       360.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
    ImGui::ColorEdit3("clear color", (float *) &clear_color); // Edit 3 floats representing a color

//        if (ImGui::Button(
//                "Switch to camera 1"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
//            camera->switchToStatic(1);
//        ImGui::SameLine();
//        if (ImGui::Button(
//                "Switch to camera 2"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
//            camera->switchToStatic(2);
    for (unsigned int i = 0; i < 2; i++) {
        if (ImGui::Button(("Switch to camera " + std::to_string(i +
                                                                1)).c_str()))                            // Buttons return true when clicked (most widgets return true when edited/activated)
            camera->switchToStatic(i + 1);
    }

    ImGui::SliderFloat("portalx angle", &xrotation, 0.0f,
                       360.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
    ImGui::SliderFloat("portaly angle", &yrotation, 0.0f,
                       360.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
    ImGui::SliderFloat("portalz angle", &zrotation, 0.0f,
                       360.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
    ImGui::SliderFloat("light height", &lightHeight, 0,
                       20);            // Edit 1 float using a slider from 0.0f to 1.0f
    ImGui::SliderFloat("light radius", &lightRadius, 0,
                       20);            // Edit 1 float using a slider from 0.0f to 1.0f

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
                ImGui::GetIO().Framerate);
    ImGui::End();

    // 3. Show another simple window.
    if (show_another_window) {
        glm::vec3 cameraPos = camera->getPos();
        ImGui::Begin("Camera Parameters",
                     &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        if (ImGui::Button("Close"))
            show_another_window = false;
        ImGui::Text("Camera position is");
        ImGui::Text("x: %f", cameraPos.x);
        ImGui::SameLine();
        ImGui::Text("y: %f", cameraPos.y);
        ImGui::SameLine();
        ImGui::Text("z: %f", cameraPos.z);
        ImGui::Text("Yaw: %f   Pitch: %f", camera->getYaw(), camera->getPitch());
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
        if (mouseControl == true) glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        else glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
        camera->switchToStatic(1);
    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
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

    GLFWwindow *window = glfwCreateWindow(1920, 1080, "PGR_Semestral", NULL, NULL);
    if (window == NULL) {
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
    /* compile shader ---------------*/
    Shader dummyShader(
            "../shaders/dummy.vert",
            "../shaders/dummy.frag");

    Shader cubeMapShader = *gamestate.shaders.find("cubemap")->second;
    Shader heightMapShader = *gamestate.shaders.find("height")->second;
    Shader fragLightShader = *gamestate.shaders.find("frag_light")->second;

    /* setup camera ---------------*/
    camera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f),
                        glm::vec3(0.0f, 0.0f, -1.0f),
                        glm::vec3(0.0f, 1.0f, 0.0f));

    Model palm(
            "/home/matejs/Projects/School/PGR/PGR_semestral_linux/data/palm_1/palm_model/kkviz phoenix sylvestris_01.fbx");
    // Model portal("/home/matejs/Projects/School/PGR/PGR_semestral_linux/data/ancient_portal/ancient_portal_model/Ancient_portal_adjusted_1.fbx");

    float scale = 0.2;
    float half_scale = scale / 2.0f;

    std::string heightFileName = "../data/terrain_floor/displaced_floor/heightmap.tga";
    std::string normalFileName = "../data/terrain_floor/displaced_floor/normalmap.PNG";
    std::string diffuseFileName = "../data/terrain_floor/displaced_floor/colormap.png";

    Model terrain = prepareTerrainModel(300, heightFileName.c_str(), normalFileName.c_str(),
                                        diffuseFileName.c_str());
    heightMapShader.use();
    glUniform1f(glGetUniformLocation(heightMapShader.ID, "scale"), scale);
    glUniform1f(glGetUniformLocation(heightMapShader.ID, "half_scale"), half_scale);

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

    Light* directionalLight = new DirectionalLight(glm::vec3(0.05, 0.05,0.05),
                                      glm::vec3(0.4,0.4,0.4),
                                      glm::vec3(0.05,0.05,0.05),
                                      glm::vec3(-1,2,0));
    Light* pointLight = new PointLight(glm::vec3(0, 0,0),
                          glm::vec3(0.4,0.3,0.0),
                          glm::vec3(0.4,0.3,0.0),
                          glm::vec3(2,2,2),
                          1.0f, 0.09f, 0.032f);

    while (!glfwWindowShouldClose(window)) {

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
                                                       io.DisplaySize.x / io.DisplaySize.y, 0.1f, 100.0f);
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
        glm::vec3 lightPos = glm::vec3(lightRadius * glm::sin(glm::radians(lightAngle)), lightHeight,
                                       lightRadius * glm::cos(glm::radians(lightAngle)));
        glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f),
                                                      io.DisplaySize.x / io.DisplaySize.y, 0.1f, 100.0f);
        glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 0, 0));
        glm::mat4 cameraMatrix = camera->getViewMatrix();
        // modelMatrix = glm::rotate(modelMatrix,glm::radians(90.0f) ,glm::vec3(1.0f, 0.0f, 0.0f));

        modelMatrix = glm::rotate(modelMatrix, glm::radians(xrotation), glm::vec3(1.0f, 0.0f, 0.0f));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(yrotation), glm::vec3(0.0f, 1.0f, 0.0f));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(zrotation), glm::vec3(0.0f, 0.0f, 1.0f));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.05f, 0.05f, 0.05f));

        fragLightShader.setMat4fv("PVMmatrix", projectionMatrix * cameraMatrix * modelMatrix);
        fragLightShader.setMat4fv("Model", modelMatrix);
        fragLightShader.setMat4fv("NormalModel", glm::transpose(glm::inverse(modelMatrix)));

        fragLightShader.setBool("normalTexUsed", false);
        fragLightShader.setVec3("cameraPosition", camera->getPos());
        fragLightShader.setFloat("material.shininess", 30.0f);
        fragLightShader.setInt("usedLights", 2);
        directionalLight->setLightParam(0, fragLightShader);
        pointLight->setLightParam(1, fragLightShader);
//        fragLightShader.setInt("lights[0].type", 2);
//        fragLightShader.setInt("lights[1].type", 1);
//        fragLightShader.setInt("lights[2].type", 0);
//
//        fragLightShader.setVec3("lights[2].direction", lightPos);
//        fragLightShader.setVec3("lights[2].ambient", 0.05f, 0.05f, 0.05f);
//        fragLightShader.setVec3("lights[2].diffuse", 0.7f, 0.7f, 0.7f);
//        fragLightShader.setVec3("lights[2].specular", 0.3f, 0.3f, 0.3f);
//
//        fragLightShader.setVec3("lights[1].position", lightPos);
//        fragLightShader.setVec3("lights[1].ambient", 0.05f, 0.05f, 0.05f);
//        fragLightShader.setVec3("lights[1].diffuse", 0.4f, 0.1f, 0.1f);
//        fragLightShader.setVec3("lights[1].specular", 0.3f, 0.1f, 0.1f);
//        fragLightShader.setFloat("lights[1].constant", 1.0f);
//        fragLightShader.setFloat("lights[1].linear", 0.09f);
//        fragLightShader.setFloat("lights[1].quadratic", 0.032f);
//
//        fragLightShader.setVec3("lights[0].position", camera->getPos());
//        fragLightShader.setVec3("lights[0].direction", camera->getFront());
//        fragLightShader.setVec3("lights[0].ambient", 0.05f, 0.05f, 0.05f);
//        fragLightShader.setVec3("lights[0].diffuse", 0.1f, 0.4f, 0.1f);
//        fragLightShader.setVec3("lights[0].specular", 0.1f, 0.3f, 0.1f);
//        fragLightShader.setFloat("lights[0].constant", 1.0f);
//        fragLightShader.setFloat("lights[0].linear", 0.09f);
//        fragLightShader.setFloat("lights[0].quadratic", 0.032f);
//        fragLightShader.setFloat("lights[0].cutOff", glm::cos(glm::radians(12.5f)));
//        fragLightShader.setFloat("lights[0].outerCutOff", glm::cos(glm::radians(20.0f)));
        palm.Draw(fragLightShader);
        /* ---------------------------------------------------*/
        /* load height map */
//        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        heightMapShader.use();
        heightMapShader.setVec3("cameraPosition", camera->getPos());
        heightMapShader.setFloat("material.shininess", 0.5f);
        heightMapShader.setInt("usedLights", 2);
        directionalLight->setLightParam(0, heightMapShader);
        pointLight->setLightParam(1, heightMapShader);

        modelMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(100.0f, 100.0f, 100.0f));
        modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.5f, 0.0f, -0.5f));
        heightMapShader.setMat4fv("PVMmatrix", projectionMatrix * cameraMatrix * modelMatrix);
        heightMapShader.setMat4fv("Model", modelMatrix);
        heightMapShader.setVec3("lightPos", lightPos);
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