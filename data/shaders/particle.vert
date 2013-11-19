#version 150

in vec3 	in_position;
in vec3 	in_oldPosition;
in float 	in_mass;

in vec3 	in_vertexPosition;
in vec3		in_vertexNormal;
in vec2 	in_vertexTexCoord;
in ivec4 	in_vertexIndex;
in vec4		in_vertexWeight;

out vec3 	out_position;
out vec3 	out_oldPosition;
out float 	out_mass;

uniform float 	dt 			= 0.08;
uniform vec3 	staticForce = vec3(0);
uniform vec3 	staticAcc 	= vec3(0,0,-9.82);

uniform mat4 	modelMatrix;
uniform mat4 	invModelMatrix;

const float damping = 0.02;

void main(void)
{
	// Perform skinning to get target position of vertex
	vec3 targetPosition = in_vertexPosition * 0.90;

	// Perhaps implement a static max distance from position to targetposition
	// if the position is beyond this limit, move the position to the edge of this limit.

	vec3 pos = in_position;
	vec3 old = in_oldPosition;

	//vec3 diff = length(targetPosition - pos);
	vec3 attrForce = (targetPosition - pos) * 10.05;

	vec3 force = staticForce + attrForce;
	vec3 acc = staticAcc + force / in_mass;

	out_position = (2-damping) * pos - (1-damping) * old + acc * dt * dt;
	out_oldPosition = in_position;
	out_mass = in_mass;
}