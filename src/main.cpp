#include <iostream>

#include <glad/glad.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <GLFW/glfw3.h>
#include "shader.h"
#include "camera.h"
#include "model.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"


static Camera* camera = NULL;
static float lightAngle = 0.0f;

static float lastX = 0.0f;
static float lastY = 0.0f;

static float deltaTime = 0.0f;
static float lastFrame = 0.0f;
static float pressDelay = -1.0f;
static bool firstFrame = true;
static bool mouseControl = false;

bool show_another_window = false;
ImVec4 clear_color = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);

static float xrotation = 0;
static float yrotation = 0;
static float zrotation = 0;

void setupTriangle(unsigned int &VBO, unsigned int &VAO)
{

    float vertices[] = {
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
     0.0f,  0.5f, 0.0f
    };
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*) 0);
    glEnableVertexAttribArray(0);
}

void ImGuiDraw()
{
	{
		static int counter = 0;

		ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

		ImGui::Text("use WASD to move around");
		ImGui::Text("use SPACE to fly up CTRL to fly downwards");
		ImGui::Text("press \"Q\" to enable cursor and disable mouse control");  // Display some text (you can use a format strings too)
		ImGui::Checkbox("Camera Parameters", &show_another_window);

		ImGui::SliderFloat("light angle", &lightAngle, 0.0f, 360.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

		if (ImGui::Button("Switch to camera 1"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			camera->switchToStatic(1);
		ImGui::SameLine();
		if (ImGui::Button("Switch to camera 2"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			camera->switchToStatic(2);

		ImGui::SliderFloat("portalx angle", &xrotation, 0.0f, 360.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::SliderFloat("portaly angle", &yrotation, 0.0f, 360.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::SliderFloat("portalz angle", &zrotation, 0.0f, 360.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}

	// 3. Show another simple window.
	if (show_another_window)
	{
		glm::vec3 cameraPos = camera->getPos();
		ImGui::Begin("Camera Parameters", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
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

void mouseCallback(GLFWwindow* window, double x, double y)
{
    if(!mouseControl) return;

    if(firstFrame)
    {
        lastX = x;
        lastY = y;
        firstFrame = false;
    }

    float xoffset = x - lastX;
    float yoffset = lastY - y;
    lastX = x;
    lastY = y;
    camera->updateFrontVec(xoffset,yoffset);
}

void processInput(GLFWwindow* window)
{
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    pressDelay -= deltaTime;

    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS && (pressDelay < 0))
	{
        if (mouseControl)
            std::cout << "mouse control is now enabled" << std::endl;
        else
            std::cout << "mouse control is now disabled" << std::endl;

		pressDelay = 0.5f;
		mouseControl = !mouseControl;
		if(mouseControl == true) glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
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

int main()
{
    const char *glsl_version = "#version 130";

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(1920, 1080, "PGR_Semestral", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetCursorPosCallback(window, mouseCallback);


    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glEnable(GL_DEPTH_TEST);

    /* setup imgui context ---------------*/
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    /* prepare simple triangle ---------------*/
    unsigned int VBO,VAO;
    setupTriangle(VBO, VAO);

    /* compile shader ---------------*/
    Shader dummyShader(
        "/home/matejs/Projects/School/PGR/PGR_semestral_linux/shaders/dummy.vert",
        "/home/matejs/Projects/School/PGR/PGR_semestral_linux/shaders/dummy.frag");
    
    Shader fragLightShader(
        "/home/matejs/Projects/School/PGR/PGR_semestral_linux/shaders/fragment_light.vert",
        "/home/matejs/Projects/School/PGR/PGR_semestral_linux/shaders/fragment_light.frag");

    /* setup camera ---------------*/
	camera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f),
				        glm::vec3(0.0f, 0.0f, -1.0f),
				        glm::vec3(0.0f, 1.0f, 0.0f));

    // Model rock("/home/matejs/Projects/School/PGR/PGR_semestral_linux/data/palm_1/palm_model/kkviz phoenix sylvestris_01.fbx");
    Model portal("/home/matejs/Projects/School/PGR/PGR_semestral_linux/data/ancient_portal/ancient_portal_model/Ancient_portal_adjusted_1.fbx");

    Vertex vertex;
    vertex.Position =glm::vec3(-0.5f, -0.5f, 0.3f);
    vertex.TexCoords = glm::vec2(0,0);
    vertex.Normal = glm::vec3(0,0,0);
    std::vector<Vertex> Triangle;
    Triangle.push_back(vertex);
    vertex.Position =glm::vec3(0.5f, -0.5f, 0.0f);
    Triangle.push_back(vertex);
    vertex.Position =glm::vec3(0.0f, 0.5f, 0.0f);
    Triangle.push_back(vertex);

    std::vector<unsigned int> indices{0,1,2};
    std::vector<Texture> textures;

    Mesh testMesh(Triangle, indices, textures);
    while (!glfwWindowShouldClose(window))
    {

        processInput(window);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGuiIO& io = ImGui::GetIO();

        ImGuiDraw();
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /* objects rendering */
        fragLightShader.use();
        glm::vec3 lightPos = glm::vec3(20.0f * glm::sin(glm::radians(lightAngle)),20.0f, 
                                       20.0f * glm::cos(glm::radians(lightAngle)));
        glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f), io.DisplaySize.x / io.DisplaySize.y,0.1f, 100.0f);
        glm::mat4 modelMatrix      = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 0, 0));
        glm::mat4 cameraMatrix     = camera->getViewMatrix();
        // modelMatrix = glm::rotate(modelMatrix,glm::radians(90.0f) ,glm::vec3(1.0f, 0.0f, 0.0f));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(xrotation), glm::vec3(1.0f, 0.0f, 0.0f));
	    modelMatrix = glm::rotate(modelMatrix, glm::radians(yrotation), glm::vec3(0.0f, 1.0f, 0.0f));
	    modelMatrix = glm::rotate(modelMatrix, glm::radians(zrotation), glm::vec3(0.0f, 0.0f, 1.0f));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f, 0.5f, 0.5f));
        fragLightShader.setMat4fv("PVMmatrix", projectionMatrix * cameraMatrix * modelMatrix);
        fragLightShader.setMat4fv("Model", modelMatrix);
        fragLightShader.setMat4fv("NormalModel", glm::transpose(glm::inverse(modelMatrix)));
        fragLightShader.setVec3("lightPos", lightPos);
        portal.Draw(fragLightShader);
        /* ---------------------------------------------------*/

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