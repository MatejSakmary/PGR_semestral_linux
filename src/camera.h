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
    /* TODO: Add camera to node system so that this is not required*/
    Bezier* dynamicCurve;

public:
    /* defines portal position for collider checking */
    /* TODO: Add the option to check all objects not just this hardcoded one*/
    glm::vec3 portalPosition;
    /**
     * @param position initial position of camera
     * @param direction initial camera direction
     * @param m_up camera up vector
     * @param curve dynamic curve on which camera moves
     */
	Camera(glm::vec3 position, glm::vec3 direction, glm::vec3 m_up, Bezier* curve);
	/**
	 * Checks colliders
	 */
	void checkPosition();
	void forward(float deltaTime);
	void back(float deltaTime);
	void left(float deltaTime);
	void right(float deltaTime);
	void up(float deltaTime);
	void down(float deltaTime);
	/**
	 * Switch to one of two static views
	 * @param i 1 or 2 defines which static view I want camera to be in
	 */
	void switchToStatic(int i);
	/**
	 * Update camera direction based on mouse offsets
	 * @param xoffset mouse X offset from last frame
	 * @param yoffset mouse Y offset from last frame
	 */
	void updateFrontVec(float xoffset, float yoffset);
	/**
	 * Force camera to glide along predefined curve
	 */
	void switchToDynamic();
	glm::vec3 getPos();
    glm::vec3 getFront();
	float getYaw();
	float getPitch();
	glm::mat4 getViewMatrix(float time);
};
