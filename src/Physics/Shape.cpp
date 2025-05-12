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

