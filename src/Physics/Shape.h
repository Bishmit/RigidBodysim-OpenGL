#pragma once

#include "glm/glm.hpp"
#include <vector>

enum ShapeType {
    CIRCLE, POLYGON
}; 

class Shape {
    public: 
    virtual ~Shape() = default; 
    virtual ShapeType GetType() const = 0; 
    virtual Shape* Clone() const = 0;
}; 

class CircleShape: public Shape{
    public:
    float radius;  
    CircleShape(const float radius); 
    virtual ~CircleShape(); 
    ShapeType GetType() const override; 
    Shape* Clone() const override; 
}; 