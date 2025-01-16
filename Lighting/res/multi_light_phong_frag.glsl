#version 330 core
#define POINT_LIGHT_COUNT 3
layout (location = 0) out vec4 oFragColor;
in vec3 Position;
in vec2 TexCoord;
in vec3 Normal;
uniform struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
} uMaterial;
uniform struct DirectionalLight {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 direction;
} uDirectionalLight;
uniform struct PointLight {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 position;
    float constant;
    float linear;
    float quadratic;
} uPointLights[POINT_LIGHT_COUNT];
uniform struct FlashLight {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 position;
    vec3 direction;
    float innerCutoff;
    float outerCutoff;
    // Attenuation
    float constant;
    float linear;
    float quadratic;
} uFlashLight;
uniform vec3 uCamPos;
uniform int uPointLightCount;
vec3 CalculateDirectionalLight(in vec3 diffuseFragColor, in vec3 specularFragColor) {
    vec3 ambient = uDirectionalLight.ambient * diffuseFragColor;
    // Diffuse light calculation
    vec3 lightDir = normalize(-uDirectionalLight.direction);
    float diff = max(dot(Normal, lightDir), 0.);
    vec3 diffuse = diff * uDirectionalLight.diffuse * diffuseFragColor;
    // Specular reflection calculation
    vec3 camDir = normalize(uCamPos - Position);
    vec3 reflectDir = reflect(-lightDir, Normal);
    float spec = pow(max(dot(camDir, reflectDir), 0.), uMaterial.shininess);
    vec3 specular = spec * uDirectionalLight.specular * specularFragColor;
    return ambient + diffuse + specular;
}
vec3 CalculatePointLight(in int index, in vec3 diffuseFragColor, in vec3 specularFragColor) {
    float dist = distance(uPointLights[index].position, Position);
    float attenuation = uPointLights[index].constant + (uPointLights[index].linear * dist) + (uPointLights[index].quadratic * (dist * dist));
    attenuation = 1. / attenuation;

    vec3 ambient = uPointLights[index].ambient * diffuseFragColor;

    vec3 lightDir = normalize(uPointLights[index].position - Position);
    float diff = max(dot(Normal, lightDir), 0.);
    vec3 diffuse = diff * uPointLights[index].diffuse * diffuseFragColor;

    vec3 camDir = normalize(uCamPos - Position);
    vec3 reflectDir = reflect(-lightDir, Normal);
    float spec = pow(max(dot(camDir, reflectDir), 0.), uMaterial.shininess);
    vec3 specular = spec * uPointLights[index].specular * specularFragColor;
    return attenuation * (ambient + diffuse + specular);
}
vec3 CalculateSpotLight(in vec3 diffuseFragColor, in vec3 specularFragColor) {
    vec3 lightDir = normalize(uFlashLight.position - Position);
    float theta = dot(lightDir, normalize(-uFlashLight.direction));
    vec3 ambient = uFlashLight.ambient * diffuseFragColor;
    if (theta > uFlashLight.outerCutoff) {
        float dist = length(uFlashLight.position - Position);
        float attenuation = uFlashLight.constant + (uFlashLight.linear * dist) + (uFlashLight.quadratic * (dist * dist));
        attenuation = 1. / attenuation;
        float epsilon = uFlashLight.innerCutoff - uFlashLight.outerCutoff;
        float intensity = clamp((theta - uFlashLight.outerCutoff) / epsilon, 0., 1.);

        float diff = max(dot(Normal, lightDir), 0.);
        vec3 diffuse = intensity * diff * uFlashLight.diffuse * diffuseFragColor;

        vec3 camDir = normalize(uCamPos - Position);
        vec3 reflectDir = reflect(-lightDir, Normal);
        float spec = pow(max(dot(camDir, reflectDir), 0.), uMaterial.shininess);
        vec3 specular = intensity * spec * uFlashLight.specular * specularFragColor;
        return ambient + attenuation * (diffuse + specular);
    }
    return ambient;
}
void main() {
    vec3 diffuseFragColor = texture2D(uMaterial.diffuse, TexCoord).rgb;
    vec3 specularFragColor = texture2D(uMaterial.specular, TexCoord).rgb;
    // Summation lights in the scene
    vec3 color = CalculateDirectionalLight(diffuseFragColor, specularFragColor);
    int size = min(uPointLightCount, POINT_LIGHT_COUNT);
    for (int i = 0; i < size; i++) {
        color += CalculatePointLight(i, diffuseFragColor, specularFragColor);
    }
    color += CalculateSpotLight(diffuseFragColor, specularFragColor);
    oFragColor = vec4(color, 1.);
}
