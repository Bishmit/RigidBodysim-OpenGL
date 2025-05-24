#pragma once 

#include "ContactInformation.h"

namespace CollisionSolver{
    void ResolveOverlap(ContactInformation &contact);  
    void ResolveCollision(ContactInformation &contact); 
}