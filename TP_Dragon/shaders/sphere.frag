# version 120

varying vec2 uvCoord;
uniform sampler2D texture0;
varying vec4 light_color;
 
void main(void) {
	const float fogDensity = 0.02;
	const float LOG2 = 1.442695;
	const vec4 fogColor = vec4(0.5, 0.5, 0.5, 1.0);

	float z = gl_FragCoord.z / gl_FragCoord.w;
	float fogFactor = exp2(-fogDensity * fogDensity * z * z * LOG2);
	fogFactor = clamp(fogFactor, 0.0, 1.0);

    gl_FragColor = (texture2D(texture0, uvCoord) + light_color) / 2;
  	gl_FragColor = mix(fogColor, gl_FragColor, fogFactor);
}