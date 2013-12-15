#version 330
 
layout (lines) in;
layout (line_strip, max_vertices=2) out;
 
in vec3 	vs_position[];
in vec3 	vs_oldPosition[];
in float 	vs_mass[];
//in vec3 	vs_vertexPos[];

out vec3 	out_position;
out vec3 	out_oldPosition;
out float 	out_mass;
 
 void main()
{
	// vec3 restDelta = vs_vertexPos[1] - vs_vertexPos[0];
	// float restLength2 = dot(restDelta, restDelta);

	// vec3 delta = vs_position[1] - vs_position[0];
	// delta *= (1.0f - 2.0f * restLength2 / (restLength2 + dot(delta,delta))) * (vs_mass[0] + vs_mass[1]);

	// const float stiffness = 0.01;
	// vec3 val = stiffness * delta;

	// vec3 pos[2];
	// pos[0] = vs_position[0] + val / vs_mass[0];
	// pos[1] = vs_position[1] + val / vs_mass[1];

	for(int i = 0; i < 2; i++)
	{
		 // copy attributes
		out_position 	= vs_position[i];
		out_oldPosition = vs_oldPosition[i];
		out_mass		= vs_mass[i];

		// done with the vertex
		EmitVertex();
	}
	EndPrimitive();
}