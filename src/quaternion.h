//
// Created by matejs on 14/05/2021.
//
// Heavily inspired by https://github.com/kromenak/gengine/blob/master/Source/Math/Quaternion.cpp

#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/constants.hpp"
#include <cmath>
#include <iostream>

class Quaternion {
public:
    float x{};
    float y{};
    float z{};
    float w{};
    Quaternion() : x{0.0f},y{0.0f},z{0.0f}, w{1.0f}{};
    Quaternion(glm::vec3 axis, float rotation) : x{axis.x}, y{axis.y}, z{axis.z}, w{rotation}{};
    Quaternion(float x, float y, float z, float w) : x{x}, y{y}, z{z}, w{w}{};
    Quaternion(float x, float y, float z);

    void setEuler(float x, float y, float z);
    Quaternion operator+(const Quaternion& second) const;
    Quaternion operator-(const Quaternion& second) const;

    friend Quaternion operator*(float scalar, const Quaternion& quaternion);
    Quaternion operator*(const Quaternion& second) const;

    glm::vec3 getEulerAngles() const;
    glm::vec3 Rotate(const glm::vec3& vector) const;
    glm::mat4 getRotMatrix();
    void Normalize();
    void Invert();

    static void Lerp(Quaternion& result, const Quaternion& start, const Quaternion& end, float t);
    static float Dot(const Quaternion& first, const Quaternion& second);
    static void Slerp(Quaternion& result, const Quaternion& start, const Quaternion& end, float t);


};


