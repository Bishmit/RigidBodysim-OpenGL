#include "CollisionSolver.h"

void CollisionSolver::ResolveOverlap(ContactInformation &contact){
    
    if(contact.a->IsStatic() && contact.b->IsStatic()) return; 

    float totalInverseMass = contact.a->invMass + contact.b->invMass; 

    if (totalInverseMass == 0.0f) return;
    
    float positionCorrectionA =  (contact.depth * contact.a->invMass) / totalInverseMass; 
    float positionCorrectionB =  (contact.depth * contact.b->invMass) / totalInverseMass; 

    contact.a->position -= contact.normal * positionCorrectionA;
    contact.b->position += contact.normal * positionCorrectionB; 
}

void CollisionSolver::ResolveCollision(ContactInformation &contact){
    ResolveOverlap(contact); 

    float e = std::min(contact.a->restitution, contact.b->restitution); 

    // Calculate the relative velocity 
    const Vec2 vrel = (contact.a->velocity - contact.b->velocity); 

    //Calculate the relative velocity along the normal collison
    float vrelDotNormal = vrel.Dot(contact.normal); 

     //if (vrelDotNormal > 0) return;

    // Main collision impulse formula 
    const Vec2 impulseDirection = contact.normal; 
    const float impulseMagnitude = -(1 + e) * vrelDotNormal / (contact.a->invMass + contact.b->invMass);

    Vec2 jn = impulseDirection * impulseMagnitude;

    // Apply the impulse vector to both objects in opposite direction 
    contact.a->ApplyImpulse(jn); 
    contact.b->ApplyImpulse(-jn);  

}