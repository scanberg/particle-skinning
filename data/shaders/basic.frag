#version 330

out vec4 out_frag0;

in vec2 TexCoord;

void main(void)
{
	vec2 texCoord = TexCoord;
	out_frag0 = vec4(vec3(1,0,0), 1.0);
}
