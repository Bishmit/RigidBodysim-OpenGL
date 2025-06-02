#include "Application.h"

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }
    
    // Configure GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    // Create window

    GLFWwindow* window = glfwCreateWindow(800, 600, "Rigid Body Physics Simulation", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(window);
    
    glfwSetFramebufferSizeCallback(window, Application::framebuffer_size_callback);
    
    // Register the callback
    glfwSetMouseButtonCallback(window, Application::MouseButtonCallBack);
    glfwSwapInterval(1);
    
    Application::Init(window);
    Application::SetUp();

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        Application::Update(window);
        Application::Render();  
        Application::ClearScene(window); 
        glfwSwapBuffers(window);
    }
    
    // Cleanup
    Application::Shutdown();
    Application::Destroy();
    
    glfwDestroyWindow(window);
    glfwTerminate();
    
    return 0;
}