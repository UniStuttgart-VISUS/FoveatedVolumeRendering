#version 430
layout(location = 0) in highp vec3 vertex;
out highp vec2 texCoord;
uniform mat4 projMatrix;
uniform mat4 mvMatrix;
void main() {
	texCoord = vec2(0.5f) + 0.5f * vertex.xy;
	gl_Position = projMatrix * mvMatrix * vec4(vertex.xy, 1.0f, 1.0f);
};