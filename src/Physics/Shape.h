#pragma once

#include "Math/Vec2.h"
#include <vector>

enum ShapeType {
    CIRCLE, POLYGON, BOX
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

class PolygonShape: public Shape{
     public:
     int sides;
     float radius;  
     PolygonShape() = default; 
     PolygonShape(const int sides, const float radius); 
     virtual ~PolygonShape();  
     ShapeType GetType() const override; 
     Shape* Clone() const override; 
};

class BoxShape: public PolygonShape {
  public: 
  float width;
  float height;

  BoxShape(float width, float height);
  virtual ~BoxShape();
  ShapeType GetType() const override;
  Shape* Clone() const override;
 // float GetMomentOfInertia() const override;
};