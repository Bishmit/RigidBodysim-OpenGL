#include "Body.h"
#include <iostream>

Body::Body(const Shape& shape, float x, float y, float mass, float rotation): shape(shape.Clone()), position(Vec2(x,y)), isColliding(false),
velocity(Vec2(0, 0)), acceleration(Vec2(0, 0)), mass(mass), restitution(1.0), gravity(10.0), friction(0.5), rotation(rotation), sumForces(Vec2(0, 0)), x(x), y(y)
{
   if (mass != 0.0) {
        this->invMass = 1.0 / mass;
    } else {
        this->invMass = 0.0;
    }
    I = shape.GetMomentOfInertia() * mass;
    if (I != 0.0) {
        this->invI = 1.0 / I;
    } else {
        this->invI = 0.0;
    }
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

void Body::ClearTorque() {
    sumTorque = 0.0;
}


bool Body::IsStatic() const{
    const float epsilon = 1e-6f; 
    return std::fabs(invMass) < epsilon;  
}

void Body::ApplyImpulse(const Vec2& j, const Vec2& contactVector){
    if (IsStatic()) return;  

    velocity += j * invMass;

    angularVelocity += invI * contactVector.Cross(j);
}

void Body::IntegrateLinear(float dt) {
    if(IsStatic()) return;
    acceleration = sumForces * invMass;
    velocity += acceleration * dt;
    position += velocity * dt;
    ClearForces();
}

void Body::IntegrateAngular(float dt) {
    if(IsStatic()) return; 
    angularAcceleration = sumTorque * invI;
    angularVelocity += angularAcceleration * dt;
    rotation += angularVelocity * dt;
    ClearTorque();
}

void Body::Update(float &deltatime){
   IntegrateLinear(deltatime);
   IntegrateAngular(deltatime);
        bool isPolygon = shape->GetType() == POLYGON || shape->GetType() == BOX; 
        if(isPolygon) {
            PolygonShape* polygonShape = (PolygonShape*) shape; 
            polygonShape->UpdateVertices(rotation, position); 
        }
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

//  std::vector<Vec2> Body::GeneratePolygon(){
//     PolygonShape* polyShape = static_cast<PolygonShape*>(shape); 
//     int sides = polyShape->sides; 
//     float radius = polyShape->radius; 
//    // auto polygonPoints = polyShape->polygonPoints; 
//    std::vector<Vec2> polygonPoints; 
   
//    for(int i = 0; i<sides; ++i){
//      float angleDeg = 360.f /sides * i ; 
//      float angleRad = glm::radians(angleDeg); 
//      float px = x + radius * std::cos(angleRad); 
//      float py = y + radius * std::sin(angleRad); 
//      polygonPoints.push_back({px, py}); 
//    }
//    return polygonPoints; 
// }