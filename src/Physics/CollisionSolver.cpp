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
   auto a = contact.a; 
   auto b = contact.b; 
   auto end = contact.end; 
   auto start = contact.start; 
   auto normal = contact.normal; 

    float e = std::min(a->restitution, b->restitution);
    float f = std::min(a->friction, b->friction);

    // Calculate ra and rb, which are vectors from the center of mass of each body to the point of contact
    Vec2 ra = end - a->position;
    Vec2 rb = start - b->position;

    // Calculate velocities at the point of contact (including rotational effects)
    Vec2 va = a->velocity + Vec2(-a->angularVelocity * ra.y, a->angularVelocity * ra.x);
    Vec2 vb = b->velocity + Vec2(-b->angularVelocity * rb.y, b->angularVelocity * rb.x);
    const Vec2 vrel = va - vb;

    // Calculate the collision impulse along the normal direction
    float vrelDotNormal = vrel.Dot(normal);
    const Vec2 impulseDirectionNormal = normal;
    const float impulseMagnitudeNormal = -(1 + e) * vrelDotNormal / ((a->invMass + b->invMass) + ra.Cross(normal) * ra.Cross(normal) * a->invI + rb.Cross(normal) * rb.Cross(normal) * b->invI);
    Vec2 jN = impulseDirectionNormal * impulseMagnitudeNormal;

    // Calculate the collision impulse along the tangent direction (using friction coefficient f)
    Vec2 tangent = Vec2(normal.y, -normal.x);  // Tangent is perpendicular to the normal
    float vrelDotTangent = vrel.Dot(tangent);  
    const Vec2 impulseDirectionTangent = tangent;
    float impulseMagnitudeTangent = f * -vrelDotTangent / ((a->invMass + b->invMass) + ra.Cross(tangent) * ra.Cross(tangent) * a->invI + rb.Cross(tangent) * rb.Cross(tangent) * b->invI);

    Vec2 jT = impulseDirectionTangent * impulseMagnitudeTangent;

    // Combine normal and tangential impulses
    Vec2 j = jN + jT;

    // Apply the impulse to both objects in opposite directions
    a->ApplyImpulse(j, ra);
    b->ApplyImpulse(-j, rb);

}