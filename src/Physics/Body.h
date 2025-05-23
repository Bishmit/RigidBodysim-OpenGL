#pragma once

#include "glm/glm.hpp"
#include "Shape.h"
#include "Math/Vec2.h" 

class Body{
  public: 
  Vec2 position;  
  
  Shape* shape = nullptr; 
  
  bool isColliding = false; 
  Body(const Shape& shape, float x, float y); 
  ~Body(); 

  float GetRadius(); 
  void SetRadius(float &r); 
}; 