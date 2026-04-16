#include "Application.h"

// === Static Members Initialization ===
int Application::screenWidth = 800;
int Application::screenHeight = 600;
float Application::radius = 50.0f;
float Application::width = 100.0f;
float Application::height = 50.0f;
float Application::radius_ = 0.0f;
float Application::restiutionValue = 0.65f; 
float Application::gravity = 9.81f; 
float Application::toastTimer = 0.0f; 
float Application::deltaTime = 0.f; 
bool Application::pause = false; 
bool Application::showNormal = false;
bool Application::attachPendulum = false; 
bool Application::showCollisionPoint = false;
int Application::maxIteration = 3; 
float Application::correctionValue = 0.85f; 
float Application::frictionValue = 0.5f; 

std::vector<Body*> Application::bodies;
Body* Application::greatBall = nullptr;
Body* Application::polygon = nullptr;
Body* Application::otherPolygon = nullptr;
Body* Application::bigBox = nullptr;
Body* Application::otherBox = nullptr;
Body* Application::smallBall = nullptr;
bool Application::isDragging = false;
bool Application::isRecentBodySelected = false; 
bool Application::showSavedToast = false;
bool Application::showLoadFailToast = false;
bool Application::showOverwriteModal = false;
Body* Application::draggedBody = nullptr;
Body* Application::recentSelectedBody = nullptr;
Vec2 Application::dragOffset; 
ContactInformation Application::contact;
WreckingBall Application::wb; 

//State Save / Load 
std::string Application::currentSceneName = "untitled";
char        Application::stateName[128] = "state_01";
char        Application::pendingFilepath[256] = {};
char        Application::newSaveName[128] = {};

void Application::Init(GLFWwindow* window) {
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        return;
    }
    glViewport(0, 0, screenWidth, screenHeight);
    
    // Initialize the renderer
    Renderer::InitRenderer(screenWidth, screenHeight);
    
    // Set callbacks
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetMouseButtonCallback(window, MouseButtonCallBack);
    
    // Setup ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    ImGui::StyleColorsDark();
}

void Application::SetUp() {
    greatBall = new Body(CircleShape(100), 400, 300, 0.f, 0.f);
    bodies.push_back(greatBall);
    radius_ = greatBall->GetRadius();

    Body* floor1 = new Body(BoxShape(800.f , 20.f), 700.f, 450.f, 0.f, glm::radians(15.f)); 
    bodies.push_back(floor1); 
    Body* floor2 = new Body(BoxShape(800.f , 20.f), 1200.f, 750.f, 0.f, glm::radians(-15.f)); 
    bodies.push_back(floor2); 
}

Body* Application::getGreatBall(){
    return greatBall; 
}

void Application::Update(GLFWwindow* window) {
    glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

      // Static to keep value between frames
    static double timePreviousFrame = glfwGetTime();

    // Calculate deltaTime in seconds
    double currentTime = glfwGetTime();
    deltaTime = static_cast<float>(currentTime - timePreviousFrame);

    // Clamp deltaTime to avoid large jumps
    if (deltaTime > 0.016f)
        deltaTime = 0.016f;

    // Set current time for next frame
    timePreviousFrame = currentTime;

    int scale = Constants::PIXELS_PER_METER; 

    // pause/Resume 

    if(!pause){
    // Apply forces to bodies
    for (auto body : bodies) {
        Vec2 weight = Vec2(0.0, body->mass * body->gravity * scale);
        body->AddForce(weight);

        // Optional:
        // Vec2 wind = Vec2(2.0 * PIXELS_PER_METER, 0.0);
        // body->AddForce(wind);
        // body->AddTorque(20.0f);
    }

    // Integrate forces to update velocity/position
     for (auto body : bodies) {
        body->Update(deltaTime);
        body->restitution = restiutionValue;
        body->gravity = gravity;  
        body->friction = frictionValue; 
    }

    // for basic applying force and all other things
    for (auto body : bodies) {
        if(draggedBody){
            double mouseX, mouseY; 
            glfwGetCursorPos(window, &mouseX, &mouseY);

              // Update the position to follow the mouse
                draggedBody->position.x = mouseX - dragOffset.x;
                draggedBody->position.y = mouseY - dragOffset.y;

                // Reset velocity for dragged objects to prevent unwanted movement
                draggedBody->velocity.x = 0;
                draggedBody->velocity.y = 0;
            }
        }
    }
        // Detect collisions
    // Update vertices before collision checks
    for (Body* body : bodies) {
        if (body && body->shape) {
            body->shape->UpdateVertices(body->rotation, body->position);
        }
    }

// Collision loop
for (int n = 0; n < maxIteration; n++){
for (size_t i = 0; i < bodies.size() - 1; i++) {
    for (size_t j = i + 1; j < bodies.size(); j++) {
        Body* a = bodies[i];
        Body* b = bodies[j];

        ContactInformation contact; 
        if (a && a->shape) { 
            bool collisionHappened = CollisionDetection::isColliding(a, b, contact);
            
            if (collisionHappened) {
                a->allowRotation = true; 
                b->allowRotation = true; 
                CollisionSolver::ResolveCollision(contact); 

                if(showCollisionPoint){
                Renderer::DrawCircle(contact.start, 3.f, {1.0f, 0.0f, 0.0f});
                Renderer::DrawCircle(contact.end, 3.f, {0.0f, 1.0f, 0.0f});
                }
                
                if(showNormal){
                Vec2 direction = contact.end - contact.start;
                if (direction.Magnitude() > 0.0f) {
                    direction = direction.Normalize();
                    Renderer::DrawLine(
                        contact.start,
                        contact.start + direction * 15.0f,
                        {0.0f, 1.0f, 1.0f}
                    );
                }
              }
            }
        } else {
            std::cerr << "a or a->shape is null.\n";
        }

    }
  } 
}

}

void Application::Render(GLFWwindow* window){
// Draw bodies with appropriate colors
    for (auto body : bodies) {        
        if (body->shape->GetType() == CIRCLE) {
            CircleShape* circle = static_cast<CircleShape*>(body->shape);
          Renderer::DrawCircle(body->position, circle->radius, glm::vec3(1.0f, 1.0f, 1.0f));
          Renderer::DrawLine(
            body->position,
            {
                body->position.x + cosf(body->rotation) * circle->radius,
                body->position.y + sinf(body->rotation) * circle->radius
            },
            glm::vec3(1.0f, 1.0f, 1.0f)
        );

        // Making outline color highlighted to make sure it is selected 
        if(recentSelectedBody && !recentSelectedBody->IsStatic() &&isRecentBodySelected){
           static float offSet = 1.0f; 
           Renderer::DrawCircle(recentSelectedBody->position, recentSelectedBody->GetRadius() - offSet, glm::vec4(1.0f, 1.0f, 0.0f, 0.5f));
        }
        
    }
        if (body->shape->GetType() == CIRCLE) {
            body->isColliding = false;
         }
        
        if (body->shape->GetType() == POLYGON) {  
        PolygonShape* polygonShape = static_cast<PolygonShape*>(body->shape);
        
        Renderer::DrawPolygon(polygonShape->worldVertices, polygonShape->worldVertices.size(), glm::vec3(1.0f, 1.0f, 0.0f));
      }   

      if (body->shape->GetType() == POLYGON) {
            body->isColliding = false;
         }

      if(body->shape->GetType() == BOX){
        BoxShape* boxShape = static_cast<BoxShape*>(body->shape); 
        Renderer::DrawRectangle(body->position, boxShape->width, boxShape->height, glm::vec3 (0.5f, 1.0f, 0.5f), body->rotation); 
        //Renderer::DrawRect(body->position.x, body->position.y, boxShape->width, boxShape->height, color);  

        // Making outline color highlighted to make sure it is selected 
        if(recentSelectedBody && isRecentBodySelected){
        BoxShape* _boxShape = static_cast<BoxShape*>(recentSelectedBody->shape); 
           static float offSet = 1.0f; 
           Renderer::DrawRectangle(recentSelectedBody->position, _boxShape->width - offSet, _boxShape->height - offSet, glm::vec4 (1.0f, 1.0f, 0.5f, 0.1f), recentSelectedBody->rotation); 
        }
    }

       if (body->shape->GetType() == BOX) {
            body->isColliding = false;
         }
    }

    // position of the hinge of pendulum
    static Vec2 origin = {700.f, 50.f};
    
    if(attachPendulum)
    {
    for (auto body : bodies) {
        if (body->shape->GetType() == CIRCLE && body->IsStatic()) {
            Renderer::DrawLine(origin, body->position, glm::vec4(1.0f, 1.0f, 0.5f, 1.0f));
            auto bobPos = wb.SolvePendulum(body->gravity, origin, body->position, deltaTime, isRecentBodySelected);
            body->position = bobPos; 
        }
        break; 
    }
  }

    // Render ImGui
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    
    RenderGUI(window);
    
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Application::SaveState(const std::string& filepath)
{
    

    nlohmann::json j; 
    j["globalGravity"] = gravity; 
    j["globalRestituion"] = restiutionValue; 
    j["globalFriction"] = frictionValue; 
    j["pause"] = pause; 
    j["pendulumAttached"] = attachPendulum; 
    
    nlohmann::json bodyArray = nlohmann::json::array(); 

    for (auto* body : bodies) {
        nlohmann::json b; 
        // --- Transform ---
        b["x"] = body->position.x;
        b["y"] = body->position.y;
        b["rotation"] = body->rotation;

        // --- Motion (this is what makes it a STATE save) ---
        b["velocityX"] = body->velocity.x;
        b["velocityY"] = body->velocity.y;
        b["angularVelocity"] = body->angularVelocity;

        // --- Physics properties ---
        b["mass"] = body->mass;
        b["restitution"] = body->restitution;
        b["friction"] = body->friction;
        b["gravity"] = body->gravity;

        // --- Shape ---
        ShapeType type = body->shape->GetType();
        if (type == CIRCLE) {
            CircleShape* c = static_cast<CircleShape*>(body->shape);
            b["shape"] = "circle";
            b["radius"] = c->radius;
        }
        else if (type == BOX) {
            BoxShape* box = static_cast<BoxShape*>(body->shape);
            b["shape"] = "box";
            b["width"] = box->width;
            b["height"] = box->height;
        }
        else if (type == POLYGON) {
            PolygonShape* poly = static_cast<PolygonShape*>(body->shape);
            b["shape"] = "polygon";
            b["numSides"] = (int)poly->localVertices.size();
        }

        bodyArray.push_back(b);
    }

    j["bodies"] = bodyArray;

    // Create folder if needed
    std::filesystem::create_directories("states");
    std::ofstream file(filepath);
    file << j.dump(4);
    file.close();

    std::cout << "[State] Saved " << bodies.size() << " bodies to: " << filepath << "\n"; 
}

void Application::LoadState(const std::string& filepath)
{
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "[State] Could not open: " << filepath << "\n";
        return;
    }

    // --- Full reset ---
    for (auto* body : bodies) delete body;
    bodies.clear();
    greatBall = nullptr;
    recentSelectedBody = nullptr;
    isRecentBodySelected = false;

    nlohmann::json j;
    file >> j;
    file.close();

    // --- Restore global simulation state ---
    if (j.contains("globalGravity"))     gravity = j["globalGravity"];
    if (j.contains("globalRestitution")) restiutionValue = j["globalRestitution"];
    if (j.contains("globalFriction"))    frictionValue = j["globalFriction"];
    if (j.contains("paused"))            pause = j["paused"];
    if (j.contains("pendulumAttached"))  attachPendulum = j["pendulumAttached"];

    // --- Rebuild every body with full motion state ---
    for (auto& b : j["bodies"]) {
        float x = b["x"];
        float y = b["y"];
        float mass = b["mass"];
        float rotation = b["rotation"];
        std::string shape = b["shape"];

        Body* body = nullptr;

        if (shape == "circle") {
            body = new Body(CircleShape(b["radius"].get<float>()), x, y, mass, rotation);
        }
        else if (shape == "box") {
            body = new Body(BoxShape(b["width"].get<float>(), b["height"].get<float>()), x, y, mass, rotation);
        }
        else if (shape == "polygon") {
            body = new Body(PolygonShape(b["numSides"].get<int>(), b["size"].get<float>()), x, y, mass, rotation);
        }

        if (body) {
            // Restore full motion — this is what makes simulation resume correctly
            body->velocity.x = b["velocityX"];
            body->velocity.y = b["velocityY"];
            body->angularVelocity = b["angularVelocity"];
            body->restitution = b["restitution"];
            body->friction = b["friction"];
            body->gravity = b["gravity"];
            body->rotation = rotation;

            bodies.push_back(body);
        }
    }

    std::cout << "[State] Loaded " << bodies.size() << " bodies from: " << filepath << "\n";
}

Body* Application::SelectCircleInCanvas(double &x, double &y, Body* clickedBody){
            for (auto body : bodies) {
                     if (body->shape->GetType() == CIRCLE) {
                             CircleShape* circleShape = (CircleShape*) body->shape;
                                if (Utils::IsPointInCircle(x, y, body->position.x, body->position.y, circleShape->radius)) {
                                    clickedBody = body;
                                    // isBobSelected = true;
                                    return clickedBody; 
                                }
                            }
                             else if (body->shape->GetType() == BOX) {
                                BoxShape* boxShape = (BoxShape*) body->shape;
                                if (Utils::IsPointInBox(x, y, body, boxShape)) {
                                    clickedBody = body;
                                    return clickedBody; 
                                }
                          }
                          isRecentBodySelected = false; 
                   } 
          return nullptr; 
}

void Application::MouseButtonCallBack(GLFWwindow* window, int button, int action, int mods) {
    double x, y;
    glfwGetCursorPos(window, &x, &y);

    switch (action) {
        case GLFW_PRESS:
            switch (button) {
                case GLFW_MOUSE_BUTTON_RIGHT:
                    if (mods & GLFW_MOD_SHIFT) {
                        // Shift + Right Click -> Create a box
                        otherBox = new Body(BoxShape(60.f, 60.f), x, y, 1.f, 0.f);
                        bodies.push_back(otherBox);
                    } else {
                        // Just Right Click -> Create a circle
                        smallBall = new Body(CircleShape(35), x, y, 1.f, 0.f);
                        bodies.push_back(smallBall);
                    }
                    break;

                case GLFW_MOUSE_BUTTON_LEFT: {
                    if (mods & GLFW_MOD_SHIFT) {
                        // Shift + Left Click -> Create polygon
                        otherPolygon = new Body(PolygonShape(RandomNumber(3, 6), 40.f), x, y, 1.f, 0.f);
                        bodies.push_back(otherPolygon);
                    } else {
                        Body* clickedBody = SelectCircleInCanvas(x, y, clickedBody); 

                        // Set appropriate dragging state
                        if (clickedBody) {
                            if (clickedBody == greatBall) {
                                isDragging = true;
                                draggedBody = greatBall;
                                recentSelectedBody = greatBall; 
                                isRecentBodySelected = true; 
                            }
                            else {
                                isDragging = true;
                                draggedBody = clickedBody;
                                recentSelectedBody = clickedBody; 
                                isRecentBodySelected = true; 
                            }

                            dragOffset.x = x - clickedBody->position.x;
                            dragOffset.y = y - clickedBody->position.y;
                        }

                    }
                    break;
                }

                case GLFW_MOUSE_BUTTON_MIDDLE: {
                    // Middle click -> create a large box
                    Body* box = new Body(BoxShape(100, 100), x, y, 1.0, 0.f);
                    bodies.push_back(box);
                    break;
                }
            }
            break;

        case GLFW_RELEASE:
            if (button == GLFW_MOUSE_BUTTON_LEFT) {
                isDragging = false;
                draggedBody = nullptr;
            }
            break;
    }
}


float Application::EaseOut(float a, float b, float t) {
    t = 1 - powf(1 - t, 3);  // cubic ease out
    return a + (b - a) * t;
}

void Application::RenderGUI(GLFWwindow* window) {
    static bool  show_panel = true;
    static float panel_x = 0.0f;
    const  float panel_width = 360.0f;

    ImGuiIO& io = ImGui::GetIO();
    float    screen_w = io.DisplaySize.x;
    float    screen_h = io.DisplaySize.y;
    float    target_x = show_panel ? (screen_w - panel_width) : screen_w;

    panel_x = EaseOut(panel_x, target_x, io.DeltaTime * 10.0f);

    //  SIDE PANEL
   
    ImGui::SetNextWindowPos(ImVec2(panel_x, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(panel_width, screen_h), ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(0.92f);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12, 12));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 8));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6, 4));
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.08f, 0.08f, 0.12f, 1.f));
    ImGui::PushStyleColor(ImGuiCol_Separator, ImVec4(0.3f, 0.3f, 0.5f, 1.f));
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.14f, 0.14f, 0.20f, 1.f));
    ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImVec4(0.4f, 0.6f, 1.0f, 1.f));
    ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, ImVec4(0.6f, 0.8f, 1.0f, 1.f));
    ImGui::PushStyleColor(ImGuiCol_CheckMark, ImVec4(0.4f, 0.9f, 0.4f, 1.f));

    ImGui::Begin("##panel", nullptr,
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoTitleBar);

    // Header
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.8f, 1.0f, 1.f));
    ImGui::SetWindowFontScale(1.1f);
    ImGui::Text("  RigidBody Simulator");
    ImGui::SetWindowFontScale(1.0f);
    ImGui::PopStyleColor();

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.6f, 1.f));
    ImGui::Text("  Press H to toggle panel");
    ImGui::PopStyleColor();
    ImGui::Separator();
    ImGui::Spacing();

    // Stats row 
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.9f, 0.6f, 1.f));
    ImGui::Text("Bodies: %zu", bodies.size());
    ImGui::SameLine(0, 20.f);
    ImGui::Text("FPS: %.1f", io.Framerate);
    ImGui::PopStyleColor();
    ImGui::Spacing();

    //  SIMULATION CONTROLS
    ImGui::SeparatorText("Simulation");
    ImGui::Spacing();

    // Pause / Resume
    bool isPaused = pause;
    if (isPaused) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.6f, 0.4f, 0.0f, 1.f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.6f, 0.0f, 1.f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.4f, 0.3f, 0.0f, 1.f));
    }
    else {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.4f, 0.1f, 1.f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.6f, 0.2f, 1.f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 0.3f, 0.0f, 1.f));
    }
    if (ImGui::Button(isPaused ? "Resume" : "Pause", ImVec2(-1, 32)))
        pause = !pause;
    ImGui::PopStyleColor(3);
    ImGui::Spacing();

    ImGui::SliderFloat("Gravity", &gravity, -10.f, 10.f);
    ImGui::SliderFloat("Restitution", &restiutionValue, 0.0f, 1.f);
    ImGui::SliderFloat("Friction", &frictionValue, 0.0f, 1.f);
    ImGui::InputInt("Max Iterations", &maxIteration, 1);
    if (ImGui::SliderFloat("Correction", &correctionValue, 0.0f, 1.f))
        CollisionSolver::SetCorrectionValue(correctionValue);

    ImGui::Spacing();

    // Normals / Contact toggles
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.18f, 0.18f, 0.28f, 1.f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.28f, 0.28f, 0.45f, 1.f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.10f, 0.10f, 0.20f, 1.f));
    if (ImGui::Button(showNormal ? "Hide Normals" : "Show Normals", ImVec2(160, 28)))
        showNormal = !showNormal;
    ImGui::SameLine();
    if (ImGui::Button(showCollisionPoint ? "Hide Contacts" : "Show Contacts", ImVec2(-1, 28)))
        showCollisionPoint = !showCollisionPoint;
    ImGui::PopStyleColor(3);

    //  GREAT BALL
    ImGui::Spacing();
    ImGui::SeparatorText("Great Ball");
    ImGui::Spacing();

    if (ImGui::SliderFloat("Circle Radius", &radius_, 10.f, 200.f))
        if (greatBall) greatBall->SetRadius(radius_);

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.2f, 0.5f, 1.f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.5f, 0.3f, 0.8f, 1.f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.1f, 0.4f, 1.f));
    if (ImGui::Button(attachPendulum ? "Detach Pendulum" : "Attach Pendulum", ImVec2(-1, 28)))
        attachPendulum = !attachPendulum;
    ImGui::PopStyleColor(3);

    //  BODY BUILDER
    ImGui::Spacing();
    ImGui::SeparatorText("Body Builder");
    ImGui::Spacing();

    static float addBoxWidth = 100.f;
    static float addBoxHeight = 20.f;
    static float rotation = 0.f;

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.15f, 0.35f, 0.55f, 1.f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.25f, 0.50f, 0.80f, 1.f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.10f, 0.25f, 0.40f, 1.f));
    if (ImGui::Button("+ Add Box", ImVec2(120, 30))) {
        Body* addBox = new Body(BoxShape(addBoxWidth, addBoxHeight), 200.f, 200.f, 1.f, rotation);
        recentSelectedBody = addBox;
        bodies.push_back(addBox);
    }
    ImGui::PopStyleColor(3);

    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.55f, 0.12f, 0.12f, 1.f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.80f, 0.20f, 0.20f, 1.f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.40f, 0.08f, 0.08f, 1.f));
    if (ImGui::Button("Clear All", ImVec2(-1, 30)))
        ClearDynamicObjectOnScreen();
    ImGui::PopStyleColor(3);

    ImGui::Spacing();

    if (recentSelectedBody) {
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.11f, 0.11f, 0.18f, 1.f));
        ImGui::BeginChild("##selectedBody", ImVec2(-1, 165), true);

        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.9f, 0.6f, 1.f));
        ImGui::Text("Selected Body");
        ImGui::PopStyleColor();
        ImGui::Separator();
        ImGui::Spacing();

        if (ImGui::SliderAngle("Rotation", &rotation, -90.f, 90.f))
            recentSelectedBody->rotation = rotation;

        if (ImGui::InputFloat("Width", &addBoxWidth, 1.f, 10.f, "%.1f")) {
            recentSelectedBody->SetWidth(addBoxWidth);
            recentSelectedBody->UpdateShapeData();
        }
        if (ImGui::InputFloat("Height", &addBoxHeight, 1.f, 10.f, "%.1f")) {
            recentSelectedBody->SetHeight(addBoxHeight);
            recentSelectedBody->UpdateShapeData();
        }

        ImGui::Spacing();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.55f, 0.12f, 0.12f, 1.f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.80f, 0.20f, 0.20f, 1.f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.40f, 0.08f, 0.08f, 1.f));
        if (ImGui::Button("Delete Selected", ImVec2(-1, 28)))
            DeleteParticularBody(recentSelectedBody);
        ImGui::PopStyleColor(3);

        ImGui::EndChild();
        ImGui::PopStyleColor();
    }

    //  STATE SAVE / LOAD
    ImGui::Spacing();
    ImGui::SeparatorText("State Save / Load");
    ImGui::Spacing();

    // File name input
    ImGui::SetNextItemWidth(-1);
    ImGui::InputText("##stateName", stateName, sizeof(stateName));

    ImGui::Spacing();
    std::string filepath = std::string("states/") + stateName + ".json";

    // Save button
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.10f, 0.50f, 0.15f, 1.f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.15f, 0.70f, 0.20f, 1.f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.05f, 0.35f, 0.10f, 1.f));
    if (ImGui::Button("Save State", ImVec2(160, 34))) {
        if (std::filesystem::exists(filepath)) {
            showOverwriteModal = true;
            strncpy(pendingFilepath, filepath.c_str(), sizeof(pendingFilepath));
        }
        else {
            std::filesystem::create_directories("states");
            SaveState(filepath);
            showSavedToast = true;
            toastTimer = 2.5f;
        }
    }
    ImGui::PopStyleColor(3);

    ImGui::SameLine();

    // Load button
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.10f, 0.25f, 0.60f, 1.f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.15f, 0.40f, 0.90f, 1.f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.05f, 0.15f, 0.45f, 1.f));
    if (ImGui::Button("Load State", ImVec2(-1, 34))) {
        if (std::filesystem::exists(filepath))
            LoadState(filepath);
        else {
            showLoadFailToast = true;
            toastTimer = 2.5f;
        }
    }
    ImGui::PopStyleColor(3);

    ImGui::Spacing();
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.45f, 0.45f, 0.55f, 1.f));
    ImGui::Text("Path: states/%s.json", stateName);
    ImGui::PopStyleColor();

    //  OVERWRITE MODAL  (must be inside Begin/End)
    if (showOverwriteModal)
        ImGui::OpenPopup("File Already Exists##modal");

    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(),
        ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(390, 0), ImGuiCond_Always);
    ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.10f, 0.10f, 0.16f, 1.f));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.40f, 0.40f, 0.60f, 1.f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 14));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 8.f);

    if (ImGui::BeginPopupModal("File Already Exists##modal", nullptr,
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
    {
        // Title
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 0.75f, 0.1f, 1.f));
        ImGui::SetWindowFontScale(1.1f);
        ImGui::Text("File Already Exists");
        ImGui::SetWindowFontScale(1.0f);
        ImGui::PopStyleColor();

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::TextWrapped("A save file named \"%s\" already exists.", stateName);
        ImGui::Spacing();
        ImGui::TextWrapped("Choose to overwrite it, or save under a new name below.");
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // Overwrite
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.65f, 0.12f, 0.10f, 1.f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.90f, 0.20f, 0.15f, 1.f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.45f, 0.08f, 0.06f, 1.f));
        if (ImGui::Button("Overwrite", ImVec2(110, 34))) {
            std::filesystem::create_directories("states");
            SaveState(std::string(pendingFilepath));
            showOverwriteModal = false;
            showSavedToast = true;
            toastTimer = 2.5f;
            ImGui::CloseCurrentPopup();
        }
        ImGui::PopStyleColor(3);

        ImGui::Spacing();

        // Save As row
        ImGui::SetNextItemWidth(210.f);
        ImGui::InputText("##newname", newSaveName, sizeof(newSaveName));
        ImGui::SameLine();

        bool nameEmpty = (newSaveName[0] == '\0');
        if (nameEmpty) ImGui::BeginDisabled();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.10f, 0.35f, 0.65f, 1.f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.20f, 0.50f, 0.90f, 1.f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.05f, 0.25f, 0.50f, 1.f));
        if (ImGui::Button("Save As", ImVec2(-1, 34))) {
            std::string newPath = "states/" + std::string(newSaveName) + ".json";
            if (!std::filesystem::exists(newPath)) {
                std::filesystem::create_directories("states");
                SaveState(newPath);
                strncpy(stateName, newSaveName, sizeof(stateName));
                memset(newSaveName, 0, sizeof(newSaveName));
                showOverwriteModal = false;
                showSavedToast = true;
                toastTimer = 2.5f;
                ImGui::CloseCurrentPopup();
            }
            // if it also exists, just clear so user retypes
            else memset(newSaveName, 0, sizeof(newSaveName));
        }
        ImGui::PopStyleColor(3);
        if (nameEmpty) ImGui::EndDisabled();

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // Cancel
        float cancelW = 80.f;
        ImGui::SetCursorPosX((390.f - cancelW) * 0.5f);
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.22f, 0.22f, 0.30f, 1.f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.35f, 0.35f, 0.50f, 1.f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.15f, 0.15f, 0.22f, 1.f));
        if (ImGui::Button("Cancel", ImVec2(cancelW, 28))) {
            showOverwriteModal = false;
            memset(newSaveName, 0, sizeof(newSaveName));
            ImGui::CloseCurrentPopup();
        }
        ImGui::PopStyleColor(3);

        ImGui::EndPopup();
    }
    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor(2);

    //  TOAST NOTIFICATIONS  (still inside Begin/End)
    if (showSavedToast || showLoadFailToast) {
        bool  isSuccess = showSavedToast;
        float toastW = 280.f, toastH = 42.f;

        ImGui::SetNextWindowPos(
            ImVec2(screen_w * 0.5f - toastW * 0.5f, screen_h - toastH - 24.f),
            ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(toastW, toastH), ImGuiCond_Always);
        ImGui::SetNextWindowBgAlpha(0.88f);

        ImGui::Begin("##toast", nullptr,
            ImGuiWindowFlags_NoDecoration |
            ImGuiWindowFlags_NoInputs |
            ImGuiWindowFlags_NoNav |
            ImGuiWindowFlags_NoMove);

        if (isSuccess) {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.3f, 1.f, 0.45f, 1.f));
            ImGui::Text("State saved: %s.json", stateName);
        }
        else {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 0.35f, 0.35f, 1.f));
            ImGui::Text("File not found: %s.json", stateName);
        }
        ImGui::PopStyleColor();
        ImGui::End();

        toastTimer -= io.DeltaTime;
        if (toastTimer <= 0.f) {
            showSavedToast = false;
            showLoadFailToast = false;
        }
    }

    //  END PANEL
    ImGui::End();
    ImGui::PopStyleVar(3);
    ImGui::PopStyleColor(6);

    // H key toggle
    if (ImGui::IsKeyPressed(ImGuiKey_H))
        show_panel = !show_panel;
}

void Application::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    screenWidth = width;
    screenHeight = height;
    Renderer::UpdateProjection(width, height);
}

void Application::Shutdown() {
    Renderer::CleanupRenderer();
}

void Application::Destroy () {
    for(auto body: bodies) {
        delete body;
    }
    bodies.clear();
    
    // ImGui cleanup 
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

int Application::RandomNumber(int start, int end){
    std::random_device rd; 
    std::mt19937 gen(rd()); 
    std::vector<int> allowed; 
    for(int i = start; i<= end; i++){
        if(i == 4) continue; // we don't want square 
        else allowed.push_back(i);  
    }
    std::uniform_int_distribution<>dist(0, allowed.size() - 1); 
    return allowed[dist(gen)];
}

void Application::ClearOffScreenBodies(GLFWwindow* window) {
  /*  Renderer r; */
    auto monitor = Utils::GetMonitor(window);  
    
    auto it = std::remove_if(bodies.begin(), bodies.end(),
        [monitor](Body* body) {
            if ((body->position.x > monitor.x || body->position.y > monitor.y) && !body->IsStatic()) {
                delete body;
                return true;
            }
             return false; 
        });

    if (it != bodies.end()) {
        bodies.erase(it, bodies.end());
    } 
}

bool Application::ClearDynamicObjectOnScreen() {
    auto it = std::remove_if(bodies.begin(), bodies.end(), [](Body* body) {
        if (!body) return false;

        if (!body->IsStatic()) {
            delete body;
            return true;  // Remove from vector
        }

        return false; // Keep static body 
    });

    bodies.erase(it, bodies.end());
    return true;
}

bool Application::DeleteParticularBody(Body* body) {
   if(!body)
        return false;

    auto it = std::find(bodies.begin(), bodies.end(), body);
    if (it != bodies.end() - 1) {
        delete *it;              
        bodies.erase(it);        
        recentSelectedBody = nullptr;
        return true;
    }
    return false; 
}





