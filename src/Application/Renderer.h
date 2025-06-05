#pragma once

#include "../glad/glad.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#include "Math/Vec2.h"
#include "Physics/Body.h"

 struct Monitors
    {
        int x; 
        int y; 
    };

class Renderer {
public:
    // Initialize the renderer with the window size
    static void InitRenderer(int screenWidth, int screenHeight);
    
    // Update the projection matrix if window size changes
    static void UpdateProjection(int screenWidth, int screenHeight);
    
    // Clean up OpenGL resources
    static void CleanupRenderer();

    // Drawing functions
    static void DrawCircle(Vec2 pos, float radius, glm::vec3 color);
    static void DrawRectangle(Vec2 pos, float width, float height, glm::vec3 color, float angleRadians);
    static void DrawPolygon(const std::vector<Vec2>& points, int count, glm::vec3 color);
    static void DrawLine(Vec2 p1, Vec2 p2, glm::vec3 color);
    static void DrawRect(int x, int y, int width, int height, glm::vec3 color); 

    Monitors GetMonitor(GLFWwindow* window); 
    
    // Utility function for hit detection
    static bool IsPointInCircle(int pointX, int pointY, int circleX, int circleY, int radius);
    static bool IsPointInBox(int pointX, int pointY, Body* body, BoxShape* boxShape); 

private:
    // Shader and buffer objects
    static GLuint shaderProgram;
    static GLuint circleVAO, circleVBO;
    static GLuint rectVAO, rectVBO;
    static const float rectVertices[8];
    
    // Shader sources
    static const char* vertexShaderSource;
    static const char* fragmentShaderSource;
    
    // Projection matrix
    static glm::mat4 projection;
    static glm::vec3 color;
    
    // Private utility functions
    static std::vector<float> generateCircleOutline(int segments);
    static GLuint compileShaderProgram();
};