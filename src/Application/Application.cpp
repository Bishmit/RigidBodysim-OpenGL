#include "Application.h"

// === Static Members Initialization ===
int Application::screenWidth = 800;
int Application::screenHeight = 600;
float Application::radius = 50.0f;
float Application::width = 100.0f;
float Application::height = 50.0f;
float Application::radius_ = 0.0f;
float Application::deltaTime = 0.f; 

std::vector<Body*> Application::bodies;
Body* Application::greatBall = nullptr;
Body* Application::polygon = nullptr;
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

    Body* box = new Body(BoxShape(100.f , 100.f), 100.f, 100.f, 0.f, 0.f); 
    bodies.push_back(box); 

    Body* polygon = new Body(PolygonShape(7, 100.f), 300.f, 400.f, 0.f, 0.f); 
    bodies.push_back(polygon); 
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

    int scale = Constant::PIXELS_PER_METER; 
    // Apply forces to bodies
    for (auto body : bodies) {
        Vec2 weight = Vec2(0.0, body->mass * 9.8f * scale);
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
            //HandleWASDMovement(window, a, 50.f, deltaTime); 
            if(CollisionDetection::IsColliding(a, b, contact)) {
                CollisionSolver::ResolveCollision(contact); 
                 Renderer::DrawCircle(contact.start, 3.f, {1.0f, 1.0f, 0.0f}); // Yellow point
                 Renderer::DrawCircle(contact.end, 3.f, {1.0f, 1.0f, 0.0f});   

                 Renderer::DrawLine(
                     contact.start,
                     { contact.start.x() + contact.normal.x() * 20,
                     contact.start.y() + contact.normal.y() * 20 },
                     {0.0f, 1.0f, 0.0f} // Green line
                 );

                a->isColliding = true;
                b->isColliding = true;
            }
        }
    }
}

void Application::Render(){
// Draw bodies with appropriate colors
    for (auto body : bodies) {
        glm::vec3 color = body->isColliding 
            ? glm::vec3(1.0f, 0.0f, 0.0f)  // red if colliding
            : glm::vec3(1.0f, 1.0f, 1.0f); // white if not
        
        if (body->shape->GetType() == CIRCLE) {
            CircleShape* circle = static_cast<CircleShape*>(body->shape);
          Renderer::DrawCircle(body->position, circle->radius, color);
          Renderer::DrawLine(
    body->position,
    {
        body->position.x() + cosf(body->rotation) * circle->radius,
        body->position.y() + sinf(body->rotation) * circle->radius
    },
    color
);

        }
        if (body->shape->GetType() == CIRCLE) {
            body->isColliding = false;
         }
        
    //     if (body->shape->GetType() == POLYGON) {  
    //     PolygonShape* polygonShape = static_cast<PolygonShape*>(body->shape);
        
    //     auto polyPoints = body->GeneratePolygon();  
    //     Renderer::DrawPolygon(polyPoints, polyPoints.size(), glm::vec3(1.0f, 1.0f, 0.0f));
    //   }   

      if(body->shape->GetType() == BOX){
        BoxShape* boxShape = static_cast<BoxShape*>(body->shape); 
        Renderer::DrawRectangle(body->position, boxShape->width, boxShape->height, color, body->rotation); 
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
                    otherBox = new Body(BoxShape(100.f, 100.f), x, y, 0.f, glm::radians(45.f));
                    bodies.push_back(otherBox);
                } else {
                    // Just Right Click -> Create a circle
                    smallBall = new Body(CircleShape(100), x, y, 1.f, 0.f);
                    bodies.push_back(smallBall);
                }
             break;
        case GLFW_MOUSE_BUTTON_LEFT:
       if (greatBall && Renderer::IsPointInCircle(x, y, greatBall->position.x(), greatBall->position.y(), greatBall->GetRadius())) {
                isDragging = true;
            }
        if (bigBox && bigBox->shape->GetType() == BOX) {
        BoxShape* boxShape = static_cast<BoxShape*>(bigBox->shape);
       
        if (Renderer::IsPointInRotatedRect({static_cast<float>(x), static_cast<float>(y)}, bigBox->position, boxShape->width, boxShape->height, bigBox->rotation)) {
            std::cout<<"yes"<<"\n"; 
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

void Application::HandleWASDMovement(GLFWwindow* window, Body* body, float speed, float deltaTime) {
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        body->position.y() += speed * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        body->position.y() -= speed * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        body->position.x() -= speed * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        body->position.x() += speed * deltaTime;
}
