#include "Renderer.h"

// === Static Members Initialization for Rendering ===
glm::mat4 Renderer::projection;
glm::vec3 Renderer::color = glm::vec3(1.0f, 1.0f, 1.0f);

GLuint Renderer::shaderProgram = 0;
GLuint Renderer::circleVAO = 0, Renderer::circleVBO = 0;
GLuint Renderer::rectVAO = 0, Renderer::rectVBO = 0;

const float Renderer::rectVertices[8] = {
    -0.5f, -0.5f,
     0.5f, -0.5f,
     0.5f,  0.5f,
    -0.5f,  0.5f
};

const char* Renderer::vertexShaderSource = R"(
#version 330 core
layout(location = 0) in vec2 aPos;
uniform mat4 uModel;
uniform mat4 uProj;
void main() {
    gl_Position = uProj * uModel * vec4(aPos, 0.0, 1.0);
})";

const char* Renderer::fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;
uniform vec3 uColor;
void main() {
    FragColor = vec4(uColor, 1.0);
})";

// === Internal Utility Functions ===
std::vector<float> Renderer::generateCircleOutline(int segments) {
    std::vector<float> vertices;
    for (int i = 0; i < segments; ++i) {
        float theta = 2.0f * M_PI * i / segments;
        vertices.push_back(std::cos(theta));
        vertices.push_back(std::sin(theta));
    }
    return vertices;
}

GLuint Renderer::compileShaderProgram() {
    auto compile = [](GLenum type, const char* src) -> GLuint {
        GLuint shader = glCreateShader(type);
        glShaderSource(shader, 1, &src, nullptr);
        glCompileShader(shader);
        return shader;
    };

    GLuint vs = compile(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fs = compile(GL_FRAGMENT_SHADER, fragmentShaderSource);

    GLuint program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);
    return program;
}

void Renderer::InitRenderer(int screenWidth, int screenHeight) {
    projection = glm::ortho(0.0f, (float)screenWidth, (float)screenHeight, 0.0f, -1.0f, 1.0f);

    shaderProgram = compileShaderProgram();

    // Circle VAO
    std::vector<float> circle = generateCircleOutline(100);
    glGenVertexArrays(1, &circleVAO);
    glGenBuffers(1, &circleVBO);
    glBindVertexArray(circleVAO);
    glBindBuffer(GL_ARRAY_BUFFER, circleVBO);
    glBufferData(GL_ARRAY_BUFFER, circle.size() * sizeof(float), circle.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    // Rectangle VAO
    glGenVertexArrays(1, &rectVAO);
    glGenBuffers(1, &rectVBO);
    glBindVertexArray(rectVAO);
    glBindBuffer(GL_ARRAY_BUFFER, rectVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rectVertices), rectVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
}

void Renderer::UpdateProjection(int screenWidth, int screenHeight) {
    projection = glm::ortho(0.0f, float(screenWidth), float(screenHeight), 0.0f, -1.0f, 1.0f);
}

void Renderer::CleanupRenderer() {
    glDeleteBuffers(1, &circleVBO);
    glDeleteVertexArrays(1, &circleVAO);
    glDeleteBuffers(1, &rectVBO);
    glDeleteVertexArrays(1, &rectVAO);
    glDeleteProgram(shaderProgram);
}

// === Drawing Helpers ===
void Renderer::DrawCircle(Vec2 pos, float radius, glm::vec3 color) {
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(pos.x, pos.y, 0.0f));
    model = glm::scale(model, glm::vec3(radius));

    glUseProgram(shaderProgram);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "uModel"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "uProj"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniform3f(glGetUniformLocation(shaderProgram, "uColor"), color.r, color.g, color.b);

    glBindVertexArray(circleVAO);
    glDrawArrays(GL_LINE_LOOP, 0, 100);
}

void Renderer::DrawRectangle(Vec2 pos, float w, float h, glm::vec3 color, float angleRadians) {
     glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(pos.x, pos.y, 0.0f));
    model = glm::rotate(model, angleRadians, glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, glm::vec3(w, h, 1.0f));

    glUseProgram(shaderProgram);
    
    // Set uniforms for the shader
    GLint modelLoc = glGetUniformLocation(shaderProgram, "uModel");
    GLint projLoc = glGetUniformLocation(shaderProgram, "uProj");
    GLint colorLoc = glGetUniformLocation(shaderProgram, "uColor");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glUniform3f(colorLoc, color.r, color.g, color.b);

    // Bind the VAO and draw the rectangle as a line loop
    glBindVertexArray(rectVAO);
    glDrawArrays(GL_LINE_LOOP, 0, 4);

}


void Renderer::DrawPolygon(const std::vector<Vec2>& points, int count, glm::vec3 color) {
    std::vector<float> verts;
    for (int i = 0; i < count; ++i) {
        verts.push_back(points[i].x);
        verts.push_back(points[i].y);
    }

    GLuint vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), verts.data(), GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    glUseProgram(shaderProgram);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "uModel"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "uProj"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniform3f(glGetUniformLocation(shaderProgram, "uColor"), color.r, color.g, color.b);

    glDrawArrays(GL_LINE_LOOP, 0, count);

    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
}

void Renderer::DrawLine(Vec2 p1, Vec2 p2, glm::vec3 color) {
    float verts[] = {
        p1.x, p1.y,
        p2.x, p2.y
    };

    GLuint vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    glUseProgram(shaderProgram);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "uModel"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "uProj"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniform3f(glGetUniformLocation(shaderProgram, "uColor"), color.r, color.g, color.b);

    glDrawArrays(GL_LINES, 0, 2);

    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
}

void Renderer::DrawRect(int x, int y, int width, int height, glm::vec3 color) {
    float halfWidth = width / 2.0f;
    float halfHeight = height / 2.0f;

    Vec2 topLeft(x - halfWidth, y - halfHeight);
    Vec2 topRight(x + halfWidth, y - halfHeight);
    Vec2 bottomRight(x + halfWidth, y + halfHeight);
    Vec2 bottomLeft(x - halfWidth, y + halfHeight);

    DrawLine(topLeft, topRight, color);       // Top
    DrawLine(topRight, bottomRight, color);   // Right
    DrawLine(bottomRight, bottomLeft, color); // Bottom
    DrawLine(bottomLeft, topLeft, color);     // Left
}


/*======================================== HELPER FUNCTION UTILS ============================================*/

bool Renderer::IsPointInCircle(int pointX, int pointY, int circleX, int circleY, int radius) {
    // Calculate the squared distance between the point and the circle center
    int deltaX = pointX - circleX;
    int deltaY = pointY - circleY;
    int distanceSquared = deltaX * deltaX + deltaY * deltaY;
    
    // Return true if the mouse is inside the circle (distance <= radius)
    return distanceSquared <= (radius * radius);
}

bool Renderer::IsPointInBox(int pointX, int pointY, Body* body, BoxShape* boxShape) {
    // Simple AABB check, assumes the box is not rotated significantly
    float halfWidth = boxShape->width / 2.0f;
    float halfHeight = boxShape->height / 2.0f;
    
    return (pointX >= body->position.x - halfWidth && 
            pointX <= body->position.x + halfWidth &&
            pointY >= body->position.y - halfHeight && 
            pointY <= body->position.y + halfHeight);
}

Monitors Renderer::GetMonitor(GLFWwindow* window){
    Monitors monitor; 

    int windowX, windowY;
    glfwGetWindowPos(window, &windowX, &windowY);

    int monitorCount; 
    GLFWmonitor** monitors = glfwGetMonitors(&monitorCount);

    for (int i = 0; i < monitorCount; ++i) {
    int mx, my;
    glfwGetMonitorPos(monitors[i], &mx, &my);
    const GLFWvidmode* mode = glfwGetVideoMode(monitors[i]);
    
    int mw = mode->width;
    int mh = mode->height;

        // Check if window is within this monitor's bounds
        if (windowX >= mx && windowX < mx + mw &&
            windowY >= my && windowY < my + mh) {
            monitor.x = mw;
            monitor.y = mh;
            return monitor;
        }
    }

    // Return primary monitor if no match found 
    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    monitor.x = mode->width;
    monitor.y = mode->height;
    return monitor;
}