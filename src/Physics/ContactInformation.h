#pragma once
#include "Body.h"
#include "Math/Vec2.h"

struct ContactInformation {
    Body* a; 
    Body* b; 
    Vec2 start, end, normal; 
    float depth; 
    struct Distance{
       float a, b;  
    };
    Distance distance; 
    ContactInformation() = default; 
    ~ContactInformation() = default; 
};

