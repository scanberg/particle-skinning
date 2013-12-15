#version 330

layout(location=0) in vec3 	in_position;
layout(location=1) in vec3 	in_oldPosition;
layout(location=2) in float in_mass;

layout(location=3) in vec3 in_vertexPosition;

out vec3 	vs_position;
out vec3 	vs_oldPosition;
out float 	vs_mass;
//out vec3	vs_vertexPos;

void main(void)
{
	vs_position 	= in_position;
	vs_oldPosition 	= in_oldPosition;
	vs_mass 		= in_mass;
	//vs_vertexPos 	= in_vertexPosition;
}