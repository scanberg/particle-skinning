#version 150

in vec3 	in_position;
in vec3 	in_oldPosition;
in float 	in_mass;

out vec3 	out_position;
out vec3 	out_oldPosition;
out float 	out_mass;

uniform float 	dt 			= 0.08;
uniform vec3 	staticForce = vec3(0);
uniform vec3 	staticAcc 	= vec3(0,-0.1,0);
 
void main(void)
{
	// Perform skinning to get target position of vertex
	vec3 targetPosition = vec3(0);

	vec3 force = staticForce;
	vec3 acc = staticAcc + force / in_mass;

	vec3 pos = in_position;
	vec3 old = in_oldPosition;

	out_position = 2 * pos - old + acc * dt * dt + vec3(0,-1,0);
	out_oldPosition = in_position;
	out_mass = in_mass;

	out_position = vec3(0);
	out_oldPosition = vec3(0);
}