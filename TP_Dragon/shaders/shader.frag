# version 120

uniform sampler2D texture0;

varying vec2 frag_texcoord0;

varying vec4 light_color;

void main()
{
	vec4 texel0 = texture2D( texture0, frag_texcoord0 );
  	gl_FragColor = (texel0 + light_color) / 2;
}
