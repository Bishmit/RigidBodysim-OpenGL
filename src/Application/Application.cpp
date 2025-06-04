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
float Application::deltaTime = 0.f; 
bool Application::pause = false; 
bool Application::showNormal = false;
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
bool Application::isRigidHingeDragging = false;
Body* Application::draggedBody = nullptr;
Vec2 Application::dragOffset; 
ContactInformation Application::contact; 

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
    greatBall = new Body(CircleShape(150), 400, 300, 0.f, 0.f);
    bodies.push_back(greatBall);
    radius_ = greatBall->GetRadius();

    Body* floor1 = new Body(BoxShape(800.f , 20.f), 700.f, 450.f, 0.f, glm::radians(15.f)); 
    bodies.push_back(floor1); 
    Body* floor2 = new Body(BoxShape(800.f , 20.f), 1200.f, 750.f, 0.f, glm::radians(0.f)); 
    bodies.push_back(floor2); 
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
        if(isDragging){
            double mouseX, mouseY; 
            glfwGetCursorPos(window, &mouseX, &mouseY);

                // Update the position to follow the mouse
                greatBall->position.x = mouseX;
                greatBall->position.y = mouseY;
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
    }

       if (body->shape->GetType() == BOX) {
            body->isColliding = false;
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

void Application::MouseButtonCallBack(GLFWwindow* window, int button, int action, int mods) {
    double x, y;
    glfwGetCursorPos(window, &x, &y);

    switch (action) {
    case GLFW_PRESS:
        switch (button) {
        case GLFW_MOUSE_BUTTON_RIGHT:
            if (mods & GLFW_MOD_SHIFT) {
                // Shift + Right Click -> Create a box
                otherBox = new Body(BoxShape(60.f , 60.f), x, y, 1.f, 0.f);
                bodies.push_back(otherBox);
            } else {
                // Just Right Click -> Create a circle
                smallBall = new Body(CircleShape(35), x, y, 1.f, 0.f);
                bodies.push_back(smallBall);
            }

            break;

        case GLFW_MOUSE_BUTTON_LEFT:
            // Check for dragging the circle
             if (mods & GLFW_MOD_SHIFT) {
                // Shift + left Click -> box
                otherPolygon =  otherBox = new Body(PolygonShape(RandomNumber(3, 6), 40.f), x, y, 1.f, 0.f);
                bodies.push_back(otherPolygon); 
             }

            else if (greatBall && Renderer::IsPointInCircle(x, y, greatBall->position.x, greatBall->position.y, greatBall->GetRadius())) {
                isDragging = true;
            }

            // Check for dragging the box
            if (bigBox && bigBox->shape->GetType() == BOX) {
                BoxShape* boxShape = static_cast<BoxShape*>(bigBox->shape);
                if (isPointInBox(static_cast<int>(x), static_cast<int>(y), bigBox, boxShape)) {
                    isDragging = true;
                }
            }
            break;
        }
        break;

    case GLFW_RELEASE:
        switch (button) {
        case GLFW_MOUSE_BUTTON_LEFT:
            isDragging = false;
            break;
        }
        break;
    }
}


float Application::EaseOut(float a, float b, float t) {
    t = 1 - powf(1 - t, 3);  // cubic ease out
    return a + (b - a) * t;
}

void Application::RenderGUI(GLFWwindow* window) {
    static bool show_panel = true; // toggle this with a key/button
    static float panel_x = 0.0f;
    float panel_width = 350.0f;
    
    ImGuiIO& io = ImGui::GetIO();
    float screen_width = io.DisplaySize.x;
    float screen_height = io.DisplaySize.y;
    float target_x = show_panel ? (screen_width - panel_width) : screen_width;
    float dt = io.DeltaTime;

    // Smooth slide animation
     panel_x = EaseOut(panel_x, target_x, dt * 10.0f);

    ImGui::SetNextWindowPos(ImVec2(panel_x, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(panel_width, screen_height), ImGuiCond_Always);
    ImGui::Begin("RigidBody Properties", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove);
     ImGui::Text("Note: Press 'H' to toggle the side menu");

    if (ImGui::SliderFloat("Circle Radius", &radius_, 10.0f, 200.0f)) {
        greatBall->SetRadius(radius_);
    }
    ImGui::SliderFloat("Restitution", &restiutionValue, 0.0f, 1.0f);
    ImGui::SliderFloat("Gravity", &gravity, -10.0f, 10.0f);
    ImGui::SliderFloat("Friction", &frictionValue, 0.0f, 1.0f);

    if (ImGui::Button("Clear Scene")) {
        bool success = ClearScreen();
        if (success) {
            ImGui::Text("Scene cleared successfully!");
        } else {
            ImGui::Text("Failed to clear scene.");
        }
    }

    ImGui::SameLine(0, 15.f); 
    if (ImGui::Button(pause ? "Resume" : "Pause")) {
        pause = !pause;
    }

    if (ImGui::Button(showNormal ? "Hide Normals" : "Show Normals")) {
        showNormal = !showNormal;
    }

    ImGui::SameLine(0, 15.f);
    if (ImGui::Button(showCollisionPoint ? "Hide Contact Point" : "Show Contact Point")) {
        showCollisionPoint = !showCollisionPoint;
    }

    ImGui::InputInt("Max Iteration", &maxIteration, 1.f);
    if (ImGui::SliderFloat("Correction", &correctionValue, 0.0f, 1.0f)) {
        CollisionSolver::SetCorrectionValue(correctionValue);
    }

        ImGui::BeginChild("Canvas", ImVec2(400, 600), true);
        ImDrawList* canvas = ImGui::GetWindowDrawList();
        ImVec2 canvasPos = ImGui::GetCursorScreenPos();

        // Box Icon
        canvas->AddRect(
        ImVec2(canvasPos.x, canvasPos.y),                            // Top-left
        ImVec2(canvasPos.x + 30, canvasPos.y + 30),                  // Bottom-right
        IM_COL32(255, 255, 0, 255),                                  // Yellow outline
        0.0f,                                                        // No corner rounding
        ImDrawFlags_None,
        2.0f                                                         // Thickness
        );

        ImGui::Dummy(ImVec2(30, 30));
        ImGui::SameLine();
        ImGui::Text("Add Box");
        
        static float addBoxWidth = 60.f; 
        static float addBoxHeight = 60.f; 
        static float rotation = 0.f; 
        static float isNonStatic = 1; 

        bool checked = (isNonStatic != 1); 

        ImGui::Text("Box Size:");
        ImGui::InputFloat("Width", &addBoxWidth, 1.0f, 10.0f, "%.1f");
        ImGui::InputFloat("Height", &addBoxHeight, 1.0f, 10.0f, "%.1f");

        // === Create Button ===
        if (ImGui::Button("Create")) {
            Body* addBox = new Body(BoxShape(addBoxWidth , addBoxHeight), 500.f, 700.f, isNonStatic, rotation);
            bodies.push_back(addBox);
        }

        ImGui::SameLine();
        if(ImGui::Checkbox("Static", &checked)){
            isNonStatic = checked ? 0 : 1; 
        }
        ImGui::SliderFloat("Rotation", &rotation, -360.f, 360.0f);


        ImGui::EndChild();

    ImGui::End();

    // Optional: toggle the panel with a key or button
    if (ImGui::IsKeyPressed(ImGuiKey_H)) {
        show_panel = !show_panel;
    }
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

bool Application::isPointInCircle(int pointX, int pointY, int circleX, int circleY, int radius) {
    // Calculate the squared distance between the point and the circle center
    int deltaX = pointX - circleX;
    int deltaY = pointY - circleY;
    int distanceSquared = deltaX * deltaX + deltaY * deltaY;
    
    // Return true if the mouse is inside the circle (distance <= radius)
    return distanceSquared <= (radius * radius);
}

// Function to check if point is inside a box (rectangle)
bool Application::isPointInBox(int pointX, int pointY, Body* body, BoxShape* boxShape) {
    // Simple AABB (Axis-Aligned Bounding Box) check
    // This assumes the box is not rotated significantly
    std::cout<<"running"<<"\n"; 
    float halfWidth = boxShape->width / 2.0f;
    float halfHeight = boxShape->height / 2.0f;
    
    return (pointX >= body->position.x - halfWidth && 
            pointX <= body->position.x + halfWidth &&
            pointY >= body->position.y - halfHeight && 
            pointY <= body->position.y + halfHeight);
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
    Renderer r; 
    auto monitor = r.GetMonitor(window);  
    
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

bool Application::ClearScreen() {
    
    auto it = std::remove_if(bodies.begin(), bodies.end(), [](Body* body) {
        if (!body) return false; 
        if (!body->IsStatic()) {
            delete body;        
            return true;         
        }
        return false;          
    });

    bodies.erase(it, bodies.end());
    return true;
}
