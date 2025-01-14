#version 330 core
layout (location = 0) out vec4 oFragColor;
in vec3 Position;
in vec2 TexCoord;
in vec3 Normal;
uniform vec3 camPos;
uniform vec3 color;
uniform vec3 lightPos;
uniform vec3 lightColor;
void main() {
    vec3 lightDir = normalize(lightPos - Normal);
    vec3 camDir = normalize(camPos - Position);
    vec3 reflectDir = reflect(-lightDir, Normal);
    // Calculating ambient light
    vec3 ambient = color * 0.1 * lightColor;
    // Calculating diffuse light
    float diff = max(dot(Normal, lightDir), 0.);
    vec3 diffuse = diff * color * lightColor;
    // Calculating specular light
    float spec = pow(max(dot(camDir, reflectDir), 0.), 4);
    vec3 specular = .5 * spec * lightColor;
    // Putting together the result
    oFragColor = vec4(ambient + diffuse + specular, 1.);
}
