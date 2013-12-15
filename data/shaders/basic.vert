#version 330

uniform mat4 viewMatrix;
uniform mat4 projMatrix;
 
in vec3 in_vertexPosition;
in vec2 in_vertexTexCoord;
in vec3 in_vertexNormal;

out vec2 TexCoord;
out vec3 Normal;
 
void main(void)
{
	gl_Position = projMatrix * viewMatrix * vec4(in_vertexPosition, 1.0);
	TexCoord = in_vertexTexCoord;
	Normal = in_vertexNormal;
}