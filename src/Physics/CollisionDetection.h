#pragma once 
#include "Body.h"
#include "../Math/Vec2.h"
#include "ContactInformation.h"

namespace CollisionDetection
{
    bool IsColliding(Body* a, Body* b, ContactInformation &contact); 
    bool IsCircleCircleColliding(Body* a, Body* b, ContactInformation &contact); 

   static std::vector<Vec2> GetBoxCorners(Body* rect);
    static std::pair<float, float> ProjectCornersOntoAxis(Body* rect, Vec2 axis);
    static std::pair<float, float> ProjectCornersOntoAxis(const std::vector<Vec2>& corners, Vec2 axis);
    static bool overlapOnAxis(const std::pair<float, float>& projA, const std::pair<float, float>& projB);
    static std::vector<Vec2> GetAxes(Body* rect);
    static bool CheckAxes(const std::vector<Vec2>& axes, Body* a, Body* b, float& minDepth, Vec2& normal);
    static bool IsBoxBoxColliding(Body* a, Body* b, ContactInformation& contact);
}
