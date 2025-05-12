#include "renderer.h"

// === Static Members Initialization ===
float Renderer::radius = 50.0f;
float Renderer::width = 100.0f;
float Renderer::height = 50.0f;

std::vector<Body*> Renderer::bodies;
Body* Renderer::greatBall = nullptr;
float Renderer::radius_ = 0.0f;

GLuint Renderer::shaderProgram = 0;
GLuint Renderer::circleVAO = 0, Renderer::circleVBO = 0;
GLuint Renderer::rectVAO = 0, Renderer::rectVBO = 0;

const float Renderer::rectVertices[8] = {
    -0.5f, -0.5f, 0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,  0.5f
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

// === Core Methods ===
void Renderer::Init(GLFWwindow* window) {
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        return;
    }

    glViewport(0, 0, screenWidth, screenHeight);
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

void Renderer::SetUp() {
    greatBall = new Body(CircleShape(150), 400, 300);
    bodies.push_back(greatBall);
    radius_ = greatBall->GetRadius();
}

void Renderer::Update(GLFWwindow* window) {
    glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    for (auto body : bodies) {
        if (body->shape->GetType() == CIRCLE) {
            CircleShape* circle = static_cast<CircleShape*>(body->shape);
            DrawCircle(body->position, circle->radius, {1.0f, 0.5f, 0.2f});
        }
    }
}

void Renderer::RenderGUI(){
    ImGui::Begin("Renderer Demo");
    if (ImGui::SliderFloat("Circle Radius", &radius_, 10.0f, 200.0f)) {
        greatBall->SetRadius(radius_);
    }
    ImGui::End();
}

void Renderer::Shutdown() {
    glDeleteBuffers(1, &circleVBO);
    glDeleteVertexArrays(1, &circleVAO);
    glDeleteBuffers(1, &rectVBO);
    glDeleteVertexArrays(1, &rectVAO);
    glDeleteProgram(shaderProgram);
}

// === Drawing Helpers ===
void Renderer::DrawCircle(glm::vec2 pos, float radius, glm::vec3 color) {
    glm::mat4 proj = glm::ortho(0.0f, (float)screenWidth, (float)screenHeight, 0.0f);
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(pos, 0.0f));
    model = glm::scale(model, glm::vec3(radius));

    glUseProgram(shaderProgram);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "uModel"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "uProj"), 1, GL_FALSE, glm::value_ptr(proj));
    glUniform3f(glGetUniformLocation(shaderProgram, "uColor"), color.r, color.g, color.b);

    glBindVertexArray(circleVAO);
    glDrawArrays(GL_LINE_LOOP, 0, 100);
}

void Renderer::DrawRectangle(glm::vec2 pos, float w, float h, glm::vec3 color) {
    glm::mat4 proj = glm::ortho(0.0f, (float)screenWidth, (float)screenHeight, 0.0f);
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(pos, 0.0f));
    model = glm::scale(model, glm::vec3(w, h, 1.0f));

    glUseProgram(shaderProgram);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "uModel"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "uProj"), 1, GL_FALSE, glm::value_ptr(proj));
    glUniform3f(glGetUniformLocation(shaderProgram, "uColor"), color.r, color.g, color.b);

    glBindVertexArray(rectVAO);
    glDrawArrays(GL_LINE_LOOP, 0, 4);
}

void Renderer::DrawPolygon(const glm::vec2* points, int count, glm::vec3 color) {
    glm::mat4 proj = glm::ortho(0.0f, (float)screenWidth, (float)screenHeight, 0.0f);
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
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "uProj"), 1, GL_FALSE, glm::value_ptr(proj));
    glUniform3f(glGetUniformLocation(shaderProgram, "uColor"), color.r, color.g, color.b);

    glDrawArrays(GL_LINE_LOOP, 0, count);

    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
}
