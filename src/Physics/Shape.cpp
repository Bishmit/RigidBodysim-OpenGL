#include "Shape.h"
#include "Math/Vec2.h"
#include <iostream>
#include <limits>

float PolygonShape::Moi::density = 1.0f;

CircleShape::CircleShape(float radius):radius(radius){}

CircleShape::~CircleShape() {
}

Shape* CircleShape::Clone() const {
    return new CircleShape(radius);
}

void CircleShape::UpdateVertices(float angle, const Vec2& position) {
    return; // Circles don't have vertices... nothing to do here
}

ShapeType CircleShape::GetType() const {
    return CIRCLE;
}

float CircleShape::GetMomentOfInertia() const {
    return 0.5 * (radius * radius);
}

PolygonShape::PolygonShape(int sides, float radius):sides(sides), radius(radius){
  
    for (int i = 0; i < sides; i++) {
        float angle = (2.0f * M_PI * i) / sides;
        localVertices.push_back(Vec2(radius * cos(angle), radius * sin(angle)));
        worldVertices.push_back(Vec2(radius * cos(angle), radius * sin(angle)));
    }
}

PolygonShape::~PolygonShape() {
}

ShapeType PolygonShape::GetType() const {
    return POLYGON;
}


Shape* PolygonShape::Clone() const {
    return new PolygonShape(sides, radius);
}

float PolygonShape::GetMomentOfInertia() const {
    float area = Moi::PolygonArea(worldVertices);
    float mass = area * 0.005f; 
    
    float moi = Moi::CalculatePolygonMomentOfInertia(worldVertices, mass);
    return moi;
}


Vec2 PolygonShape::GetEdge(int index) const {
    int currVertex = index;
    int nextVertex = (index + 1) % worldVertices.size();
    return worldVertices[nextVertex] - worldVertices[currVertex];
}

Vec2 PolygonShape::GetNormal(int index) const
{
    Vec2 edge = GetEdge(index);
    return edge.Normal();
}


float PolygonShape::FindMinSeparation(const PolygonShape* other, Vec2& outAxis, Vec2& outPoint) const {
    float bestSeparation = -std::numeric_limits<float>::infinity();
    Vec2 bestAxis;
    Vec2 bestContactPoint;

    const size_t vertexCount = worldVertices.size();

    for (size_t i = 0; i < vertexCount; ++i) {
        Vec2 currentVertex = worldVertices[i];
        Vec2 edge = GetEdge(i);
        Vec2 normal = edge.Normal();

        float smallestProjection = std::numeric_limits<float>::infinity();
        Vec2 closestVertex;

        for (const Vec2& otherVertex : other->worldVertices) {
            float projection = (otherVertex - currentVertex).Dot(normal);
            if (projection < smallestProjection) {
                smallestProjection = projection;
                closestVertex = otherVertex;
            }
        }

        if (smallestProjection > bestSeparation) {
            bestSeparation = smallestProjection;
            bestAxis = edge;
            bestContactPoint = closestVertex;
        }
    }

    outAxis = bestAxis;
    outPoint = bestContactPoint;
    return bestSeparation;
}

void PolygonShape::UpdateVertices(float angle, const Vec2& position){
        for(int i = 0; i<localVertices.size(); i++){
        worldVertices[i] = localVertices[i].Rotate(angle); 
        worldVertices[i] += position; 
    }
}

float PolygonShape::Moi::PolygonArea(const std::vector<Vec2>& vertices){
     if (vertices.size() < 3) return 0.0f; 
    
    float area = 0.0f;
    for(int i = 0; i < vertices.size(); i++) {
        const Vec2& current = vertices[i];
        const Vec2& next = vertices[(i+1) % vertices.size()];
        area += current.x * next.y - next.x * current.y;
    }
    return std::abs(area) * 0.5f;
}

float PolygonShape::Moi::CalculateMass(const std:: vector<Vec2>& vertices, float density){
    float area = PolygonArea(vertices); 
    float mass = area * density; 
    return mass; 
}

float PolygonShape::Moi::CalculatePolygonMomentOfInertia(const std::vector<Vec2>& verts, float mass){
    float denominator = 0.f, numerator = 0.f; 

    for(int i = 0; i<verts.size(); i++){
        const Vec2& a = verts[i]; 
        const Vec2& b = verts[(i+1) % verts.size()]; 

        float cross = a.Cross(b);
        float term = (a.Dot(a) + a.Dot(b) + b.Dot(b));
        denominator += cross;
        numerator += cross * term;
    }
    float area = 0.5f * fabsf(denominator);
    return (mass / 6.0f) * (numerator / denominator);
}


BoxShape::BoxShape(float width, float height) {
    this->width = width; 
    this->height = height; 
    
    localVertices.push_back(Vec2(-width/2.0, -height/2.0));
    localVertices.push_back(Vec2(width/2.0, -height/2.0));  
    localVertices.push_back(Vec2(width/2.0, height/2.0));
    localVertices.push_back(Vec2(-width/2.0, height/2.0));
    
    worldVertices.push_back(Vec2(-width/2.0, -height/2.0));
    worldVertices.push_back(Vec2(width/2.0, -height/2.0));  
    worldVertices.push_back(Vec2(width/2.0, height/2.0));
    worldVertices.push_back(Vec2(-width/2.0, height/2.0));  
}

BoxShape::~BoxShape() {
   
}

ShapeType BoxShape::GetType() const {
    return BOX;
}

Shape* BoxShape::Clone() const {
    return new BoxShape(width, height);
}

float BoxShape::GetMomentOfInertia() const {
    return (0.083333) * (width * width + height * height);
}

