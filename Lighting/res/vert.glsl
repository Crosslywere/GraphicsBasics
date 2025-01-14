#version 330 core
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;
out vec3 Position;
out vec2 TexCoord;
out vec3 Normal;
uniform mat4 uProj;
uniform mat4 uView;
uniform mat4 uModel;
void main() {
    Position = vec3(uModel * vec4(aPosition, 1.));
    TexCoord = aTexCoord;
    Normal = normalize(mat3(transpose(inverse(uModel))) * aNormal);
    gl_Position = uProj * uView * uModel * vec4(aPosition, 1.);
}
