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
#include "Physics/WreckingBall/WreckingBall.h"

#include "Renderer.h"
//#include "Pendulum.h"


class Application {
    public:
    // Core application methods
    static void Init(GLFWwindow* window);
    static void SetUp();
    static void Update(GLFWwindow* window);
    static float EaseOut(float a, float b, float t); 
    static void Render(GLFWwindow* window); 
    static Body* SelectCircleInCanvas(double &x, double &y, Body* clickedBody); 
    static void ClearOffScreenBodies(GLFWwindow* window); 
    static bool ClearDynamicObjectOnScreen(); 
    static bool DeleteParticularBody(Body* body); 
    static void Shutdown();
    static void Destroy();
    
    // User interaction
    static void RenderGUI(GLFWwindow* window);
    
    // Callback functions
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    static void MouseButtonCallBack(GLFWwindow* window, int button, int action, int mods);

    static Body* getGreatBall(); 
    private:
    static int RandomNumber(int start, int end); 
 
    // Application state
    static int screenWidth, screenHeight;
    static float radius;
    static float width, height;
    static float radius_;
    static float gravity; 
    static float restiutionValue; 
    static bool pause; 
    static bool showNormal, showCollisionPoint; 
    static int maxIteration; 
    static float correctionValue;
    static float frictionValue;

    // Physics objects

    // ball
    static std::vector<Body*> bodies;
    static Body* smallBall;

    // box 
    static Body* bigBox, *otherBox;  
   
     // polygon 
    static Body* polygon, * otherPolygon; 

    // time 
    static float deltaTime;  

    // Interaction state
    static bool isDragging;
    static bool isRecentBodySelected; 
    static bool attachPendulum; 
    static Body* greatBall;
    static Body* draggedBody;
    static Body* recentSelectedBody; 
    static Vec2 dragOffset;
    
    static ContactInformation contact;
    static WreckingBall wb; 

    //static Pendulum* pendulum; 
    
};