#include "camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 direction, glm::vec3 up, Bezier* curve) {
    this->dynamicCurve = curve;
	this->m_position = position;
	this->m_front = direction;
	this->m_up = up;
	this->m_speed = 6.0f;
	this->yaw = -90.0f;
	this->pitch = 0.0f;
	this->sensitivity = 0.08f;
	this->inDynamic = false;
}

glm::mat4 Camera::getViewMatrix(float t = 0) {
	glm::vec3 direction;

	if(inDynamic){
	    m_position = dynamicCurve->getPosition(t);
	    direction = glm::vec3(20.0f, -15.5f, -16.6f) - m_position;
	}else {
        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction.y = sin(glm::radians(pitch));
        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	}
	m_front = glm::normalize(direction);

	return glm::lookAt(m_position, m_position + m_front, m_up);
}
    
void Camera::forward(float deltaTime) {
    inDynamic = false;
	this->m_position += m_front * m_speed * deltaTime;
    checkPosition();
}
void Camera::back(float deltaTime) {
    inDynamic = false;
	this->m_position -= m_front * m_speed * deltaTime;
    checkPosition();
}
void Camera::left(float deltaTime) {
    inDynamic = false;
	this->m_position -= glm::normalize(glm::cross(m_front, m_up)) * m_speed * deltaTime;
    checkPosition();
}
void Camera::right(float deltaTime) {
    inDynamic = false;
	this->m_position += glm::normalize(glm::cross(m_front, m_up)) * m_speed * deltaTime;
    checkPosition();
}
void Camera::updateFrontVec(float xoffset, float yoffset) {
	yaw += sensitivity * xoffset;
	pitch += sensitivity * yoffset;
	// make sure the camera is not flipping, lock the top and bottom view angle so that
	// we can look up to the sky or to the ground but not further
	if (pitch > 89.0f) pitch = 89.0f;
	if (pitch < -89.0f) pitch = -89.0f;
}
void Camera::up(float deltaTime)
{
	this->m_position += m_up * m_speed * deltaTime;
    checkPosition();
}

void Camera::down(float deltaTime)
{
	this->m_position -= m_up * m_speed * deltaTime;
    checkPosition();
}

void Camera::switchToStatic(int i) {
	switch (i)
	{
	case 1:
		this->m_position = glm::vec3(0.0f, 200.0f, 0.0f);
		this->pitch = -89.0f;
		this->yaw = -90.0f;
		inDynamic = false;
		break;
	case 2:
		this->m_position = glm::vec3(-92.5f, 10.3f, -29.8f);
		this->pitch = -4.16f;
		this->yaw = 1.57f;
		inDynamic = false;
		break;
	default:
		break;
	}
}

glm::vec3 Camera::getPos() {
	return m_position;
}
float  Camera::getPitch() {
	return pitch;
}
float Camera::getYaw() {
	return yaw;
}

glm::vec3 Camera::getFront() {
    return this->m_front;
}

void Camera::switchToDynamic() {
    inDynamic = true;
}

void Camera::checkPosition() {
    /* If the direction to portal is less than 30 don't move closer*/
    if(glm::abs(glm::distance(portalPosition, m_position)) < 30){
        /*Get the vector pointing from portal origin to camera and set camera position
         * to be portalOrigin + this vector -> always moves camera to corresponding position
         * on the collider sphere*/
        glm::vec3 portalToCamVec = glm::normalize(m_position - portalPosition);
        m_position = portalPosition + (30.0f * portalToCamVec);
    }
    /*Check map bounds-> if camera tries to go outside of the map snap it back in*/
    if(m_position.x > 100 ){
        m_position.x = 100;
    }
    if(m_position.x < -100 ){
        m_position.x = -100;
    }
    if(m_position.y > 50){
        m_position.y = 50;
    }
    if(m_position.y < -5 ){
        m_position.y = -5;
    }
    if(m_position.z > 100 ){
        m_position.z = 100;
    }
    if(m_position.z < -100 ){
        m_position.z = -100;
    }
}
