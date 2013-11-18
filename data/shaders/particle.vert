#version 150

in vec3 	in_oldPosition;
in vec3 	in_position;
in float 	in_mass;

out vec3 	out_oldPosition;
out vec3 	out_position;
out float 	out_mass;

uniform float 	dt 			= 0.08;
uniform vec3 	staticForce = vec3(0);
uniform vec3 	staticAcc 	= vec3(0);
 
void main(void)
{
	// Perform skinning to get target position of vertex
	vec3 targetPosition = vec3(0);

	vec3 force = staticForce;
	vec3 acc = staticAcc + force / in_mass;

	out_position = 2 * in_position - in_oldPosition + acc * dt * dt;
	out_oldPosition = in_position;
	out_mass = in_mass;
}