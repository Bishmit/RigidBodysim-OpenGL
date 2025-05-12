#pragma once

#include "glm/glm.hpp"
#include "Shape.h" 

class Body{
  public: 
  glm::vec2 position;  
  
  Shape* shape = nullptr; 
  
  Body(const Shape& shape, float x, float y); 
  ~Body(); 

  float GetRadius(); 
  void SetRadius(float &r); 
}; 