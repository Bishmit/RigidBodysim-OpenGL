#pragma once 

#include "ContactInformation.h"

namespace CollisionSolver{

    static float correctionFactor = 0.8f; 
    void ResolveOverlap(ContactInformation &contact);  
    void ResolveCollision(ContactInformation &contact); 
    void SetCorrectionValue(float &correction); 
}