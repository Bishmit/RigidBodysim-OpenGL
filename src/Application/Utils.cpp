#include "Utils.h"


bool Utils::IsPointInCircle(int pointX, int pointY, int circleX, int circleY, int radius) {
    int deltaX = pointX - circleX;
    int deltaY = pointY - circleY;
    int distanceSquared = deltaX * deltaX + deltaY * deltaY;

    return distanceSquared <= (radius * radius);
}

bool Utils::IsPointInBox(int pointX, int pointY, Body* body, BoxShape* boxShape) {
    // Note: Ensure Body has a position member with x and y
    // and BoxShape has width and height
    float halfWidth = boxShape->width / 2.0f;
    float halfHeight = boxShape->height / 2.0f;

    return (pointX >= body->position.x - halfWidth &&
        pointX <= body->position.x + halfWidth &&
        pointY >= body->position.y - halfHeight &&
        pointY <= body->position.y + halfHeight);
}

Monitors Utils::GetMonitor(GLFWwindow* window) {
    Monitors monitor;

    int windowX, windowY;
    glfwGetWindowPos(window, &windowX, &windowY);

    int monitorCount;
    GLFWmonitor** monitors = glfwGetMonitors(&monitorCount);

    for (int i = 0; i < monitorCount; ++i) {
        int mx, my;
        glfwGetMonitorPos(monitors[i], &mx, &my);
        const GLFWvidmode* mode = glfwGetVideoMode(monitors[i]);

        int mw = mode->width;
        int mh = mode->height;

        // Check if window is within this monitor's bounds
        if (windowX >= mx && windowX < mx + mw &&
            windowY >= my && windowY < my + mh) {
            monitor.x = mw;
            monitor.y = mh;
            return monitor;
        }
    }

    // Return primary monitor if no match found 
    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    monitor.x = mode->width;
    monitor.y = mode->height;
    return monitor;
}