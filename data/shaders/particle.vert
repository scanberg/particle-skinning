#version 330

#define MAX_WEIGHTS 4
#define	MAX_BONES 128

layout(location=0) in vec3 	in_position;
layout(location=1) in vec3 	in_oldPosition;
layout(location=2) in float in_mass;

layout(location=3) in vec3 	in_vertexPosition;
layout(location=4) in vec4	in_vertexWeight;

out vec3 	out_position;
out vec3 	out_oldPosition;
out float 	out_mass;

uniform float 	dt = 0.016;

uniform vec3 	externalForce	= vec3(0);
uniform vec3 	externalAcc		= vec3(0,0,0);

uniform float	randomForce		= 0;

uniform mat4 	modelMatrix;
uniform mat4 	invModelMatrix;
uniform mat4	boneMatrix[MAX_BONES];

uniform float	damping = .05;

int unpackIndex(float vw)
{
	return int(vw);
}

float unpackWeight(float vw)
{
	const float unpackScale = 1.0/0.99;
	return fract(vw) * unpackScale;
}

bool unusedWeight(float vw)
{
	return vw == 0.0;
}

void main(void)
{
	vec4 vertexPos = vec4(in_vertexPosition, 1);
	vec4 targetPosition = vec4(0);

	for(int i=0; i<MAX_WEIGHTS; ++i)
	{
		if(unusedWeight(in_vertexWeight[i]))
			break;

		int		index	= unpackIndex(in_vertexWeight[i]);
		float	weight	= unpackWeight(in_vertexWeight[i]);

		targetPosition += weight * boneMatrix[index] * vertexPos;
	}

	//targetPosition = modelMatrix * vec4(targetPosition.xyz, 1);

	// Perhaps implement a static max distance from position to targetposition
	// if the position is beyond this limit, move the position to the edge of this limit.

	vec3 pos = in_position;
	vec3 old = in_oldPosition;
	vec3 target = targetPosition.xyz;
	vec3 goPath = normalize(pos-target);


	vec3 attrForce = (target - pos) * 60.0;

	vec3 force = externalForce + attrForce;
	vec3 acc = externalAcc + force / in_mass;

	vec3 velocity = (1.0 - damping) * (pos - old) + acc * dt * dt;

	float dist = distance(target,pos);
	out_position = mix((in_position + velocity), target+goPath, 0.1*dist);
	out_oldPosition = mix(in_position, target+goPath-velocity, 0.1*dist);
	//out_oldPosition = in_position;
	//if(distance(target, pos) > 5.0) {
	//	out_position = target + 0.2*goPath;
	//	out_oldPosition = target + 0.21*goPath;
	//}

	out_mass = in_mass;
}
