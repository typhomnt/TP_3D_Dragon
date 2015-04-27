# version 120


// Coordonnées de texture
varying vec2 uvCoord;

// Texture à appliquer
uniform sampler2D texture0;

// Couleur de la lumière
varying vec4 light_color;


 
/////////////////////////////////////////////////////////////////////////////// 
void main(void) {

	const float fogDensity = 0.007;					// Densité du brouillard
	const vec4 fogColor = vec4(0.5, 0.5, 0.5, 1.0); // Couleur du brouillard
	const float LOG2 = 1.442695;					

	// On choisit un brouillard de type exponentiel, variant avec la distance
	float z = gl_FragCoord.z / gl_FragCoord.w;
	float fogFactor = exp2(-fogDensity * fogDensity * z * z * LOG2);
	fogFactor = clamp(fogFactor, 0.0, 1.0);

	// La couleur finale est un mix entre la couleur de la texture, la couleur
	// de la lumière et la couleur du brouillard
    gl_FragColor = (texture2D(texture0, uvCoord) + light_color) / 2;
  	gl_FragColor = mix(fogColor, gl_FragColor, fogFactor);
}