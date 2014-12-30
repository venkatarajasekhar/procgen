varying vec4 Color;
varying vec3 Normal;
varying vec3 Pos;
varying vec2 Tex;
uniform sampler2D gSampler;
//uniform float gTime;

varying vec3 diffuse;

float rand(vec2 co){
	return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}
vec3 noise( vec3 inval ) {
	vec3 pos=floor(10.0*inval);
	return vec3( rand(pos.xy), rand(pos.xz), rand(pos.yz) );
}

void main() {
	vec3 N = normalize(Normal.rgb);
	vec4 textureColor = texture2D(gSampler, Tex);
	vec3 lightFrom = normalize( vec3( -4.0,2.0,4.0 ) );
	float lightEffect = dot( N, lightFrom );
	lightEffect = ( 0.1 + (lightEffect) ) / 1.0;
	//vec3 diffuse = Color.xyz * lightEffect;
	//gl_FragColor = textureColor * vec4(diffuse,1.0);
	vec3 diffuse = textureColor.rgb * lightEffect;
	gl_FragColor.rgb = diffuse;
	//gl_FragColor.rgb = textureColor.rgb * 1.0;
	gl_FragColor.a = textureColor.a;
	//gl_FragColor.rgb = mod( ( N.rgb + vec3( 1.0 ) ) * 0.5, vec3(1.0) );
	//gl_FragColor.rgb = vec3( N.r * 0.5 + 0.5 );
	//gl_FragColor.rgb = vec3(0.5) + N * 0.0625;
	//gl_FragColor = textureColor * vec4(Color.xyz,1.0);
	//gl_FragColor = vec4( noise( Pos.xyz ), gl_FragColor.w );

	//gl_FragColor = vec4(vec3(pow(1-abs(Normal.z),3)),1.0);
	//gl_FragColor = vec4(diffuse,1.0);
	//gl_FragColor.xyz = Color.xyz;
}
