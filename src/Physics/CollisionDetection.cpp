#include "CollisionDetection.h"
#include "Physics/ContactInformation.h"

bool CollisionDetection::IsColliding(Body* a, Body* b, ContactInformation &contact){
    ShapeType aType = a->shape->GetType(); 
    ShapeType bType = b->shape->GetType(); 

    bool aIsCircle = aType == CIRCLE; 
    bool bIsCircle = bType == CIRCLE; 

    if(aIsCircle && bIsCircle){
        return IsCircleCircleColliding(a, b, contact); 
    }

    return false; 
}

bool CollisionDetection::IsCircleCircleColliding(Body* a, Body* b, ContactInformation &contact){
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
    
    contact.a = a; 
    contact.b = b; 
    contact.normal = ab; 
    contact.normal.Normalize(); 
    float abMag = ab.Magnitude();  // compute once

    contact.distance.a = abMag - b->GetRadius();
    contact.distance.b = abMag - a->GetRadius();
    
    contact.start = a->position + contact.normal * contact.distance.a;
    contact.end = b->position - contact.normal * contact.distance.b;

    contact.depth = (contact.end - contact.start).Magnitude();  
    return true; 
}
