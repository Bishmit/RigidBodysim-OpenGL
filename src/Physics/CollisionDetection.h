#pragma once
#include "Body.h"
#include "ContactInformation.h"
#include <limits>

namespace CollisionDetection {
   bool isColliding(Body* a, Body* b, ContactInformation &contact); 
   bool isCircleCircleColliding(Body* a, Body* b, ContactInformation &contact);
   bool IsCollidingPolygonPolygon(Body* a, Body* b, ContactInformation& contact);
   bool IsCollidingPolygonCircle(Body* a, Body* b, ContactInformation& contact);  
};
