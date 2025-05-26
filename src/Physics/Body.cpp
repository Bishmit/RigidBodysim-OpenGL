#include "Body.h"
#include <iostream>

Body::Body(const Shape& shape, float x, float y, float mass): shape(shape.Clone()), position(Vec2(x,y)), isColliding(false),
velocity(Vec2(0, 0)), acceleration(Vec2(0, 0)), mass(mass), restitution(1.0), gravity(10.0), friction(0.5), sumForces(Vec2(0, 0)), x(x), y(y)
{
   if (mass != 0.0) {
        this->invMass = 1.0 / mass;
    } else {
        this->invMass = 0.0;
    }

  // GeneratePolygon(); 
}

Body::~Body(){
   delete shape; 
}

void Body::AddForce(const Vec2& force) {
    sumForces += force;
}

void Body::ClearForces() {
     sumForces = Vec2(0.0, 0.0);
}

bool Body::IsStatic() const{
    const float epsilon = 1e-6f; // typically a small value
    return std::fabs(invMass) < epsilon;  
}

void Body::ApplyImpulse(const Vec2& j){
   if(IsStatic()) return; 

   velocity += j * invMass; 
}

void Body::IntegrateLinear(float dt) {
    if(IsStatic()) return;
    // Find the acceleration based on the forces that are being applied and the mass
    acceleration = sumForces * invMass;

    // Integrate the acceleration to find the new velocity
    velocity += acceleration * dt;

    // Integrate the velocity to find the new position
    position += velocity * dt;

    // Clear all the forces acting on the object before the next physics step
    ClearForces();
}

void Body::Update(float &deltatime){
   IntegrateLinear(deltatime);
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



/*======================POLYGON METHODS=========================*/

 std::vector<Vec2> Body::GeneratePolygon(){
    PolygonShape* polyShape = static_cast<PolygonShape*>(shape); 
    int sides = polyShape->sides; 
    float radius = polyShape->radius; 
   // auto polygonPoints = polyShape->polygonPoints; 
   std::vector<Vec2> polygonPoints; 
   
   for(int i = 0; i<sides; ++i){
     float angleDeg = 360.f /sides * i ; 
     float angleRad = glm::radians(angleDeg); 
     float px = x + radius * std::cos(angleRad); 
     float py = y + radius * std::sin(angleRad); 
     polygonPoints.push_back({px, py}); 
   }
   return polygonPoints; 
}