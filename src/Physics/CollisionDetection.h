#pragma once 
#include "Body.h"
#include "../Math/Vec2.h"

namespace CollisionDetection
{
    bool IsColliding(Body* a, Body* b); 
    bool IsCircleCircleColliding(Body* a, Body* b); 
};
