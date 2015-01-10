#version 120
attribute vec2 in_Vertex;
attribute vec4 in_Color;
attribute vec2 in_MultiTexCoord0;

uniform mat4 in_Pmat;
uniform mat4 in_Vmat;
uniform mat4 in_Mmat;

uniform float gTime;

varying vec4 Color;
varying vec3 Pos;
varying vec2 Tex;

void main() {
	Pos = vec3(in_Vertex.x, in_Vertex.y, 0.0);
	Tex = vec2(in_MultiTexCoord0);
	vec4 vert = vec4(in_Vertex.x, in_Vertex.y, 0.1, 1.0);
	gl_Position = in_Pmat * in_Vmat * in_Mmat * vert;
	Color = in_Color;

}
