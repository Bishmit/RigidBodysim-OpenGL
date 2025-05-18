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
#include "Physics/Body.h"
#include "Physics/Shape.h"
#include "Physics/CollisionDetection.h"
#include "../stb/stb_image.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <imgui.h>

class Renderer {
    public:
       static glm::mat4 projection;  // Global projection matrix
       static int screenWidth;
       static int screenHeight;

        static void Init(GLFWwindow* window);
        static void SetUp();
        static void MouseButtonCallBack(GLFWwindow* window, int button, int action, int mods);  
        static void Update(GLFWwindow* window);
        static void RenderGUI();
        static void Shutdown();
    
        static void DrawCircle(glm::vec2 pos, float radius, glm::vec3 color);
        static void DrawRectangle(glm::vec2 pos, float width, float height, glm::vec3 color);
        static void DrawPolygon(const glm::vec2* points, int count, glm::vec3 color);
        static void DrawLine(glm::vec2 p1, glm::vec2 p2, glm::vec3 color);

        static void framebuffer_size_callback(GLFWwindow* window, int width, int height); 

        // Function to check if mouse is inside the circle or not 
        static bool IsPointInCircle(int pointX, int pointY, int circleX, int circleY, int radius); 
        
        static void Destroy(); 
        static GLuint shaderProgram, circleVAO, circleVBO, rectVAO, rectVBO;
        // static const int screenWidth = 800;
        // static const int screenHeight = 600;
        static float radius;
        static float width;
        static float height;
    
        static std::vector<float> generateCircleOutline(int segments = 100);
        
        static const float rectVertices[8];
        static const char* vertexShaderSource;
        static const char* fragmentShaderSource;
    
        static GLuint compileShaderProgram();
        static std::vector<Body*> bodies; 
        static float radius_; 
        static Body* greatBall; 
        static Body* smallBall; 
        static bool isDragging, isRigidHingeDragging; 
    };