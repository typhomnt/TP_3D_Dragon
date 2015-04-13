# version 120

uniform sampler2D texture0;

varying vec2 frag_texcoord0;

varying vec4 light_color;


void main()
{
	const float fogDensity = 0.007;
	const float LOG2 = 1.442695;
	const vec4 fogColor = vec4(0.5, 0.5, 0.5, 1.0);

	float z = gl_FragCoord.z / gl_FragCoord.w;
	float fogFactor = exp2(-fogDensity * fogDensity * z * z * LOG2);
	fogFactor = clamp(fogFactor, 0.0, 1.0);

	vec4 texel0 = texture2D( texture0, frag_texcoord0 );
  	gl_FragColor = (texel0 + light_color) / 2;
  	gl_FragColor = mix(fogColor, gl_FragColor, fogFactor);
}
