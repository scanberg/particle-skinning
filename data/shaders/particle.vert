#version 330

// from http://stackoverflow.com/questions/4200224/random-noise-functions-for-glsl

// A single iteration of Bob Jenkins' One-At-A-Time hashing algorithm.
uint hash( uint x ) {
    x += ( x << 10u );
    x ^= ( x >>  6u );
    x += ( x <<  3u );
    x ^= ( x >> 11u );
    x += ( x << 15u );
    return x;
}

// Compound versions of the hashing algorithm I whipped together.
uint hash( uvec2 v ) { return hash( v.x ^ hash(v.y)                         ); }
uint hash( uvec3 v ) { return hash( v.x ^ hash(v.y) ^ hash(v.z)             ); }
uint hash( uvec4 v ) { return hash( v.x ^ hash(v.y) ^ hash(v.z) ^ hash(v.w) ); }

// Construct a float with half-open range [0:1] using low 23 bits.
// All zeroes yields 0.0, all ones yields the next smallest representable value below 1.0.
float floatConstruct( uint m ) {
    const uint ieeeMantissa = 0x007FFFFFu; // binary32 mantissa bitmask
    const uint ieeeOne      = 0x3F800000u; // 1.0 in IEEE binary32

    m &= ieeeMantissa;                     // Keep only mantissa bits (fractional part)
    m |= ieeeOne;                          // Add fractional part to 1.0

    float  f = uintBitsToFloat( m );       // Range [1:2]
    return f - 1.0;                        // Range [0:1]
}

// Pseudo-random value in half-open range [0:1].
float random( float x ) { return floatConstruct(hash(floatBitsToUint(x))); }
float random( vec2  v ) { return floatConstruct(hash(floatBitsToUint(v))); }
float random( vec3  v ) { return floatConstruct(hash(floatBitsToUint(v))); }
float random( vec4  v ) { return floatConstruct(hash(floatBitsToUint(v))); }

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

const int		MAX_BONES = 128;

uniform float 	dt = 0.016;

uniform vec3 	externalForce	= vec3(0);
uniform vec3 	externalAcc		= vec3(0,0,-9.82);

uniform float	randomForce		= 0;

uniform mat4 	modelMatrix;
uniform mat4 	invModelMatrix;
uniform mat4	boneMatrix[MAX_BONES];

uniform float damping = .95;

void main(void)
{
	vec4 targetPosition = vec4(0,0,0,0);

	for(int i=0; i<4; ++i)
	{
		if(in_vertexIndex[i] > -1)
			targetPosition += in_vertexWeight[i] * boneMatrix[in_vertexIndex[i]] * vec4(in_vertexPosition,1);
		else
			break;
	}

	targetPosition = modelMatrix * targetPosition;

	// Perhaps implement a static max distance from position to targetposition
	// if the position is beyond this limit, move the position to the edge of this limit.

	vec3 pos = in_position;
	vec3 old = in_oldPosition;
	vec3 target = targetPosition.xyz;

	//vec3 diff = length(target - pos);
	vec3 attrForce = (target - pos) * 100.1;

	float rf = randomForce * random( pos );
	vec3 force = externalForce + attrForce + rf * in_vertexNormal;
	vec3 acc = externalAcc + force / in_mass;

	out_position = (2-damping) * pos - (1-damping) * old + acc * dt * dt;
	//out_position = vec3(modelMatrix * vec4(in_position, 1));
	out_oldPosition = in_position;
	out_mass = in_mass;
}