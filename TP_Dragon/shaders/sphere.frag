# version 120

varying vec2 uvCoord;
uniform sampler2D texture0;
varying vec4 light_color;
 
void main(void) {
    gl_FragColor = (texture2D(texture0, uvCoord) + light_color) / 2;
}