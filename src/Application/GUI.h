#pragma once

#include "../glad/glad.h"
#include <GLFW/glfw3.h>
#include <imgui.h>
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <vector>
#include <string>
#include <functional>

#include "Physics/Body.h"
#include "Physics/CollisionSolver.h"

// All simulation state that the GUI reads or writes.
struct SimContext {
    bool&   pause;
    bool&   showNormal;
    bool&   showCollisionPoint;
    bool&   attachPendulum;
    bool&   isRecentBodySelected;
    bool&   showSavedToast;
    bool&   showLoadFailToast;
    bool&   showOverwriteModal;

    float&  gravity;
    float&  restitution;
    float&  friction;
    float&  correctionValue;
    float&  radius_;
    float&  toastTimer;

    int&    maxIteration;

    std::vector<Body*>&  bodies;
    Body*&               greatBall;
    Body*&               recentSelectedBody;

    char*   stateName;       // char[128]
    char*   pendingFilepath; // char[256]
    char*   newSaveName;     // char[128]

    std::function<void(const std::string&)> onSave;
    std::function<void(const std::string&)> onLoad;
    std::function<bool()>                   onClearAll;
    std::function<bool(Body*)>              onDeleteBody;
};

class GUI {
public:
    GUI() = delete;
    static void Render([[maybe_unused]] GLFWwindow* window, SimContext& ctx);

private:
    static float EaseOut(float a, float b, float t);

    static bool  show_panel;
    static float panel_x;
    static float addBoxWidth;
    static float addBoxHeight;
    static float localRotation;
};
