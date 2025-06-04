#include "Body.h"
#include <cmath>
#include <iostream>

Body::Body(const Shape& shape, float x, float y, float mass, float rotation): shape(shape.Clone()), position(Vec2(x, y)), velocity(Vec2(0, 0)),
      acceleration(Vec2(0, 0)), sumForces(Vec2(0, 0)), sumTorque(0.0), isColliding(false), mass(mass), restitution(1.0), gravity(10.0), friction(0.5), x(x), y(y), rotation(rotation), allowRotation(false)
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

Body::~Body() {
    delete shape;
}

void Body::AddForce(const Vec2& force) {
    sumForces += force;
}

void Body::AddTorque(float torque) {
    sumTorque += torque;
}

void Body::ClearForces() {
    sumForces = Vec2(0.0, 0.0);
}

void Body::ClearTorque() {
    sumTorque = 0.0;
}


void Body::ApplyImpulse(const Vec2 &j, const Vec2 &contactVector) {
    if (IsStatic()) return;  

    velocity += j * invMass;

    angularVelocity += invI * contactVector.Cross(j);
}

void Body::Update(const float &deltatime){
        IntegrateLinear(deltatime);
        IntegrateAngular(deltatime);
        bool isPolygon = shape->GetType() == POLYGON || shape->GetType() == BOX; 
        if(isPolygon) {
            PolygonShape* polygonShape = (PolygonShape*) shape; 
            polygonShape->UpdateVertices(rotation, position); 
        }
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
    
    if(allowRotation){
    angularAcceleration = sumTorque * invI;

    angularVelocity += angularAcceleration * dt;

    rotation += angularVelocity * dt;
   }
    ClearTorque();
}

bool Body::IsStatic() const{
    const float epsilon = 1e-6f; // typically a small value
    return std::fabs(invMass) < epsilon;  
}


void Body::IntegrateVelocities(const float dt) {
    if (IsStatic())
        return;
    position += velocity * dt;
    rotation += angularVelocity * dt;
    shape->UpdateVertices(rotation, position);
}

float Body::GetRadius(){
    CircleShape* circle = static_cast<CircleShape*>(shape);
    if(circle){
        return circle->radius; 
    }
    return 0.0f; 
}

void Body::SetRadius(float &r){
    CircleShape* circle = static_cast<CircleShape*>(shape);
    if(circle){
        circle->radius = r; 
    }
}

void Body::setSize(float w, float h) {
    BoxShape* boxShape = static_cast<BoxShape*> (shape); 
    boxShape->width = w; 
    boxShape->height = h;  
}

void Body::setRotation(float rot) {
    rot = rotation; 
}

// void Body::setStatic(bool isStatic){
//     bool _isStatic = IsStatic(); 
//     isStatic =  _isStatic; 
// }
