#version 150
uniform sampler2D frameTex;

in vec2 texCoord;
out vec4 fragColor;

void main() 
{
	fragColor = texture(frameTex, texCoord);
};