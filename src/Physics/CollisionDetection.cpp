#include "CollisionDetection.h"
#include "Shape.h"

bool CollisionDetection::isColliding(Body* a, Body* b, ContactInformation &contact){
   bool aIsCircle = a->shape->GetType() == CIRCLE;
    bool bIsCircle = b->shape->GetType() == CIRCLE;
    bool aIsPolygon = a->shape->GetType() == POLYGON || a->shape->GetType() == BOX;
    bool bIsPolygon = b->shape->GetType() == POLYGON || b->shape->GetType() == BOX;

    if (aIsCircle && bIsCircle) {
        return isCircleCircleColliding(a, b, contact);
    }
    if (aIsPolygon && bIsPolygon) {
        return IsCollidingPolygonPolygon(a, b, contact);
    }

    if (aIsPolygon && bIsCircle) {
        return IsCollidingPolygonCircle(a, b, contact);
    }

    if (aIsCircle && bIsPolygon) {
        return IsCollidingPolygonCircle(b, a, contact);
    }
    return false;
}

bool CollisionDetection::IsCollidingPolygonCircle(Body* a, Body* b, ContactInformation& contact) {
    const PolygonShape* polygonShape = static_cast<PolygonShape*>(a->shape);
    const CircleShape* circleShape = static_cast<CircleShape*>(b->shape);
    
    // Find the closest point on the polygon to the circle center
    Vec2 closestPoint;
    float minDistance = std::numeric_limits<float>::max();
    Vec2 closestNormal;
    
    // Check distance to each edge of the polygon
    for (int i = 0; i < polygonShape->worldVertices.size(); i++) {
        Vec2 va = polygonShape->worldVertices[i];
        Vec2 vb = polygonShape->worldVertices[(i + 1) % polygonShape->worldVertices.size()];
        
        // Get the edge vector and normal
        Vec2 edge = vb - va;
        Vec2 edgeNormal = edge.Normal();
        
        // Vector from edge start to circle center
        Vec2 toCircle = b->position - va;
        
        // Project circle center onto the edge
        float edgeLength = edge.Magnitude();
        float projection = toCircle.Dot(edge) / edgeLength;
        projection = std::max(0.0f, std::min(projection, edgeLength));
        
        // Find the closest point on this edge
        Vec2 pointOnEdge = va + (edge / edgeLength) * projection;
        
        // Calculate distance from circle center to this point
        Vec2 toCenter = b->position - pointOnEdge;
        float distance = toCenter.Magnitude();
        
        if (distance < minDistance) {
            minDistance = distance;
            closestPoint = pointOnEdge;
            closestNormal = (distance > 0) ? toCenter / distance : Vec2(1, 0); // Normalize 
        }
    }
    
    // Check if collision occurred
    if (minDistance >= circleShape->radius) {
        return false; // No collision
    }
    
    // Set up contact information
    contact.a = a;
    contact.b = b;
    contact.depth = circleShape->radius - minDistance;
    contact.normal = closestNormal;
    contact.start = closestPoint;
    contact.end = b->position - closestNormal * circleShape->radius;
    
    return true;
}

bool CollisionDetection::isCircleCircleColliding(Body* a, Body* b, ContactInformation &contact){
    CircleShape* aCircleShape = static_cast<CircleShape*> (a->shape); 

    CircleShape* bCircleShape = static_cast<CircleShape*> (b->shape);
    
    const Vec2 ab = b->position - a->position; 
    const float radiusSum = aCircleShape->radius + bCircleShape->radius; 

    bool isColliding = ab.MagnitudeSquared() <= (radiusSum * radiusSum); 
    
    if (!isColliding){
        return false; 
    }

    contact.a = a; 
    contact.b = b; 
    contact. normal = ab; 
    contact.normal.Normalize(); 
    
    contact.start = b->position - contact.normal * bCircleShape->radius; 
    contact.end = a->position + contact.normal * aCircleShape->radius; 
    contact.depth = (contact.end - contact.start).Magnitude();  
    return true; 
}

bool CollisionDetection::IsCollidingPolygonPolygon(Body* a, Body* b, ContactInformation& contact) {
    const PolygonShape* aPolygonShape = static_cast<PolygonShape*>(a->shape);
    const PolygonShape* bPolygonShape = static_cast<PolygonShape*>(b->shape);
    Vec2 aAxis, bAxis;
    Vec2 aPoint, bPoint;
    float abSeparation = aPolygonShape->FindMinSeparation(bPolygonShape, aAxis, aPoint);
    if (abSeparation >= 0) {
        return false;
    }
    float baSeparation = bPolygonShape->FindMinSeparation(aPolygonShape, bAxis, bPoint);
    if (baSeparation >= 0) {
        return false;
    }
    contact.a = a;
    contact.b = b;
    if (abSeparation > baSeparation) {
        contact.depth = -abSeparation;
        contact.normal = aAxis.Normal();
        contact.start = aPoint;
        contact.end = aPoint + contact.normal * contact.depth;
    } else {
        contact.depth = -baSeparation;
        contact.normal = -bAxis.Normal();
        contact.start = bPoint - contact.normal * contact.depth;
        contact.end = bPoint;
    }
    return true;
}
