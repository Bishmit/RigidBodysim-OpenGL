#pragma once

#include "../glad/glad.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Physics/Body.h"
#include "Physics/Constants.h"

struct Monitors
{
    int x;
    int y;
};

class Utils {
public:
    // Disallow creating instances of this class
    Utils() = delete;

    static bool IsPointInCircle(int pointX, int pointY, int circleX, int circleY, int radius);

    static bool IsPointInBox(int pointX, int pointY, Body* body, BoxShape* boxShape);

    static Monitors GetMonitor(GLFWwindow* window);
};