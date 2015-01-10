varying vec4 Color;
varying vec3 Pos;
varying vec2 Tex;
uniform sampler2D gSampler;
uniform float gTime;

varying vec3 diffuse;

void main() {
	vec4 textureColor = texture2D(gSampler, Tex);
	gl_FragColor = textureColor * Color;
	//gl_FragColor.xyz = vec3(1.0);
}
