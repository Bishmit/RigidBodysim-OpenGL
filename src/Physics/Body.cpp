#include "Body.h"
#include <iostream>

Body::Body(const Shape& shape, float x, float y): shape(shape.Clone()), position(Vec2(x,y)), isColliding(false){

}

Body::~Body(){
   delete shape; 
}

float Body::GetRadius(){
   CircleShape* circle = static_cast<CircleShape*>(shape); 
   if(circle) return circle->radius; 
   return 0.f; 
}

void Body::SetRadius(float &r){
   CircleShape* circle = static_cast<CircleShape*>(shape);
   if(circle) circle->radius = r;  
}

