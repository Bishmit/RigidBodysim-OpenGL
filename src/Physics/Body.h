#pragma once

#include "glm/glm.hpp"
#include "Math/Vec2.h"
#include "Shape.h"

struct Body {
  // Linear motion
  Vec2 position;
  Vec2 velocity;
  Vec2 acceleration;

  // Angular motion
  float rotation;
  float angularVelocity;
  float angularAcceleration;
  
  // Forces and torque
  Vec2 sumForces;
  float sumTorque;

  // Restitution of the body
  float restitution; 

  // Mass and Moment of Inertia
  float mass;
  float gravity; 
  float invMass;
  float I;
  float invI;
  float friction; 
  bool isColliding;

  float x, y; 

  // Pointer to the shape/geometry of this rigid body
  Shape* shape = nullptr;

  Body(const Shape& shape, float x, float y, float mass, float rotation);
  ~Body();
  
  bool IsStatic() const; 
  void AddForce(const Vec2& force);
  void AddTorque(float torque);
  void ClearForces();
  void ClearTorque(); 
  void ApplyImpulse(const Vec2& ji, const Vec2& contactVector); 
  void Update(const float &deltatime); 
  float GetRadius(); 
  void  SetRadius(float &radius);

  void IntegrateLinear(float dt);
  void IntegrateAngular(float dt);
  void IntegrateVelocities(const float dt);
};
