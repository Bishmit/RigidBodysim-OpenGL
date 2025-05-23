#pragma once 
#include "Body.h"
#include "../Math/Vec2.h"
#include "ContactInformation.h"
namespace CollisionDetection
{
    bool IsColliding(Body* a, Body* b, ContactInformation &contact); 
    bool IsCircleCircleColliding(Body* a, Body* b, ContactInformation &contact); 
};
