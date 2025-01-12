#version 330 core
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aColor;
out vec2 TexCoord;
out vec3 Color;
uniform mat4 uProj;
uniform mat4 uModel;
void main() {
	TexCoord = aTexCoord;
	Color = aColor;
	gl_Position = uProj * uModel * vec4(aPosition, 1.);
}