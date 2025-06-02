#pragma once

#include "../glad/glad.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <iostream>
#include <vector>

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <imgui.h>
#include <random>
#include <algorithm>

#include "Physics/Body.h"
#include "Physics/Shape.h"
#include "Physics/CollisionDetection.h"
#include "Physics/ContactInformation.h"
#include "Physics/CollisionSolver.h"
#include "Physics/Constants.h"

#include "Renderer.h"

class Application {
public:
    // Core application methods
    static void Init(GLFWwindow* window);
    static void SetUp();
    static void Update(GLFWwindow* window);
    static void Render(); 
    static void ClearScene(GLFWwindow* window); 
    static void Shutdown();
    static void Destroy();
    
    // User interaction
    static void RenderGUI();
    
    // Callback functions
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    static void MouseButtonCallBack(GLFWwindow* window, int button, int action, int mods);
    static void HandleWASDMovement(GLFWwindow* window, Body* body, float speed, float deltaTime);
    private:
    static int RandomNumber(int start, int end); 
    static bool isPointInBox(int pointX, int pointY, Body* body, BoxShape* boxShape);
    // Application state
    static int screenWidth, screenHeight;
    static float radius;
    static float width, height;
    static float radius_;
    static float restiutionValue; 
    static float gravity; 
    
    // Physics objects

    // ball
    static std::vector<Body*> bodies;
    static Body* greatBall;
    static Body* smallBall;

    // box 
    static Body* bigBox, *otherBox;  
   
     // polygon 
    static Body* polygon, * otherPolygon; 

    // time 
    static float deltaTime;  

    // Interaction state
    static bool isDragging;
    static bool isRigidHingeDragging;
    
    static ContactInformation contact; 
    
};