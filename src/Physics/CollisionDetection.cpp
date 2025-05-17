#include "CollisionDetection.h"

bool CollisionDetection::IsColliding(Body* a, Body* b){
    ShapeType aType = a->shape->GetType(); 
    ShapeType bType = b->shape->GetType(); 

    bool aIsCircle = aType == CIRCLE; 
    bool bIsCircle = bType == CIRCLE; 

    if(aIsCircle && bIsCircle){
        return IsCircleCircleColliding(a, b); 
    }

    return false; 
}

bool CollisionDetection::IsCircleCircleColliding(Body* a, Body* b){
    CircleShape* aCircleShape = static_cast<CircleShape*> (a->shape); 
    CircleShape* bCircleShape = static_cast<CircleShape*> (b->shape); 

    const Vec2 ab = b->position - a->position; 
    const float radii = a->GetRadius() + b->GetRadius();
    auto radiiSquared = radii * radii;  

    bool isColliding = false; 
    if(ab.MagnitudeSquared() <= radiiSquared){
        isColliding = true; 
    }

    if(!isColliding) return false; 

    return true; 
}