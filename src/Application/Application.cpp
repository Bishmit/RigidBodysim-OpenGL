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

    // Update dragged body position to follow cursor
    if (draggedBody) {
        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);

        int winW, winH, fbW, fbH;
        glfwGetWindowSize(window, &winW, &winH);
        glfwGetFramebufferSize(window, &fbW, &fbH);
        mouseX = mouseX * fbW / winW;
        mouseY = mouseY * fbH / winH;

        Vec2 targetPos = {
            (float)(mouseX - dragOffset.x),
            (float)(mouseY - dragOffset.y)
        };

        // Derive velocity from mouse movement so collision impulses transfer correctly
        if (deltaTime > 0.0f) {
            draggedBody->velocity.x = (targetPos.x - draggedBody->position.x) / deltaTime;
            draggedBody->velocity.y = (targetPos.y - draggedBody->position.y) / deltaTime;
        }
        draggedBody->position = targetPos;
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

    SimContext ctx {
        pause, showNormal, showCollisionPoint, attachPendulum,
        isRecentBodySelected, showSavedToast, showLoadFailToast, showOverwriteModal,
        gravity, restiutionValue, frictionValue, correctionValue, radius_, toastTimer,
        maxIteration,
        bodies, greatBall, recentSelectedBody,
        stateName, pendingFilepath, newSaveName,
        [](const std::string& fp){ SaveState(fp); },
        [](const std::string& fp){ LoadState(fp); },
        []{ return ClearDynamicObjectOnScreen(); },
        [](Body* b){ return DeleteParticularBody(b); }
    };
    GUI::Render(window, ctx);

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

    // Scale logical coords → framebuffer coords (fixes Retina/HiDPI offset on macOS)
    int winW, winH, fbW, fbH;
    glfwGetWindowSize(window, &winW, &winH);
    glfwGetFramebufferSize(window, &fbW, &fbH);
    x = x * fbW / winW;
    y = y * fbH / winH;

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
    auto it = std::remove_if(bodies.begin(), bodies.end(),
        [](Body* body) {
            if (!body->IsStatic() && (
                body->position.x < -400.f ||
                body->position.x > screenWidth  + 400.f ||
                body->position.y < -400.f ||
                body->position.y > screenHeight + 400.f)) {
                if (body == draggedBody)       { draggedBody = nullptr; isDragging = false; }
                if (body == recentSelectedBody){ recentSelectedBody = nullptr; isRecentBodySelected = false; }
                if (body == greatBall)          { greatBall = nullptr; }
                delete body;
                return true;
            }
            return false;
        });

    if (it != bodies.end())
        bodies.erase(it, bodies.end());
}

bool Application::ClearDynamicObjectOnScreen() {
    auto it = std::remove_if(bodies.begin(), bodies.end(), [](Body* body) {
        if (!body) return false;

        if (!body->IsStatic()) {
            if (body == draggedBody)       { draggedBody = nullptr; isDragging = false; }
            if (body == recentSelectedBody){ recentSelectedBody = nullptr; isRecentBodySelected = false; }
            if (body == greatBall)          { greatBall = nullptr; }
            delete body;
            return true;
        }

        return false;
    });

    bodies.erase(it, bodies.end());
    return true;
}

bool Application::DeleteParticularBody(Body* body) {
   if(!body)
        return false;

    auto it = std::find(bodies.begin(), bodies.end(), body);
    if (it != bodies.end()) {
        if (body == draggedBody)  { draggedBody = nullptr; isDragging = false; }
        if (body == greatBall)     { greatBall = nullptr; }
        delete *it;
        bodies.erase(it);
        recentSelectedBody = nullptr;
        isRecentBodySelected = false;
        return true;
    }
    return false;
}





