#version 330 core
layout (location = 0) out vec4 oFragColor;
uniform vec3 color;
void main() {
    oFragColor = vec4(color, 1.);
}
