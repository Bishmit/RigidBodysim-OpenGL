#include "Shape.h"  
#include <iostream>

CircleShape::CircleShape(float radius):radius(radius){}

CircleShape::~CircleShape(){}

Shape* CircleShape::Clone() const{
    return new CircleShape(radius); // this is basically returning you the new instance of the CircleShape  
}

ShapeType CircleShape::GetType() const {
    return CIRCLE;
}

float CircleShape::GetMomentOfInertia() const {
    return 0.5 * (radius * radius);
}


PolygonShape::PolygonShape(int sides, float radius):sides(sides), radius(radius){
     localVertices.reserve(sides);
     worldVertices.reserve(sides);
    
    for (int i = 0; i < sides; i++) {
        float angle = (2.0f * M_PI * i) / sides;
        localVertices.push_back(Vec2(radius * cos(angle), radius * sin(angle)));
        worldVertices.push_back(Vec2(radius * cos(angle), radius * sin(angle)));
    }
}

PolygonShape::~PolygonShape(){}

ShapeType PolygonShape::GetType() const {
    return POLYGON;
}

Shape* PolygonShape::Clone() const {
    return new PolygonShape(sides, radius);
}

void PolygonShape::UpdateVertices(float angle, const Vec2& position) {
    for (int i = 0; i < localVertices.size(); i++) {
        // First rotate, then translate 
        worldVertices[i] = localVertices[i].Rotate(angle); 
        worldVertices[i] += position; 
    }
}

float PolygonShape::GetMomentOfInertia() const {
    return 0.0f; 
}

BoxShape::BoxShape(float width, float height):width(width), height(height)  {
    localVertices.reserve(4);
    worldVertices.reserve(4);
    
    localVertices.push_back(Vec2(-width/2.0f, -height/2.0f)); // top-left
    localVertices.push_back(Vec2(width/2.0f, -height/2.0f));  // top-right
    localVertices.push_back(Vec2(width/2.0f, height/2.0f));   // bottom-right
    localVertices.push_back(Vec2(-width/2.0f, height/2.0f));  // bottom-left
    
    worldVertices.push_back(Vec2(-width/2.0f, -height/2.0f));
    worldVertices.push_back(Vec2(width/2.0f, -height/2.0f));  
    worldVertices.push_back(Vec2(width/2.0f, height/2.0f));
    worldVertices.push_back(Vec2(-width/2.0f, height/2.0f));  
}

BoxShape::~BoxShape() {}

ShapeType BoxShape::GetType() const {
    return BOX;
}

Shape* BoxShape::Clone() const {
    return new BoxShape(width, height);
}

float BoxShape::GetMomentOfInertia() const {
    return 0.0f; 
}