#pragma once 

#include "Math/Vec2.h"
#include <vector>
#include <cmath>

enum ShapeType {
  CIRCLE,
  POLYGON,
  BOX
};

struct Shape {
  virtual ~Shape() = default;
  virtual ShapeType GetType() const = 0;
  virtual Shape* Clone() const = 0;
  virtual void UpdateVertices(float angle, const Vec2& position) = 0;
  virtual float GetMomentOfInertia() const = 0;
};

struct CircleShape: public Shape {
  float radius;

  CircleShape(const float radius);
  virtual ~CircleShape();
  ShapeType GetType() const override;
  Shape* Clone() const override;
  void UpdateVertices(float angle, const Vec2& position) override;
  float GetMomentOfInertia() const override;
};

struct PolygonShape: public Shape {
  int sides;
  float radius;
  std::vector<Vec2> localVertices; 
  std::vector<Vec2> worldVertices; 

    PolygonShape() = default;
    PolygonShape(const int sides, const float radius); 
    virtual ~PolygonShape();
    ShapeType GetType() const override;
    Shape* Clone() const override;
    Vec2 GetEdge(int index) const;
      Vec2 GetNormal(int index) const;
      float FindMinSeparation(const PolygonShape* other, Vec2& axis, Vec2& point) const;
    float GetMomentOfInertia() const override;
    
  void UpdateVertices(float angle, const Vec2& position) override; 

  struct Moi
  { 
    static float density; 
    static float PolygonArea(const std::vector<Vec2>& vertices);
    static float CalculateMass(const std::vector<Vec2>& vertices, float density);
    static float CalculatePolygonMomentOfInertia(const std::vector<Vec2>& verts, float mass); 
  };
  
};

struct BoxShape: public PolygonShape {
  float width;
  float height;

  BoxShape(float width, float height);
  virtual ~BoxShape();
  ShapeType GetType() const override;
  Shape* Clone() const override;
  float GetMomentOfInertia() const override;
};
