#pragma once 
#include "Math/Vec2.h"
#include "glm/glm.hpp"
#define PI 3.14159

struct WreckingBall {
    float mass = 1.f;
    float length = 100.f; 
    float angle = PI / 4.f;
    float angleVelocity = 0.f;
    float angleAcceleration = 0.f;
    float speedMultiplier = 15.0f; 

    Vec2 SolvePendulum(float gravity, Vec2 origin, Vec2 bobPos, float deltatime) {
        float dx = bobPos.x - origin.x;
        float dy = bobPos.y - origin.y;
        float currentLength = glm::sqrt(dx * dx + dy * dy);
        
        // updating angle from current position only if length changed significantly
        if (abs(currentLength - length) > 0.1f) {
            length = currentLength;
            angle = atan2(dx, dy);
        }
        float adjustedDeltaTime = deltatime * speedMultiplier;

        float force = mass * gravity * sin(angle);
        angleAcceleration = (-1 * force) / length;
        angleVelocity += angleAcceleration * adjustedDeltaTime;
        angle += angleVelocity * adjustedDeltaTime;

        bobPos.x = length * sin(angle) + origin.x;
        bobPos.y = length * cos(angle) + origin.y;

        angleVelocity *= 0.99f; // damping

        return bobPos;
    }
};

