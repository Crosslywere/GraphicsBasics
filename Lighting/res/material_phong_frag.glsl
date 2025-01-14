#version 330 core
layout (location = 0) out vec4 oFragColor;
in vec3 Position;
in vec2 TexCoord;
in vec3 Normal;
uniform struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
} material;
uniform struct Light {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 position;
} light;
uniform vec3 camPos;
void main() {
    vec3 diffuseFragColor = texture2D(material.diffuse, TexCoord).rgb;
    vec3 specularFragColor = texture2D(material.specular, TexCoord).rgb;

    vec3 lightDir = normalize(light.position - Normal);
    vec3 camDir = normalize(camPos - Position);
    vec3 reflectDir = reflect(-lightDir, Normal);
    // Calculating ambient light
    vec3 ambient = diffuseFragColor * light.ambient;
    // Calculating diffuse light
    float diff = max(dot(Normal, lightDir), 0.);
    vec3 diffuse = diff * diffuseFragColor * light.diffuse;
    // Calculating specular light
    float spec = pow(max(dot(camDir, reflectDir), 0.), material.shininess);
    vec3 specular = spec * specularFragColor * light.specular;
    oFragColor = vec4(ambient + diffuse + specular, 1.);
}
