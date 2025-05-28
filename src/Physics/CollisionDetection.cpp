#include "CollisionDetection.h"
#include "Physics/ContactInformation.h"
#include <algorithm>

bool CollisionDetection::IsColliding(Body* a, Body* b, ContactInformation &contact){
    ShapeType aType = a->shape->GetType(); 
    ShapeType bType = b->shape->GetType(); 

    bool aIsCircle = aType == CIRCLE; 
    bool bIsCircle = bType == CIRCLE; 

    bool aIsBox = aType == BOX; 
    bool bIsBox = bType == BOX; 

    if(aIsCircle && bIsCircle){
        return IsCircleCircleColliding(a, b, contact); 
    }

    if(aIsBox && bIsBox){
        return IsBoxBoxColliding(a, b, contact); 
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

std::vector<Vec2> CollisionDetection::GetBoxCorners(Body* rect) {
    PolygonShape* shape = static_cast<PolygonShape*>(rect->shape); 
    shape->UpdateVertices(rect->rotation, rect->position); 
    return shape->GetWorldVertices();
}

std::pair<float, float> CollisionDetection::ProjectCornersOntoAxis(Body* rect, Vec2 axis) {
    auto corners = GetBoxCorners(rect);
    return ProjectCornersOntoAxis(corners, axis);
}

std::pair<float, float> CollisionDetection::ProjectCornersOntoAxis(const std::vector<Vec2>& corners, Vec2 axis) {
    float min = std::numeric_limits<float>::max();
    float max = std::numeric_limits<float>::lowest();

    for (auto& corner : corners) {
        float projection = corner.Dot(axis);
        min = std::min(min, projection);
        max = std::max(max, projection);
    }

    return {min, max};
}

bool CollisionDetection::overlapOnAxis(const std::pair<float, float>& projA, const std::pair<float, float>& projB) {
    return !(projA.second < projB.first || projB.second < projA.first);
}

std::vector<Vec2> CollisionDetection::GetAxes(Body* rect) {
    auto corners = GetBoxCorners(rect);
    std::vector<Vec2> axes;

    for (size_t i = 0; i < corners.size(); i++) {
        Vec2 edge = corners[(i + 1) % corners.size()] - corners[i];
        Vec2 axis = Vec2(-edge.y(), edge.x()).Normalize();
        axes.push_back(axis);
    }

    // Remove duplicate axes (optimization)
    if (axes.size() > 2) {
        axes.resize(2);
    }

    return axes;
}

bool CollisionDetection::CheckAxes(const std::vector<Vec2>& axes, Body* a, Body* b, float& minDepth, Vec2& normal) {
    for (const auto& axis : axes) {
        auto projA = ProjectCornersOntoAxis(a, axis);
        auto projB = ProjectCornersOntoAxis(b, axis);

        if (!overlapOnAxis(projA, projB)) {
            return false;
        }

        float axisDepth = std::min(projB.second - projA.first, projA.second - projB.first);

        if (axisDepth < minDepth) {
            minDepth = axisDepth;
            normal = axis;
        }
    }
    return true;
}

bool CollisionDetection::IsBoxBoxColliding(Body* a, Body* b, ContactInformation& contact) {
    float minDepth = std::numeric_limits<float>::max();
    Vec2 normal;
    int bestAxisIndex = -1;
    bool isAxisFromA = false;
    
    auto cornersA = GetBoxCorners(a);
    auto cornersB = GetBoxCorners(b);
    std::vector<Vec2> axes;
    
    // Get axes from box A
    for (size_t i = 0; i < cornersA.size(); i++) {
        Vec2 edge = cornersA[(i + 1) % cornersA.size()] - cornersA[i];
        Vec2 axis = Vec2(-edge.y(), edge.x()).Normalize();
        axes.push_back(axis);
    }
    
    // Get axes from box B
    for (size_t i = 0; i < cornersB.size(); i++) {
        Vec2 edge = cornersB[(i + 1) % cornersB.size()] - cornersB[i];
        Vec2 axis = Vec2(-edge.y(), edge.x()).Normalize();
        axes.push_back(axis);
    }
    
    // Test all axes
    for (size_t axisIdx = 0; axisIdx < axes.size(); axisIdx++) {
        const auto& axis = axes[axisIdx];
        auto projA = ProjectCornersOntoAxis(cornersA, axis);
        auto projB = ProjectCornersOntoAxis(cornersB, axis);
        
        if (!overlapOnAxis(projA, projB)) {
            return false; // Separating axis found
        }
        
        float axisDepth = std::min(projB.second - projA.first, projA.second - projB.first);
        if (axisDepth < minDepth) {
            minDepth = axisDepth;
            normal = axis;
            bestAxisIndex = axisIdx;
            isAxisFromA = (axisIdx < 4); // First 4 axes are from box A
        }
    }
    
    // Ensure normal points from A to B
    Vec2 dirToB = b->position - a->position;
    if (dirToB.Dot(normal) < 0.0f) {
        normal = -normal;
    }
    
    contact.a = a;
    contact.b = b;
    contact.normal = normal;
    contact.depth = minDepth;
    Vec2 midPoint = (a->position + b->position) / 2.f;
    Vec2 contactPoint = midPoint - contact.normal / 2.f * contact.depth;
    contact.start = contactPoint;
    contact.end = contactPoint + contact.normal * contact.depth;
    return true;
}

