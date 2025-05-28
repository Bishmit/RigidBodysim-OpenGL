#pragma once

#include "glm/glm.hpp"
#include "Shape.h"
#include "Math/Vec2.h"
#include <vector> 

class Body{
  public: 
  Vec2 position; 
  Vec2 velocity;
  Vec2 acceleration; 

  Vec2 sumForces;
  float sumTorque;

  // Restitution of the body
  float restitution; 
  
  //Angular motion
  float rotation;
  float angularVelocity;
  float angularAcceleration;

  // Mass and Moment of Inertia
  float mass;
  float gravity; 
  float invMass;
  float I;
  float invI;
  float friction; 
  bool isColliding;

  float x, y; 
  
  Shape* shape = nullptr; 
  
  Body(const Shape& shape, float x, float y, float mass,  float rotation); 
  ~Body(); 

  // functions 
  void AddForce(const Vec2& force);
  void AddTorque(float torque);
  void ClearForces(); 
  void ClearTorque();
  void Update(float &deltatime); 
  void IntegrateLinear(float dt);
  void IntegrateAngular(float dt); 
  bool IsStatic() const; 
  void ApplyImpulse(const Vec2& ji, const Vec2& contactVector);  
  float GetRadius(); 
  void SetRadius(float &r);

  //std::vector<Vec2> GeneratePolygon(); 
}; 