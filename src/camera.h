#pragma once

#include <iostream>
#include "bezier.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

class Camera
{
private:
	glm::vec3 m_position;
	glm::vec3 m_front;
	glm::vec3 m_up;
	float pitch;
	float yaw;
	float m_speed;
	float sensitivity;
    bool inDynamic;
    Bezier* dynamicCurve;

public:
	Camera(glm::vec3 position, glm::vec3 direction, glm::vec3 m_up, Bezier* curve);
	void forward(float deltaTime);
	void back(float deltaTime);
	void left(float deltaTime);
	void right(float deltaTime);
	void up(float deltaTime);
	void down(float deltaTime);
	void switchToStatic(int i);
	void updateFrontVec(float xoffset, float yoffset);
	void switchToDynamic();
	glm::vec3 getPos();
    glm::vec3 getFront();
	float getYaw();
	float getPitch();
	glm::mat4 getViewMatrix(float time);
};
