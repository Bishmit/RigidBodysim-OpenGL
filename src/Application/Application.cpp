#include "Application.h"

// === Static Members Initialization ===
int Application::screenWidth = 800;
int Application::screenHeight = 600;
float Application::radius = 50.0f;
float Application::width = 100.0f;
float Application::height = 50.0f;
float Application::radius_ = 0.0f;

std::vector<Body*> Application::bodies;
Body* Application::greatBall = nullptr;
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
    greatBall = new Body(CircleShape(150), 400, 300, 0.f);
    bodies.push_back(greatBall);
    radius_ = greatBall->GetRadius();
}

void Application::Update(GLFWwindow* window) {
    glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

      // Static to keep value between frames
    static double timePreviousFrame = glfwGetTime();

    // Calculate deltaTime in seconds
    double currentTime = glfwGetTime();
    float deltaTime = static_cast<float>(currentTime - timePreviousFrame);

    // Clamp deltaTime to avoid large jumps
    if (deltaTime > 0.016f)
        deltaTime = 0.016f;

    // Set current time for next frame
    timePreviousFrame = currentTime;

    int scale = Constant::PIXELS_PER_METER; 
    // Apply forces to bodies
    for (auto body : bodies) {
        Vec2 weight = Vec2(0.0, body->mass * 9.8 * scale);
        body->AddForce(weight);

        // Optional:
        // Vec2 wind = Vec2(2.0 * PIXELS_PER_METER, 0.0);
        // body->AddForce(wind);
        // body->AddTorque(20.0f);
    }

    // Integrate forces to update velocity/position
    for (auto body : bodies) {
       if (body) body->Update(deltaTime);
    }

    // for basic applying force and all other things
    for (auto body : bodies) {
        if(isDragging){
            double mouseX, mouseY; 
            glfwGetCursorPos(window, &mouseX, &mouseY);

            // Update the position to follow the mouse
            greatBall->position.x() = mouseX;
            greatBall->position.y() = mouseY;
        }
    }

    // Detect collisions
    for(int i = 0; i < bodies.size()-1; i++) {
        for(int j = i+1; j < bodies.size(); j++) {
            Body* a = bodies[i];
            Body* b = bodies[j];
            
            if(CollisionDetection::IsColliding(a, b, contact)) {
               // contact.ResolveCollison(); 
                CollisionSolver::ResolveCollision(contact); 
                 Renderer::DrawCircle(contact.start, 3.f, {1.0f, 0.0f, 0.0f}); 
                 Renderer::DrawCircle(contact.end, 3.f, {1.0f, 0.0f, 0.0f}); 
                 Renderer::DrawLine(contact.start, { contact.start.x() + contact.normal.x() * 20, contact.start.y() + contact.normal.y() * 20}, {1.0f, 0.0f, 0.0f}); 
                a->isColliding = true;
                b->isColliding = true;
            }
        }
    }

    // Draw bodies with appropriate colors
    for (auto body : bodies) {
        glm::vec3 color = body->isColliding 
            ? glm::vec3(1.0f, 0.0f, 0.0f)  // red if colliding
            : glm::vec3(1.0f, 1.0f, 1.0f); // white if not
        
        if (body->shape->GetType() == CIRCLE) {
            CircleShape* circle = static_cast<CircleShape*>(body->shape);
            Renderer::DrawCircle(body->position, circle->radius, color);
            Renderer::DrawLine(body->position, body->position + glm::vec2(circle->radius, 0.f), color);
        }
        
        body->isColliding = false;
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
            // Create a new small ball on right click
            smallBall = new Body(CircleShape(100), x, y, 1.f); 
            bodies.push_back(smallBall); 
            break;
        case GLFW_MOUSE_BUTTON_LEFT:
            if (greatBall && Renderer::IsPointInCircle(x, y, greatBall->position.x(), greatBall->position.y(), greatBall->GetRadius())) {
                isDragging = true;
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

void Application::Destroy() {
    for(auto body: bodies) {
        delete body;
    }
    bodies.clear();
    
    // ImGui cleanup 
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}