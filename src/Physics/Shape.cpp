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

PolygonShape::PolygonShape(int sides, float radius):sides(sides), radius(radius){}

PolygonShape::~PolygonShape(){}
ShapeType PolygonShape::GetType() const {
    return POLYGON;
}

Shape* PolygonShape::Clone() const {
    return new PolygonShape(sides, radius);
}

BoxShape::BoxShape(float width, float height):width(width), height(height)  {}

BoxShape::~BoxShape() {}

ShapeType BoxShape::GetType() const {
    return BOX;
}

Shape* BoxShape::Clone() const {
    return new BoxShape(width, height);
}