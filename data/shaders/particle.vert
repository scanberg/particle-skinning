#version 330

#define MAX_WEIGHTS 4
#define	MAX_BONES 128

layout(location=0) in vec3 	in_position;
layout(location=1) in vec3 	in_oldPosition;
layout(location=2) in vec3 	in_mass_k_d;

layout(location=3) in vec3 	in_vertexPosition;
layout(location=4) in vec4	in_vertexWeight;

out vec3 	out_position;
out vec3 	out_oldPosition;
out vec3 	out_mass_k_d;

uniform float 	dt = 0.016;

uniform vec3 	externalForce	= vec3(0);
uniform vec3 	externalAcc		= vec3(0,0,0);

uniform float	randomForce		= 0;

uniform mat4 	modelMatrix;
uniform mat4 	invModelMatrix;
uniform mat4	boneMatrix[MAX_BONES];

uniform float	damping = .1;
uniform float 	maxTolDist = 1.5;

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

	float mass 	= in_mass_k_d.x;
	float k 	= in_mass_k_d.y;
	float d 	= in_mass_k_d.z;

	vec3 pos = in_position;
	vec3 old = in_oldPosition;
	vec3 target = targetPosition.xyz;
	vec3 goPath = normalize(pos-target);
	float maxdist = 0.2f;

	vec3 attrForce = (target - pos) * k;

	vec3 force = externalForce + attrForce;
	vec3 acc = externalAcc + force / mass;
	vec3 velocity = (1.0 - d) * (pos - old) + acc * dt * dt;
	float ratio = distance(pos,target) - maxdist;
	
	pos = in_position + velocity;
	if(distance(target, pos) < maxdist) {
		out_position = pos;
		out_oldPosition = in_position;
	}else{
		vec3 maxDistPos = target + goPath*maxdist;
		out_position = mix(pos, maxDistPos, clamp(distance(maxDistPos,pos), 0.0, 1.0));
		out_oldPosition = out_position - velocity;
	}


	//out_position = in_position + velocity;
	//out_oldPosition = in_position;
	out_mass_k_d = in_mass_k_d;

/*
	vec3 	dir = out_position - target;	
	float 	len = length(dir);

	if(len > maxTolDist)
	{
		dir = dir/len;
		out_position = target + dir * maxTolDist;
		out_oldPosition = out_position;
	}*/
}
