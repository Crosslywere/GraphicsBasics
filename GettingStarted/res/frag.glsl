#version 330 core
layout (location = 0) out vec4 oFragColor;
in vec2 TexCoord;
in vec3 Color;
uniform sampler2D uTexture1;
uniform sampler2D uTexture2;
uniform float uTime;
void main() {
	float t = sin(uTime * 4.);
	vec4 woodTexture = texture2D(uTexture1, TexCoord);
	vec4 awesomeTexture;
	if (t > 0) {
		vec2 awesomeFaceTexCoord = vec2(abs(TexCoord.x - 1.), TexCoord.y);
		awesomeTexture = texture2D(uTexture2, awesomeFaceTexCoord);
	}
	else {
		awesomeTexture = texture2D(uTexture2, TexCoord);
	}
	oFragColor = vec4(mix(woodTexture, awesomeTexture, .5).rgb * Color, 1.0);

}