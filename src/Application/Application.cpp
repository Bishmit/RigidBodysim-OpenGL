#include "Application.h"

// === Static Members Initialization ===
int Application::screenWidth = 800;
int Application::screenHeight = 600;
float Application::radius = 50.0f;
float Application::width = 100.0f;
float Application::height = 50.0f;
float Application::radius_ = 0.0f;
float Application::restiutionValue = 0.5f; 
float Application::gravity = 10.f; 
float Application::deltaTime = 0.f; 

std::vector<Body*> Application::bodies;
Body* Application::greatBall = nullptr;
Body* Application::polygon = nullptr;
Body* Application::otherPolygon = nullptr;
Body* Application::bigBox = nullptr;
Body* Application::otherBox = nullptr;
Body* Application::smallBall = nullptr;
bool Application::isDragging = false;
bool Application::isRigidHingeDragging = false;
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

    Body* floor1 = new Body(BoxShape(800.f , 20.f), 700.f, 350.f, 0.f, glm::radians(15.f)); 
    bodies.push_back(floor1); 
    Body* floor2 = new Body(BoxShape(800.f , 20.f), 1000.f, 750.f, 0.f, glm::radians(-15.f)); 
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
    // Apply forces to bodies
    for (auto body : bodies) {
        Vec2 weight = Vec2(0.0, body->mass * 10.f * scale);
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

    // Detect collisions
// Update vertices before collision checks
for (Body* body : bodies) {
    if (body && body->shape) {
        body->shape->UpdateVertices(body->rotation, body->position);
    }
}

// Collision loop
for (size_t i = 0; i < bodies.size() - 1; i++) {
    for (size_t j = i + 1; j < bodies.size(); j++) {
        Body* a = bodies[i];
        Body* b = bodies[j];

        ContactInformation contact;
        HandleWASDMovement(window, a, 50.f, deltaTime); 
        if (a && a->shape) { 
            bool collisionHappened = CollisionDetection::isColliding(a, b, contact);
            
            if (collisionHappened) {
                CollisionSolver::ResolveCollision(contact); 
                Renderer::DrawCircle(contact.start, 3.f, {1.0f, 0.0f, 0.0f});
                Renderer::DrawCircle(contact.end, 3.f, {0.0f, 1.0f, 0.0f});
                
                Vec2 direction = contact.end - contact.start;
                if (direction.Magnitude() > 0.0f) {
                    direction = direction.Normalize();
                    Renderer::DrawLine(
                        contact.start,
                        contact.start + direction * 5.0f,
                        {0.0f, 1.0f, 1.0f}
                    );
                }
            }
        } else {
            std::cerr << "a or a->shape is null.\n";
        }

    }
}
}

void Application::Render(){
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
    
    RenderGUI();
    
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


void Application::RenderGUI() {
    ImGui::Begin("Application Demo");
    if (ImGui::SliderFloat("Circle Radius", &radius_, 10.0f, 200.0f)) {
        greatBall->SetRadius(radius_);
    }
    ImGui::End();
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

void Application::HandleWASDMovement(GLFWwindow* window, Body* body, float speed, float deltaTime) {
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        body->position.y += speed * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        body->position.y -= speed * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        body->position.x -= speed * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        body->position.x += speed * deltaTime;
}

bool Application::isPointInBox(int pointX, int pointY, Body* body, BoxShape* boxShape) {
    // Simple AABB (Axis-Aligned Bounding Box) check
    // This assumes the box is not rotated significantly
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

void Application::ClearScene(GLFWwindow* window) {
    Renderer r; 
    auto monitor = r.GetMonitor(window);  
    
    auto it = std::remove_if(bodies.begin(), bodies.end(),
        [monitor](Body* body) {
            if ((body->position.x > monitor.x || body->position.y > monitor.y) && !body->IsStatic()) {
                std::cout << "Deleting body at position (" 
                          << body->position.x << ", " 
                          << body->position.y << ") "
                          << "because it exceeds monitor size (" 
                          << monitor.x << ", " << monitor.y << ").\n";
                delete body;
                return true;
            }
             return false; 
        });

    if (it != bodies.end()) {
        bodies.erase(it, bodies.end());
    } 
}
