#version 330

uniform mat4 viewMatrix;
uniform mat4 projMatrix;
 
in vec3 in_position;
in vec2 in_texCoord;

out vec2 TexCoord;
 
void main(void)
{
	gl_Position = projMatrix * viewMatrix * vec4(in_position, 1.0);
	TexCoord = in_texCoord;
}
