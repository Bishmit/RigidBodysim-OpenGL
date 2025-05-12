#version 330 core
out vec4 FragColor;

uniform vec3 color;
in vec2 localPos;

void main() {
    if (length(localPos) > 1.0) discard;
    FragColor = vec4(color, 1.0);
}
