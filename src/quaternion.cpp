//
// Created by matejs on 14/05/2021.
//

#include "quaternion.h"

Quaternion::Quaternion(float x, float y, float z) {
    setEuler(x, y, z);
}

Quaternion Quaternion::operator+(const Quaternion &second) const {
    return {glm::vec3(x + second.x, y + second.y,z + second.z), w + second.w };
}

Quaternion Quaternion::operator-(const Quaternion &second) const {
    return {glm::vec3(x - second.x, y - second.y,z - second.z), w - second.w };
}

Quaternion Quaternion::operator*(const Quaternion &second) const {
    return {w * second.x + x * second.w + y * second.z - z * second.y,
                      w * second.y + y * second.w + z * second.x - x * second.z,
                      w * second.z + z * second.w + x * second.y - y * second.x,
                      w * second.w - x * second.x - y * second.y - z * second.z};
}

glm::vec3 Quaternion::Rotate(const glm::vec3 &vector) const {
    float vMult = 2.0f * (x * vector.x + y * vector.y + z * vector.z);
    float crossMult = 2.0f * w;
    float pMult = crossMult * w - 1.0f;

    return glm::vec3(pMult * vector.x + vMult * x + crossMult * (y * vector.z - z * vector.y),
                   pMult * vector.y + vMult * y + crossMult * (z * vector.x - x * vector.z),
                   pMult * vector.z + vMult * z + crossMult * (x * vector.y - y * vector.x));
}

void Quaternion::Normalize() {
    float length = glm::sqrt((x*x) + (y*y) + (z*z) + (w*w));
    if(length == 0){
        x = y = z = w = 0;
    }else{
        x /= length;
        y /= length;
        z /= length;
        w /= length;
    }
}

void Quaternion::Invert() {
    x *= -1.0f;
    y *= -1.0f;
    z *= -1.0f;
}

void Quaternion::Lerp(Quaternion &result, const Quaternion &start, const Quaternion &end, float t) {
    // Get cos of angle between quaternions.
    float cosTheta = Quaternion::Dot(start, end);

    // Set result to end by default.
    result = t * end;

    // If angle between quaternions is less than 90 degrees, use standard interpolation.
    // Otherwise, take the other path.
    if(cosTheta == 0)
    {
        result = result + (1.0f - t) * start;
    }
    else
    {
        result = result + (t - 1.0f) * start;
    }
}

Quaternion operator*(float scalar, const Quaternion &quaternion) {
    return {scalar * quaternion.x, scalar*quaternion.y, scalar*quaternion.z, scalar*quaternion.w};
}

glm::vec3 Quaternion::getEulerAngles() const {
    glm::vec3 result;
    float sinr_cosp = 2 * (w * x + y * z);
    float cosr_cosp = 1 - (2 * (x * x + y * y));
    result.x = std::atan2(sinr_cosp, cosr_cosp);

    // Y-axis
    float sinp = 2 * (w * y - z * x);
    if(std::abs(sinp) >= 1)
    {
        result.y = std::copysign(glm::pi<float>() / 2, sinp);
    }
    else
    {
        result.y = std::asin(sinp);
    }

    // Z-axis
    float siny_cosp = 2 * (w * z + x * y);
    float cosy_cosp = 1 - (2 * (y * y + z * z));
    result.z = std::atan2(siny_cosp, cosy_cosp);

    return result;
}

float Quaternion::Dot(const Quaternion &first, const Quaternion &second) {
    return(first.x * second.x + first.y * second.y +first.z * second.z +first.w * second.w);
}

void Quaternion::Slerp(Quaternion &result, const Quaternion &start, const Quaternion &end, float t) {
    float cosTheta = Quaternion::Dot(start, end);
    float startInterp, endInterp;

    // If angle between quaternions is less than 90 degrees...
    if(cosTheta >= 0)
    {
        // If angle is greater than zero, use standard slerp.
        if((1.0f - cosTheta) > 0)
        {
            float theta = std::acos(cosTheta);
            float recipSinTheta = 1.0f / std::sin(theta);

            startInterp = std::sin((1.0f - t) * theta) * recipSinTheta;
            endInterp = std::sin(t * theta) * recipSinTheta;
        }
            // Angle is close to zero, so use linear interpolation.
        else
        {
            startInterp = 1.0f - t;
            endInterp = t;
        }
    }
        // Otherwise, take the shorter route.
    else
    {
        // If angle is less than 180 degrees...
        if((1.0f + cosTheta) > 0)
        {
            // ...use slerp w/ negation of start quaternion.
            float theta = std::acos(-cosTheta);
            float recipSinTheta = 1.0f / std::sin(theta);

            startInterp = std::sin((t - 1.0f) * theta) * recipSinTheta;
            endInterp = std::sin(t * theta) * recipSinTheta;
        }
            // Or angle is close to 180 degrees, so use lerp w/ negated start quat.
        else
        {
            startInterp = t - 1.0f;
            endInterp = t;
        }
    }
    result = startInterp * start + endInterp * end;
}

glm::mat4 Quaternion::getRotMatrix() {
    float s = 2.0f / Quaternion::Dot(*this, *this);

    float xs = s * this->x;
    float ys = s * this->y;
    float zs = s * this->z;
    float wx = this->w * xs;
    float wy = this->w * ys;
    float wz = this->w * zs;
    float xx = this->x * xs;
    float xy = this->x * ys;
    float xz = this->x * zs;
    float yy = this->y * ys;
    float yz = this->y * zs;
    float zz = this->z * zs;

    auto* rot = new glm::mat4(1.0f);
    (*rot)[0][0] = 1.0f - (yy + zz);
    (*rot)[0][1] = xy + wz;
    (*rot)[0][2] = xz - wy;

    (*rot)[1][0] = xy - wz;
    (*rot)[1][1] = 1.0f - (xx + zz);
    (*rot)[1][2] = yz + wx;

    (*rot)[2][0] = xz + wy;
    (*rot)[2][1] = yz - wx;
    (*rot)[2][2] = 1.0f - (xx + yy);

    return *rot;
}

void Quaternion::setEuler(float x_, float y_, float z_) {
    float xRadians = glm::radians(x_) * 0.5;
    float yRadians = glm::radians(y_) * 0.5;
    float zRadians = glm::radians(z_) * 0.5;

    float sinX = glm::sin(xRadians);
    float cosX = glm::cos(xRadians);

    float sinY = glm::sin(yRadians);
    float cosY = glm::cos(yRadians);

    float sinZ = glm::sin(zRadians);
    float cosZ = glm::cos(zRadians);

    this->w = cosX * cosY * cosZ - sinX * sinY * sinZ;
    this->x = sinX * cosY * cosZ + cosX * sinY * sinZ;
    this->y = cosX * sinY * cosZ - sinX * cosY * sinZ;
    this->z = cosX * cosY * sinZ + sinZ * sinY * cosX;
}
